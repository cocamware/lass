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
 *  Copyright (C) 2004-2023 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_DYNAMIC_LIBRARY_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_DYNAMIC_LIBRARY_H

#include "util_common.h"

#if (LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32) || LASS_HAVE_DLOPEN
#   define LASS_HAVE_DYNAMIC_LIBRARY 1
#endif

namespace lass
{
namespace util
{

#ifdef LASS_HAVE_DYNAMIC_LIBRARY

/** Platform independant way to load a dynamic library or plugin
 * 
 *  @remark It is advised to provide absolute paths to the dynamic library as
 *      relative paths will not work reliably on Linux. It used to be that
 *      specifying -Wl,-rpath did set DT_RPATH which is a transitive dynamic
 *      array tag, meaning that DT_RPATH of the calling binary could in used
 *      by liblass-x-y.so to find the dynamic library. But since 
 *      -Wl,--enable-new-dtags has become the default, -Wl,-rpath will set
 *      DT_RUNPATH instead, which is NOT transitive. Lass can thus not rely on
 *      it to find the dynamic library. Instead, the calling code should do one
 *      of these things:
 *      1. rely on LD_LIBRARY_PATH environment variable
 *      2. use absolute paths to the dynamic libraries to be loaded. 
 */
class LASS_DLL DynamicLibrary: NonCopyable
{
public:

    DynamicLibrary(const std::string& path);
    ~DynamicLibrary();

    template <typename FunPtr>
    FunPtr resolveFunction(const std::string& functionName) const
    {
        // reinterpet_cast as some function signatures are not converted
        // example: typedef BOOL (* WINAPI TQueryThreadCycleTimeFunPtr) (::HANDLE, PULONG64);
        return reinterpret_cast<FunPtr>(resolveFunctionImpl(functionName));
    }

private:

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
    typedef int (*TFunctionPtr)(void);
#elif LASS_HAVE_DLOPEN
    // Not using a function pointer, because GCC 8 warns about cast between
    // incompatible function types. In general, we shouldn't be using void*
    // to cast function pointers, because they're not guaranteed to be the same
    // size. However, dlsym already returns void*, so we're fine.
    typedef void* TFunctionPtr;
#else
#   error missing implementation
#endif

    TFunctionPtr resolveFunctionImpl (const std::string& functionName) const;

    void* pimpl_;
};

#endif


class DynamicLibraryError: public ExceptionMixin<DynamicLibraryError>
{
public:
    DynamicLibraryError(std::string msg, std::string loc):
        ExceptionMixin<DynamicLibraryError>(std::move(msg), std::move(loc))
    {
    }
    ~DynamicLibraryError() noexcept {}
};

}
}

#endif

// EOF
