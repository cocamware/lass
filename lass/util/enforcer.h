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
 *	Copyright (C) 2004-2023 the Initial Developer.
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

/** @defgroup Enforcers Enforcers
 *  @brief enforcers are release-time counterpart of assertions to ease condition verifying
 *  @author [Bramz]
 *
 *  Enforcers are a tool described by Alexandrescu [1]
 *  [1] ALEXANDRESCU A. & MARGINEAN P. (2003), Enforcements. June 2003, C++ Experts Forum,
 *  http://www.cuj.com/documents/s=8250/cujcexp2106alexandr/alexandr.htm
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_ENFORCER_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_ENFORCER_H

#include "util_common.h"
#include "impl/lass_errno.h"

namespace lass
{
namespace util
{

/** Exception thrown by enforcers
 *  @ingroup Enforcers
 */
class EnforceFailure: public ExceptionMixin<EnforceFailure>
{
public:
	EnforceFailure(std::string msg, std::string loc): ExceptionMixin<EnforceFailure>(std::move(msg), std::move(loc)) {}
	~EnforceFailure() noexcept {}
};

}
}

/** Enforces the expression to be "true", by using operator!.
 *  @ingroup Enforcers
 *
 *  taken from:
 *  ALEXANDRESCU A. & MARGINEAN P. (2003), Enforcements. June 2003, C++ Experts Forum,
 *  http://www.cuj.com.
 *
 *  http://www.cuj.com/documents/s=8250/cujcexp2106alexandr
 */
#define LASS_ENFORCE(expression)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::TruePredicate,\
		::lass::util::impl::DefaultRaiser,\
		(expression), \
		int(0), \
		"Expression '" LASS_STRINGIFY(expression) "' failed in '" LASS_HERE "'.")



/** Enforces a pointer to be different than the NULL pointer.
 *  @ingroup Enforcers
 */
#define LASS_ENFORCE_POINTER(pointer) \
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::TruePredicate,\
		::lass::util::impl::DefaultRaiser,\
		(pointer), \
		int(0), \
		"Null pointer '" LASS_STRINGIFY(pointer) "' detected in '" LASS_HERE "'.")



/** Enforces a stream to be in the good state.
 *  @ingroup Enforcers
 */
#define LASS_ENFORCE_STREAM(stream)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::StreamPredicate,\
		::lass::util::impl::DefaultRaiser,\
		(stream),\
		int(0), \
		"Failing stream '" LASS_STRINGIFY(stream) "' detected in '" LASS_HERE "'.") 



/** Enforces the result of the expression to be zero.
 *  @ingroup Enforcers
 */
#define LASS_ENFORCE_ZERO(expression) \
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::EqualPredicate,\
		::lass::util::impl::ZeroRaiser>(\
		(expression), \
		int(0), \
		"'" LASS_STRINGIFY(expression) "' in '" LASS_HERE "'.")



/** Enforces the return code of a CLIB function call to be different than -1.
 *  @ingroup Enforcers
 *  Some CLIB functions return -1 on failure.  An error code indicating the failure
 *  can be found using errno.  LASS_ENFORCE_CLIB will enforce that the return
 *  value of the function call is not -1.  If it is -1, it will rais a runtime
 *  exception with the error code errno and its string message translated by strerror().
 */
#define LASS_ENFORCE_CLIB(functionCall)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::UnequalPredicate,\
		::lass::util::impl::ClibRaiser,\
		(functionCall), \
		int(-1), \
		"'" LASS_STRINGIFY(functionCall) "' in '" LASS_HERE "'")

#define LASS_ENFORCE_CLIB_EX(functionCall, errorValue)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::UnequalPredicate,\
		::lass::util::impl::ClibRaiser,\
		(functionCall), \
		(errorValue), \
		"'" LASS_STRINGIFY(functionCall) "' in '" LASS_HERE "'")

#define LASS_WARN_CLIB(functionCall)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::UnequalPredicate,\
		::lass::util::impl::ClibWarner,\
		(functionCall), \
		int(-1), \
		"'" LASS_STRINGIFY(functionCall) "' in '" LASS_HERE "'")

	
/** Enforces the return code of a CLIB function call to be zero.
 *  @ingroup Enforcers
 *  Some CLIB functions return zero on success and an error code on failure.
 *  This error code can be translated to a string message with strerror().
 *  LASS_ENFORCE_CLIB_RC will enforce that the return code of the function call
 *  is zero.  If it's not, it will raise a runtime exception with the translated
 *  error code.
 */
#define LASS_ENFORCE_CLIB_RC(errorCode)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::EqualPredicate,\
		::lass::util::impl::ClibRcRaiser,\
		(errorCode), \
		int(0), \
		"'" LASS_STRINGIFY(errorCode) "' in '" LASS_HERE "'")

	
/** Enforces the return code of a CLIB function call to be zero.
 *  @ingroup Enforcers
 *  Some CLIB functions return zero on success and an error code on failure.
 *  This error code can be translated to a string message with strerror().
 *  LASS_ENFORCE_CLIB_RC will enforce that the return code of the function call
 *  is zero.  If it's not, it will raise a runtime exception with the translated
 *  error code.
 */
#define LASS_WARN_CLIB_RC(errorCode)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::EqualPredicate,\
		::lass::util::impl::ClibRcWarner,\
		(errorCode), \
		int(0), \
		"'" LASS_STRINGIFY(errorCode) "' in '" LASS_HERE "'")


		
/** Enforces the HRESULT of a COM call to be 0 or more.
 *  @ingroup Enforcers
 */
#define LASS_ENFORCE_COM(comResult) \
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::GreaterEqualPredicate,\
		::lass::util::impl::ComRaiser,\
		(comResult), \
		int(0), \
		"'" LASS_STRINGIFY(comResult) "' in '" LASS_HERE "'.")



#ifdef LASS_HAS_GETLASTERROR

/** Enforces a Win API call to return non-zero and uses GetLastError() to retrieve error code.
 *  @ingroup Enforcers
 */
#define LASS_ENFORCE_WINAPI(functionCall)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::WinAPIPredicate,\
		::lass::util::impl::LastErrorRaiser,\
		(functionCall), \
		int(0), \
		"'" LASS_STRINGIFY(functionCall) "' in '" LASS_HERE "'")

#define LASS_WARN_WINAPI(functionCall)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::WinAPIPredicate,\
		::lass::util::impl::LastErrorWarner,\
		(functionCall), \
		int(0), \
		"'" LASS_STRINGIFY(functionCall) "' in '" LASS_HERE "'")


/** Enforces the handle to be different than INVALID_HANDLE_VALUE and uses GetLastError() to retrieve error code.
 *  @ingroup Enforcers
 */
#define LASS_ENFORCE_HANDLE(handle) \
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::UnequalPredicate,\
		::lass::util::impl::LastErrorRaiser,\
		(handle),\
		INVALID_HANDLE_VALUE, \
		"Invalid handle '" LASS_STRINGIFY(handle) "' in '" LASS_HERE "'.")

#endif



/** Enforces an index to be in the half open range [0, iSize).
 *  @ingroup Enforcers
 */
#define LASS_ENFORCE_INDEX(index, size)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::IndexPredicate,\
		::lass::util::impl::IndexRaiser,\
		(index), (size), LASS_HERE)



/** Enforces a pointer to an object to be dynamic casted to related type.
 *  @ingroup Enforcers
 *  @param t_DestPointer 
 *		the type of the pointer to be casted to.  
 *  @param v_pointer 
 *		a pointer to be casted (note that we don't cast references, use
 *      LASS_ENFORCE_DYNAMIC_REF_CAST for that)
 */
#define LASS_ENFORCE_DYNAMIC_CAST(t_DestPointer, v_pointer)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::TruePredicate,\
		::lass::util::impl::DefaultRaiser,\
		(dynamic_cast<t_DestPointer>(v_pointer)),\
		int(0), \
		"Unable to cast to '" LASS_STRINGIFY(t_DestPointer) "': '" LASS_STRINGIFY(v_pointer)\
		"' is a null pointer or is not of the correct type, '" LASS_HERE "'.")



/** Enforces a python shared pointer to be dynamic casted to a related python shared pointer.
 *  @ingroup Enforcers
 *  @param t_DestPyObjectPtr
 *		the type of python shared pointer to be casted to. 
 *		So if you want to cast to @c PyObjectPtr<Foo>::Type, @a t_DestPyObjectPtr should
 *		be @c PyObjectPtr<Foo>::Type.
 *  @param v_pyObjectPtr
 *		a python shared pointer to be casted.
 */
#define LASS_ENFORCE_DYNAMIC_PY_CAST(t_DestPyObjectPtr, v_pyObjectPtr)\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::TruePredicate,\
		::lass::util::impl::DefaultRaiser,\
		::lass::python::dynamicPyCast<t_DestPyObjectPtr>(v_pyObjectPtr),\
		int(0), \
		"Unable to cast to '" LASS_STRINGIFY(t_DestPyObjectPtr) "': '" LASS_STRINGIFY(v_pyObjectPtr)\
		"' is a null pointer or is not of the correct type, '" LASS_HERE "'.")



/** always-failing enforcer to block unreachable code paths
 */
#define LASS_ENFORCE_UNREACHABLE\
	*LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::TruePredicate,\
		::lass::util::impl::DefaultRaiser,\
		(false),\
		int(0), \
		"You have reached unreachable code in '" LASS_HERE "'.")

#include "impl/enforcer_impl.h"
#include <string>
#include <sstream>

#endif
