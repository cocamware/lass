/** @file
 *  @author Bram de Greve (bram@cocamware.com)
 *  @author Tom De Muer (tom@cocamware.com)
 *
 *  *** BEGIN LICENSE INFORMATION ***
 *  
 *  The contents of this file are subject to the Common Public Attribution License 
 *  Version 1.0 (the "License"); you may not use this file except in compliance with 
 *  the License. You may obtain a copy of the License at 
 *  http://lass.sourceforge.net/cpal-license. The License is based on the 
 *  Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover 
 *  use of software over a computer network and provide for limited attribution for 
 *  the Original Developer. In addition, Exhibit A has been modified to be consistent 
 *  with Exhibit B.
 *  
 *  Software distributed under the License is distributed on an "AS IS" basis, WITHOUT 
 *  WARRANTY OF ANY KIND, either express or implied. See the License for the specific 
 *  language governing rights and limitations under the License.
 *  
 *  The Original Code is LASS - Library of Assembled Shared Sources.
 *  
 *  The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
 *  The Original Developer is the Initial Developer.
 *  
 *  All portions of the code written by the Initial Developer are:
 *  Copyright (C) 2004-2018 the Initial Developer.
 *  All Rights Reserved.
 *  
 *  Contributor(s):
 *
 *  Alternatively, the contents of this file may be used under the terms of the 
 *  GNU General Public License Version 2 or later (the GPL), in which case the 
 *  provisions of GPL are applicable instead of those above.  If you wish to allow use
 *  of your version of this file only under the terms of the GPL and not to allow 
 *  others to use your version of this file under the CPAL, indicate your decision by 
 *  deleting the provisions above and replace them with the notice and other 
 *  provisions required by the GPL License. If you do not delete the provisions above,
 *  a recipient may use your version of this file under either the CPAL or the GPL.
 *  
 *  *** END LICENSE INFORMATION ***
 */

#include "test_common.h"
#include "../lass/util/dynamic_library.h"

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#endif

/*
 *  Since -Wl,--enable-new-dtags has become the new default setting (gcc6?),
 *  -Wl,-rpath will now set DT_RUNPATH instead of DT_RPATH. Besides DT_RUNPATH
 *  having a lower priority than LD_LIBRARY_PATH, in contrast to DT_RPATH
 *  having a higher priority, it is also no longer *transitive*.
 *
 *  This means that the DT_RUNPATH of test_suite can no longer be used in
 *  liblass-x-y.so to dlopen a library. Perhaps we could specify
 *  -Wl,-rpath,'$ORIGIN/.' on liblass-x-y.so, but at the end, it's not liblass'
 *  job to know where dynamic libraries should be found. 
 *
 *  Two other solutions remain:
 *  1. Set LD_LIBRARY_PATH environment variable
 *  2. Use absolute paths to the dynamic library to be loaded
 *
 *  Both solutions are within reach of the calling code, and in this case
 *  we'll use the absolute path which CMake will provide use in the 
 *  TEST_DYNAMIC_CHILD definition.
 * 
 *  https://linux.die.net/man/3/dlopen
 *  https://gist.github.com/psychon/c7625ea030e3d78f09b1a709fd2d0bc7
 *  https://www.slideshare.net/linaroorg/sfo15307-advanced-toolchain-usage-part-5
 */

#if !defined(TEST_DYNAMIC_CHILD)
#   error "TEST_DYNAMIC_CHILD must be defined as path to dynamic module"
#endif

namespace lass
{
namespace test
{

void testUtilDynamicLibrary()
{

#if LASS_HAVE_DYNAMIC_LIBRARY

	LASS_TEST_CHECK_THROW(util::DynamicLibrary("thislibrarydoesnotexist"), util::DynamicLibraryError);

	try
	{
		util::DynamicLibrary module(TEST_DYNAMIC_CHILD);

		typedef float(*TMultiplyPtr)(float, float);
		LASS_TEST_CHECK_THROW(module.resolveFunction<TMultiplyPtr>("thisfunctiondoesnotexist"), util::DynamicLibraryError);

		TMultiplyPtr multiply = 0;
		LASS_TEST_CHECK_NO_THROW(multiply = module.resolveFunction<TMultiplyPtr>("multiply"));
		LASS_TEST_CHECK(multiply != 0);

		float result = 0.f;
		LASS_TEST_CHECK_NO_THROW(result = multiply(2.f, 3.f));
		LASS_TEST_CHECK_EQUAL(result, 6.f);
	}
	catch (const util::DynamicLibraryError&)
	{
		LASS_TEST_ERROR("Failed to load " TEST_DYNAMIC_CHILD);
	}

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
	try
	{
		util::DynamicLibrary kernel32("kernel32");

		typedef BOOL(*TQueryThreadCycleTimePtr) (HANDLE, PULONG64);
		TQueryThreadCycleTimePtr func = 0;
		LASS_TEST_CHECK_NO_THROW(func = kernel32.resolveFunction<TQueryThreadCycleTimePtr>("QueryThreadCycleTime"));
		LASS_TEST_CHECK(func != 0);

		HANDLE thread = GetCurrentThread();
		ULONG64 cycles = 0;
		BOOL ok = 0;
		LASS_TEST_CHECK_NO_THROW(ok = func(thread, &cycles));
		LASS_TEST_CHECK(ok != 0);
		LASS_TEST_CHECK(cycles > 0);
	}
	catch (const util::DynamicLibraryError&)
	{
		LASS_TEST_ERROR("Failed to load kernel32");
	}
#endif

#else

    LASS_TEST_ERROR( "DynamicLibrary not supported" );

#endif
}



TUnitTest test_util_dynamic_library()
{
    return TUnitTest(1, LASS_TEST_CASE(testUtilDynamicLibrary));
}

}

}

// EOF