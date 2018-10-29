/**	@file
*	@author Bram de Greve (bram@cocamware.com)
*	@author Tom De Muer (tom@cocamware.com)
*
*	*** BEGIN LICENSE INFORMATION ***
*
*	The contents of this file are subject to the Common Public Attribution License
*	Version 1.0 (the "License"); you may not use this file except in compliance with
*	the License. You may obtain a copy of the License at
*	http://lass.sourceforge.net/cpal-license. The License is based on the
*	Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover
*	use of software over a computer network and provide for limited attribution for
*	the Original Developer. In addition, Exhibit A has been modified to be consistent
*	with Exhibit B.
*
*	Software distributed under the License is distributed on an "AS IS" basis, WITHOUT
*	WARRANTY OF ANY KIND, either express or implied. See the License for the specific
*	language governing rights and limitations under the License.
*
*	The Original Code is LASS - Library of Assembled Shared Sources.
*
*	The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
*	The Original Developer is the Initial Developer.
*
*	All portions of the code written by the Initial Developer are:
*	Copyright (C) 2004-2013 the Initial Developer.
*	All Rights Reserved.
*
*	Contributor(s):
*
*	Alternatively, the contents of this file may be used under the terms of the
*	GNU General Public License Version 2 or later (the GPL), in which case the
*	provisions of GPL are applicable instead of those above.  If you wish to allow use
*	of your version of this file only under the terms of the GPL and not to allow
*	others to use your version of this file under the CPAL, indicate your decision by
*	deleting the provisions above and replace them with the notice and other
*	provisions required by the GPL License. If you do not delete the provisions above,
*	a recipient may use your version of this file under either the CPAL or the GPL.
*
*	*** END LICENSE INFORMATION ***
*/

#include "lass_common.h"
#include "subprocess.h"
#include "wchar_support.h"

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32

#include "../stde/extended_string.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace lass
{
namespace util
{
namespace impl
{

class SubprocessImpl
{
public:
    typedef experimental::Subprocess::TArgs TArgs;
    typedef experimental::Subprocess::TWArgs TWArgs;

    SubprocessImpl(const TArgs& args)
    {
        for (TArgs::const_iterator i = args.begin(); i != args.end(); ++i)
        {
            wargs_.push_back(util::utf8ToWchar(*i));
        }
    }

    SubprocessImpl(const TWArgs& wargs):
        wargs_(wargs)
    {
    }

    ~SubprocessImpl()
    {
        ::CloseHandle(pi_.hProcess);
        ::CloseHandle(pi_.hThread);
    }

    void run()
    {
        if (wargs_.empty())
        {
            LASS_THROW("You need to specify at least one argument: the binary to start.");
        }

        ZeroMemory(&si_, sizeof(si_));
        si_.cb = sizeof(si_);
        si_.dwFlags = STARTF_USESHOWWINDOW;
        si_.wShowWindow = SW_HIDE;
        ZeroMemory(&pi_, sizeof(pi_));

        std::vector<wchar_t> cmd;
        makeCommandline(cmd);

        LASS_ENFORCE_WINAPI(::CreateProcessW(wargs_[0].c_str(), &cmd[0], 0, 0, FALSE, 0, 0, 0, &si_, &pi_))(&cmd[0]);
    }

    void sendSignal(int /*signal*/)
    {
        // not implemented;
        LASS_ASSERT(false);
    }

    void kill()
    {
        ::TerminateProcess(pi_.hProcess, 0xffffffff);
    }

    bool checkExitCode(int& exitCode, bool join) const
    {
        if (join && ::WaitForSingleObject(pi_.hProcess, INFINITE) == WAIT_FAILED)
        {
            const unsigned err = lass_GetLastError();
            LASS_THROW("Failed to wait for subprocess: (" << err << ") " << lass_FormatMessage(err));
        }
        DWORD value;
        LASS_ENFORCE_WINAPI(::GetExitCodeProcess(pi_.hProcess, &value));
        if (value == STILL_ACTIVE)
        {
            return false;
        }
        exitCode = static_cast<int>(value);
        return true;
    }


private:

    void makeCommandline(std::vector<wchar_t>& cmd) const
    {
        // this is jolly good fun ...
        cmd.clear();
        for (TWArgs::const_iterator i = wargs_.begin(); i != wargs_.end(); ++i)
        {
            if (i != wargs_.begin())
            {
                cmd.push_back(L' ');
            }
            const std::wstring& arg = *i;
            const bool mustQuote = arg.empty() || arg.find_first_of(L" \t") != std::wstring::npos;
            if (mustQuote)
            {
                cmd.push_back(L'"');
            }
            size_t numBackslashes = 0;
            for (size_t k = 0, n = arg.size(); k < n; ++k)
            {
                switch (arg[k])
                {
                case L'\\':
                    ++numBackslashes;
                    break;
                case L'"':
                    // double preceding backslashes, and add one more to escape me
                    numBackslashes = 2 * numBackslashes + 1;
                    // and fall through
                default:
                    cmd.insert(cmd.end(), numBackslashes, L'\\');
                    numBackslashes = 0;
                    cmd.push_back(arg[k]);
                }
            }
            cmd.insert(cmd.end(), numBackslashes, L'\\'); // remaining backslashes ...
            if (mustQuote)
            {
                cmd.insert(cmd.end(), numBackslashes, L'\\'); // must be doubled, but don't escape theq final quote.
                cmd.push_back(L'"');
            }
        }
        cmd.push_back(0);
    }

    STARTUPINFOW si_;
    PROCESS_INFORMATION pi_;
    TWArgs wargs_;
};

}
}
}

#else

#include <string.h> // strdup
#include <signal.h>
#include <spawn.h> // posix_spawn
#include <sys/wait.h> // waitpid

extern char **environ;

namespace lass
{
namespace util
{
namespace impl
{

class SubprocessImpl
{
public:
    typedef experimental::Subprocess::TArgs TArgs;
    typedef experimental::Subprocess::TWArgs TWArgs;

    SubprocessImpl(const TArgs& args):
        args_(args),
        pid_(0)
    {
    }

    SubprocessImpl(const TWArgs& wargs):
        pid_(0)
    {
        for (TWArgs::const_iterator i = wargs.begin(); i != wargs.end(); ++i)
        {
            args_.push_back(util::wcharToUtf8(*i));
        }
    }

    ~SubprocessImpl()
    {
        if (pid_)
        {
            // last chance to waitpid to avoid zombiestate.
            int dummy;
            checkExitCode(dummy, false);
        }
    }

    void run()
    {
        LASS_ASSERT(!pid_);

        if (args_.empty())
        {
            LASS_THROW("You need to specify at least one argument: the binary to start.");
        }

        const size_t argc = args_.size();
        ScopedArgv argv(argc);
        for (size_t k = 0; k < argc; ++k)
        {
            argv[k] = ::strdup(args_[k].c_str());
        }

        LASS_ENFORCE_CLIB_RC(posix_spawnp(&pid_, argv[0], 0, 0, argv.ptr(), environ));
    }

    void sendSignal(int signal)
    {
        if (pid_)
        {
            ::kill(pid_, signal);
        }
    }

    void kill()
    {
        sendSignal(SIGKILL);
    }

    bool checkExitCode(int& exitCode, bool join)
    {
        int status;
        const int options = join ? 0 : WNOHANG;

        if (!pid_)
        {
            return false; // not running yet, or already has exit code.
        }

        const pid_t pid = LASS_ENFORCE_CLIB(waitpid(pid_, &status, options));
        if (pid == 0)
        {
            return false; // still running
        }
        LASS_ASSERT(pid == pid_);

        if (WIFSIGNALED(status))
        {
            exitCode = -(WTERMSIG(status));
            pid_ = 0;
            return true;
        }
        else if (WIFEXITED(status))
        {
            exitCode = WEXITSTATUS(status);
            pid_ = 0;
            return true;
        }
        else
        {
            LASS_THROW("Internal error"); // uh-oh
        }
    }

private:

    class ScopedArgv
    {
    public:
        ScopedArgv(size_t argc)
        {
            argv_.resize(argc + 1, 0);
        }
        ~ScopedArgv()
        {
            for (TArgv::reverse_iterator i = argv_.rbegin(), end = argv_.rend(); i != end; ++i)
            {
                ::free(*i);
            }
        }
        char*& operator[](size_t index)
        {
            return argv_[index];
        }
        char** ptr()
        {
            return &argv_[0];
        }
    private:
        typedef std::vector<char*> TArgv;
        TArgv argv_;
    };

    TArgs args_;
    pid_t pid_;
};

}
}
}

#endif

// --- Subprocess ------------------------------------------------------------------------------------------------------

namespace lass
{
namespace util
{
namespace experimental
{

Subprocess::Subprocess(const TArgs& args) :
    pimpl_(new impl::SubprocessImpl(args)),
    exitCode_(0),
    hasExitCode_(false),
    isStarted_(false),
    isDetached_(false)
{
}


Subprocess::Subprocess(const TWArgs& args) :
    pimpl_(new impl::SubprocessImpl(args)),
    exitCode_(0),
    hasExitCode_(false),
    isStarted_(false),
    isDetached_(false)
{
}


Subprocess::~Subprocess()
{
    try
    {
        if (!isDetached_)
        {
            pimpl_->kill();
        }
    }
    catch (const std::exception& error)
    {
        std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: " << error.what() << std::endl;
    }
    delete pimpl_;
}


void Subprocess::run()
{
    if (isStarted_)
    {
        LASS_THROW("Subprocess cannot be run more than once.");
    }
    pimpl_->run();
    isStarted_ = true;
}


int Subprocess::join()
{
    enforceStarted();
    if (!checkExitCode(true))
    {
        LASS_THROW("Failed to join subprocess.");
    }
    LASS_ASSERT(hasExitCode_);
    return exitCode_;
}


void Subprocess::detach()
{
    isDetached_ = true;
}


void Subprocess::sendSignal(int signal)
{
    pimpl_->sendSignal(signal);
}


void Subprocess::kill()
{
    if (!isStarted_ || !isRunning())
    {
        return;
    }
    enforceNotDetached();
    pimpl_->kill();
}


bool Subprocess::isRunning() const
{
    return isStarted_ && !checkExitCode();
}


bool Subprocess::isDetached() const
{
    return isDetached_;
}


int Subprocess::exitCode() const
{
    enforceStarted();
    if (!checkExitCode())
    {
        LASS_THROW("Subprocess is still running.");
    }
    LASS_ASSERT(hasExitCode_);
    return exitCode_;
}


void Subprocess::enforceStarted() const
{
    if (!isStarted_)
    {
        LASS_THROW("Subprocess has not started yet.");
    }
}


void Subprocess::enforceNotDetached() const
{
    if (isDetached_)
    {
        LASS_THROW("Subprocess is detached.");
    }
}


bool Subprocess::checkExitCode(bool join) const
{
    if (hasExitCode_)
    {
        return true;
    }
    enforceNotDetached();
    if (pimpl_->checkExitCode(exitCode_, join))
    {
        hasExitCode_ = true;
        return true;
    }
    return false;
}

}
}
}

// EOF
