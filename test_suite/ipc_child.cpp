#include <lass/io/message_pipe.h>
#include <lass/io/shared_memory.h>
#include <lass/util/thread.h>
#include "ipc_protocol.h"


#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32

/* a bit funky.
 * See, to avoid having a console window for the child process, we build it with /SUBSYSTEM:WINDOWS
 * (hence the WIN32 keyword in CMakeLists.txt).
 * But then we can't have a main, we must have a WinMain instead which only gives us the entire
 * commandline. No problem you say, it can be parsed with CommandLineToArgv.
 *
 * Not exactly. Only the Unicode version CommandLineToArgvW exists.
 *
 * So, here's what we do: get the unicode commandline, split it to argv with CommandLineToArgvW,
 * and encode it to UTF-8.
 */

#include <lass/util/wchar_support.h>
#define NOMINMAX
//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

INT CALLBACK WinMain(HINSTANCE, HINSTANCE, PSTR, INT)
{
    LPWSTR cmdLine = ::GetCommandLineW();
    int argc;
    LPWSTR* wargv = ::CommandLineToArgvW(cmdLine, &argc);

    // we're going to leak argv at the end of the main,
    // but as leaking at the end of a process isn't really a leak, we don't care.
    char** argv = new char*[argc + 1];
    for (int k = 0; k < argc; ++k)
    {
        const std::string arg = lass::util::wcharToUtf8(wargv[k]);
        argv[k] = _strdup(arg.c_str());
    }
    argv[argc] = 0; // for good measure.

    LocalFree(wargv); // at least clean up this.

    // note: there's an internal __getmainargs function that could do the same thing?

    // note: this is suspected to not quite work as advertised: if executable is started without
    // arguments, but executable path has spaces in it, the cmdLine will be that unquoted path.
    // Then, this will be interpreted as separated arguments, and argc != 1 :-(

    // std::cout and friends don't work with WinMain
    lass::io::ProxyMan* proxyMan = lass::io::proxyMan();
    proxyMan->cout()->remove(&std::cout);
    proxyMan->cerr()->remove(&std::cerr);
    proxyMan->clog()->remove(&std::clog);

#else

int main(int argc, char* argv[])
{

#endif

    using namespace lass;

    try
    {
#ifdef NDEBUG
        const size_t msecTimeout = 1000;
#else
        const size_t msecTimeout = 60000;
#endif

        if (argc != 2)
        {
            LASS_CERR << "Expected extactly one argument: the message pipe name.\n";
            LASS_CERR << "Usage: " << argv[0] << " <message-pipe-name>\n";
            return 2;
        }

        const char* pipeName = argv[1];

        LASS_CERR << "Child: Connecting to pipe '" << pipeName << "'...\n";
        io::TypedMessagePipe<ipc::Message> pipe;
        if (!pipe.connect(pipeName, msecTimeout))
        {
            LASS_CERR << "Child: Failed to connect to pipe '" << pipeName << "'\n";
            return 1;
        }

        LASS_CERR << "Child: Sending Hello, and waiting for first message...\n";
        ipc::Message res(ipc::mcHello);
        ipc::Message req;
        while (pipe.transact(res, req))
        {
            switch (req.code())
            {
            case ipc::mcDouble:
                {
                    const ipc::Tfloat32 doubled = 2 * req.value<ipc::Tfloat32>();
                    res = ipc::Message(ipc::mcDoubled, doubled);
                    break;
                }

            case ipc::mcUpper:
                {
                    io::SharedMemory mem;
                    if (!mem.open(req.payload()))
                    {
                        res = ipc::Message(ipc::mcError);
                    }
                    else
                    {
                        for (char* p = static_cast<char*>(mem.get()); *p != 0; ++p)
                        {
                            *p = static_cast<char>(std::toupper(*p));
                        }
                        res = ipc::Message(ipc::mcUppered);
                    }
                    break;
                }

            case ipc::mcExit:
                //LASS_CERR << "Child: Sending Goodbye...\n";
                pipe.send(ipc::Message(ipc::mcGoodbye));
                //LASS_CERR << "Child: Closing pipe...\n";
                // timing problem: if we close this fast after last message, it is never received.
                // a slight delay aready fixes the problem, but can we fix it without?
                pipe.close();
                return 0;

            default:
                // uh, message not understood!
                res = ipc::Message(ipc::mcNone);
            }
        }

        LASS_CERR << "Child: Lost connection to pipe (or timedout)\n";
        return 1;

    }
    catch (const std::exception& error)
    {
        LASS_CERR << "Child: " << error.what() << std::endl;
        return 1;
    }
}
