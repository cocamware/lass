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
*	Copyright (C) 2004-2022 the Initial Developer.
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



#include "test_common.h"

#include <lass/io/message_pipe.h>
#include <lass/io/shared_memory.h>
#include <lass/util/subprocess.h>
#include <lass/util/thread_fun.h>
#include "ipc_protocol.h"

#include <signal.h>

// path to executable
#ifndef TEST_IPC_CHILD
#   define TEST_IPC_CHILD "test_ipc_child"
#endif

namespace
{

using namespace lass;

const size_t msecTimeout = 60000;

typedef io::TypedMessagePipe<ipc::Message> TMessagePipe;

ipc::Message transact(TMessagePipe& pipe, const ipc::Message& req)
{
    ipc::Message res;
    if (!pipe.transact(req, res))
    {
        LASS_THROW("Transact timeout or error.");
    }
    return res;
}

#if LASS_PLATFORM_TYPE != LASS_PLATFORM_TYPE_WIN32

void sendInterrupts(util::experimental::Subprocess* child)
{
    while (child->isRunning())
    {
        util::Thread::sleep(200);
        child->sendSignal(SIGUSR1);
    }
}

#endif

void runScenario(unsigned long msecSleepParent, unsigned long msecSleepChild)
{
    TMessagePipe pipe;

    LASS_COUT << "Creating pipe...\n";
    if (!pipe.create())
    {
        LASS_TEST_ERROR("Failed to create pipe.");
        return;
    }

    util::experimental::Subprocess::TArgs args;
    args.push_back(TEST_IPC_CHILD);
    args.push_back(pipe.name());
    args.push_back(util::stringCast<std::string>(msecSleepChild));

    LASS_CERR << "Parent: Creating subprocess: " << args << std::endl;
    util::experimental::Subprocess child(args);

    LASS_COUT << "Parent: Starting subprocess...\n";
    child.run();

#if LASS_PLATFORM_TYPE != LASS_PLATFORM_TYPE_WIN32
    std::unique_ptr<util::Thread> interrupter(util::threadFun(
        sendInterrupts, &child, util::threadJoinable));
    interrupter->run();
#endif

    util::Thread::sleep(msecSleepParent);

    LASS_COUT << "Parent: Accepting subprocess on pipe...\n";
    if (!pipe.accept(msecTimeout))
    {
        LASS_TEST_ERROR("Accept timeout or error.");
        return;
    }

    util::Thread::sleep(msecSleepParent);

    {
        LASS_COUT << "Parent: Waiting for Hello message...\n";
        ipc::Message msgIn;
        if (!pipe.receive(msgIn))
        {
            LASS_TEST_ERROR("Receive timeout or error.");
            return;
        }
        LASS_TEST_CHECK_EQUAL(msgIn.code(), ipc::mcHello);
    }

    util::Thread::sleep(msecSleepParent);

    {
        LASS_COUT << "Parent: Asking to double a number...\n";
        const ipc::Tfloat32 number = 3.14f;
        ipc::Message res = transact(pipe, ipc::Message(ipc::mcDouble, number));
        LASS_TEST_CHECK_EQUAL(res.code(), ipc::mcDoubled);
        const ipc::Tfloat32 doubled = res.value<ipc::Tfloat32>();
        LASS_TEST_CHECK_EQUAL(doubled, 6.28f);
        LASS_COUT << "Parent: double of " << number << " is " << doubled << std::endl;
    }

    util::Thread::sleep(msecSleepParent);

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#   pragma warning(disable: 4996) // 'strncpy': This function or variable may be unsafe. Consider using strncpy_s instead.
#endif

    {
        LASS_COUT << "Parent: Asking to uppercase a string via shared memory...\n";
        const char* lowercase = "this is a string";
        const size_t size = ::strlen(lowercase) + 1;
        io::SharedMemory mem;
        const bool hasSuccessfullyCreatedSharedMemory = mem.create(size);
        LASS_TEST_CHECK(hasSuccessfullyCreatedSharedMemory);
        if (hasSuccessfullyCreatedSharedMemory)
        {
            char* buf = static_cast<char*>(mem.get());
            ::strncpy(buf, lowercase, size);
            buf[size - 1] = 0; // for good measure

            ipc::Message res = transact(pipe, ipc::Message(ipc::mcUpper, mem.name()));
            LASS_TEST_CHECK_EQUAL(res.code(), ipc::mcUppered);
            LASS_TEST_CHECK(strcmp(buf, "THIS IS A STRING") == 0);
        }
    }

    util::Thread::sleep(msecSleepParent);

    {
        LASS_COUT << "Parent: Transacting Exit message...\n";
        ipc::Message res = transact(pipe, ipc::Message(ipc::mcExit));
        LASS_TEST_CHECK_EQUAL(res.code(), ipc::mcGoodbye);
    }

    for (size_t k = 0; k < 2; ++k)
    {
        util::Thread::sleep(msecSleepParent);

        ipc::Message msgIn;
        if (!pipe.receive(msgIn))
        {
            LASS_CERR << "well, really can't receive anymore\n";
        }
        else
        {
            LASS_TEST_ERROR("OOPS.");
        }
    }

    util::Thread::sleep(msecSleepParent);

    LASS_COUT << "Parent: Closing pipe...\n";
    pipe.close();

    util::Thread::sleep(msecSleepParent);

    LASS_COUT << "Parent: Joining child...\n";
    LASS_TEST_CHECK_EQUAL(child.join(), 0);

#if LASS_PLATFORM_TYPE != LASS_PLATFORM_TYPE_WIN32
    interrupter->join();
#endif
}

}

namespace lass
{
namespace test
{

void testIoIPCNoSleep()
{
    runScenario(0, 0);
}

void testIoIPCParentSleep()
{
    runScenario(100, 0);
}

void testIoIPCChildSleep()
{
    runScenario(0, 100);
}

TUnitTest test_io_ipc()
{
    return TUnitTest{
        LASS_TEST_CASE(testIoIPCNoSleep),
        LASS_TEST_CASE(testIoIPCParentSleep),
        LASS_TEST_CASE(testIoIPCChildSleep),
    };
}

}

}

// EOF
