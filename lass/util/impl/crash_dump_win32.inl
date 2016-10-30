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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

/* @file
 * @internal
 * http://www.debuginfo.com/examples/debugfilterexamples.html
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_CRASH_DUMP_WIN32_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_CRASH_DUMP_WIN32_INL

#include "../util_common.h"
#include "lass_errno.h"
#include "../thread_fun.h"
#include "../scoped_ptr.h"

#include <windows.h>
#include <objbase.h>
#include <tchar.h>
#include <stdlib.h>

#pragma warning(push)
#pragma warning(disable: 4996) // This function or variable may be unsafe.
#pragma warning(disable: 4091) // 'typedef ': ignored on left of '' when no variable is declared (in C:\Program Files (x86)\Windows Kits\8.1\Include\um\dbghelp.h)

#include <dbghelp.h>

#define LASS_EXPERIMENTAL_CATCH_AND_REPORT(expression)\
	try\
	{\
		expression;\
	}\
	catch (std::exception& error)\
	{\
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: '" LASS_STRINGIFY(expression)\
			"' failed in '" LASS_HERE "' with std::exception: " << error.what() << std::endl << std::flush;\
	}\
	catch (...)\
	{\
		std::cerr << "[LASS RUN MSG] UNDEFINED BEHAVIOUR WARNING: '" LASS_STRINGIFY(expression)\
			"' failed in '" LASS_HERE "' with unknown exception." << std::endl << std::flush;\
	}\
	/**/

namespace lass
{
namespace util
{
namespace impl
{

/**
 *  
 */
class CrashDumpImpl
{
public:

	typedef void(*TCallback)(const char* /* filename */, void* /* closure */);

	CrashDumpImpl(const std::string& name, TCallback callback, void* callbackClosure):
		oldFilter_(0),
		callback_(callback),
		callbackClosure_(callbackClosure),
		mutex_(0),
		isExiting_(false),
		isHandlingException_(false)
	{
		if (instance_)
		{
			LASS_THROW("You can install only one crash dumper at the same time");
		}

		::GUID guid;
		LASS_ENFORCE_COM(::CoCreateGuid(&guid));
		const size_t guidSize = 39;
		char guidString[guidSize];
		_snprintf(guidString, guidSize, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			int(guid.Data1), int(guid.Data2), int(guid.Data3), int(guid.Data4[0]), int(guid.Data4[1]), 
			int(guid.Data4[2]), int(guid.Data4[3]), int(guid.Data4[4]), int(guid.Data4[5]), 
			int(guid.Data4[6]), int(guid.Data4[7]));
		guidString[guidSize - 1] = '\0';
		const size_t guidLength = strlen(guidString);

		const size_t n = name.length();
		const char* c_str = name.c_str();
		std::copy(c_str, c_str + n, std::back_inserter(dumpFilename_));
		dumpFilename_.push_back('-');
		std::copy(guidString + 1, guidString + guidLength - 1, std::back_inserter(dumpFilename_));
		const size_t extensionSize = 5;
		char extension[extensionSize] = {'.', 'd', 'm', 'p', '\0'};
		std::copy(extension, extension + extensionSize, std::back_inserter(dumpFilename_));
		
		if (!dumpFilename_.empty())
		{
#ifdef _UNICODE
			const char* filename = &dumpFilename_[0];
			const int bufferLength = LASS_ENFORCE_WINAPI(
				::MultiByteToWideChar(CP_UTF8, 0, filename, -1, 0, 0));
			dumpFilenameT_.resize(bufferLength);
			LASS_ENFORCE_WINAPI(::MultiByteToWideChar(CP_UTF8, 0, filename, -1, &dumpFilenameT_[0], bufferLength));
#else
			dumpFilenameT_ = dumpFilename_;
#endif
		}

		instance_ = this;
	}

	void init()
	{
		if (instance_ != this || mutex_)
		{
			LASS_THROW("you should have called this!");
		}
		mutex_ = new TMutex;

		dbghelp_ = loadLibrary(_T("dbghelp.dll"));
		miniDumpWriteDump_ = reinterpret_cast<TMiniDumpWriteDump>(LASS_ENFORCE_WINAPI(
			::GetProcAddress(dbghelp_, "MiniDumpWriteDump")));

		isExiting_ = false;
		handlerThread_.reset(util::threadMemFun(this, &CrashDumpImpl::handlerThread, util::threadJoinable));
		handlerThread_->run();

		oldFilter_ = ::SetUnhandledExceptionFilter(unhandledExceptionFilter);
		patchSetUnhandledExceptionFilter(true);

		instance_ = this;
	}

	~CrashDumpImpl()
	{
		instance_ = 0;

		LASS_EXPERIMENTAL_CATCH_AND_REPORT(patchSetUnhandledExceptionFilter(false));
		::SetUnhandledExceptionFilter(oldFilter_);

		isExiting_ = true;
		handleCondition_.signal();
		handlerThread_->join();

		LASS_WARN_WINAPI(::FreeLibrary(dbghelp_));

		delete mutex_;
		mutex_ = 0;
	}

public:

	enum 
	{ 
		bufferSize_ = _MAX_PATH,
#if LASS_ADDRESS_SIZE == 64
		numPatchBytes_ = 3,
#else
		numPatchBytes_ = 5,
#endif
	};

	typedef BOOL (WINAPI *TMiniDumpWriteDump)(HANDLE hProcess, DWORD ProcessId, HANDLE hFile, MINIDUMP_TYPE DumpType,
		CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, 
		CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

	typedef util::CriticalSection TMutex;

	const HMODULE loadLibrary(const TCHAR* libraryName) const
	{
		HMODULE library = 0;

		// try to load from exe directory
		library = loadSideLibrary(libraryName, 0);
		
#if LASS_BUILD_DLL
		if (!library && dll::getLassInstance())
		{
			// try to load from LASS DLL directory
			library = loadSideLibrary(libraryName, dll::getLassInstance());
		}
#endif

		if (!library) 
		{
			// try to load from default location
			library = ::LoadLibrary(libraryName);
		}

		if (!library)
		{
			LASS_THROW("Failed to install CrashDump: failed to load " << libraryName);
		}

		return library;
	}

	const HMODULE loadSideLibrary(const TCHAR* libraryName, HMODULE me) const
	{
		TCHAR modulePath[bufferSize_];
		const DWORD pathLength = ::GetModuleFileName(me, modulePath, bufferSize_);
		if (pathLength != 0 && pathLength < bufferSize_)
		{
			TCHAR* const lastBackSlash = ::_tcsrchr(modulePath, _T('\\'));
			if (lastBackSlash)
			{
				TCHAR* const filename = lastBackSlash + 1;
				::_tcsncpy(filename, libraryName, (bufferSize_ - 1) - (filename - modulePath));
				modulePath[bufferSize_ - 1] = _T('\0');
				if (::_tcscmp(filename, libraryName) == 0)
				{
					return ::LoadLibrary(modulePath);
				}
			}
		}
		return 0;
	}

	/** @internal
	 *  Patch or restore ::SetUnhandledExceptionFilter to keep our filter in charge.
	 *  
	 *  We need to sabotage ::SetUnhandledExceptionFilter in order to prevent anyone else from
	 *  calling it to override our filter.  This is a very real issue since even the runtime
	 *  library is tempted to do so.
	 *
	 *  @param patch [in] true to patch it, false to restore it.
	 */
	void patchSetUnhandledExceptionFilter(bool patch)
	{
		HMODULE kernel32 = LASS_ENFORCE_WINAPI(::GetModuleHandle(_T("kernel32.dll")));
		void* fun = LASS_ENFORCE_WINAPI(::GetProcAddress(kernel32, "SetUnhandledExceptionFilter"));
		if (patch)
		{
			LASS_ENFORCE(!::IsBadReadPtr(fun, numPatchBytes_));
			::memcpy(oldBytes_, fun, numPatchBytes_);
			writeMemory(fun, patchBytes_, numPatchBytes_);
		}
		else
		{
			writeMemory(fun, oldBytes_, numPatchBytes_);
		}
	}

	static void writeMemory(void* dest, const void* source, size_t size)
	{
		DWORD oldProtect, dummy;
		LASS_ENFORCE_WINAPI(::VirtualProtect(dest, size, PAGE_EXECUTE_READWRITE, &oldProtect));
		::memcpy(dest, source, size);
		::VirtualProtect(dest, size, oldProtect, &dummy);
	}

	void handlerThread()
	{
		while (true)
		{
			while (!(isHandlingException_ || isExiting_))
			{
				handleCondition_.wait(2000);
			}
			if (isExiting_)
			{
				return;
			}
			
			doHandleException();

			const char* filename = &dumpFilename_[0];
			if (callback_) 
			{
				callback_(filename, callbackClosure_);
			}
			else
			{
				std::cerr << "[LASS RUN MSG] dumped minidump at \"" << filename << "\"." 
					<< std::endl << std::flush;
			}
			isHandlingException_ = false;
			resultCondition_.signal();
		}
	}

	void doHandleException()
	{
		handlerResult_ = EXCEPTION_CONTINUE_SEARCH;
		const TCHAR* filename = &dumpFilenameT_[0]; 
		HANDLE file = ::CreateFile(filename, GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
		if (file == INVALID_HANDLE_VALUE)
		{
			return;
		}

		HANDLE process = ::GetCurrentProcess();
		DWORD processId = ::GetCurrentProcessId();
		MINIDUMP_EXCEPTION_INFORMATION exceptionParam;
		exceptionParam.ThreadId = threadId_;
		exceptionParam.ExceptionPointers = exceptionInfo_;
		exceptionParam.ClientPointers = 0;

		if (miniDumpWriteDump_(process, processId, file, ::MiniDumpNormal, &exceptionParam, 0, 0))
		{
			handlerResult_ = EXCEPTION_EXECUTE_HANDLER;
		}

		::CloseHandle(file);
	}

	static LONG WINAPI unhandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo)
	{
		const DWORD code = exceptionInfo->ExceptionRecord->ExceptionCode;
		if (code == EXCEPTION_BREAKPOINT || code == EXCEPTION_SINGLE_STEP)
		{
			return EXCEPTION_CONTINUE_SEARCH;
		};
		LASS_LOCK(*instance_->mutex_)
		{
			instance_->exceptionInfo_ = exceptionInfo;
			instance_->threadId_ = ::GetCurrentThreadId();
			instance_->isHandlingException_ = true;
			instance_->handleCondition_.signal();
			while (instance_->isHandlingException_)
			{
				instance_->resultCondition_.wait(500);
			}
			return instance_->handlerResult_;
		}
		return EXCEPTION_CONTINUE_SEARCH; // unreachable though
	}

	std::vector<char> dumpFilename_;
	std::vector<TCHAR> dumpFilenameT_;
	TCallback callback_;
	void* callbackClosure_;
	HMODULE dbghelp_;
	TMiniDumpWriteDump miniDumpWriteDump_;
	util::ScopedPtr<Thread> handlerThread_;
	util::Condition handleCondition_;
	util::Condition resultCondition_;
	TMutex* mutex_;
	LPTOP_LEVEL_EXCEPTION_FILTER oldFilter_;
	unsigned char oldBytes_[numPatchBytes_];

	_EXCEPTION_POINTERS* exceptionInfo_;
	DWORD threadId_;
	LONG handlerResult_;

	volatile bool isExiting_;
	volatile bool isHandlingException_;

	static CrashDumpImpl* instance_;
	static unsigned char patchBytes_[numPatchBytes_];
};

CrashDumpImpl* CrashDumpImpl::instance_ = 0;
#if LASS_ADDRESS_SIZE == 64
unsigned char CrashDumpImpl::patchBytes_[numPatchBytes_] = { 0x33, 0xc0, 0xc3 };
#else
unsigned char CrashDumpImpl::patchBytes_[numPatchBytes_] = { 0x33, 0xc0, 0xc2, 0x04, 0x00 };
#endif

}
}
}

#pragma warning(pop)

#endif

// EOF
