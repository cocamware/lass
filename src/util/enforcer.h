/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 */

/** @defgroup Enforcers Enforcers
 *  @brief enforcers are release-time counterpart of assertions to ease condition verifying
 *  @author [BdG]
 *
 *  Enforcers are a tool described by Alexandrescu [1] 
 *  [1] ALEXANDRESCU A. & MARGINEAN P. (2003), Enforcements. June 2003, C++ Experts Forum,
 *  http://www.cuj.com/documents/s=8250/cujcexp2106alexandr/alexandr.htm
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_DEBUG_ENFORCER_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_DEBUG_ENFORCER_H



#include "common_macros.h"
#include "impl/enforcer_impl.h"


#include <string>
#include <sstream>



/** Enforces the expression to be "true", by using operator!.
 *  @ingroup Enforcers
 *
 *  taken from:
 *  ALEXANDRESCU A. & MARGINEAN P. (2003), Enforcements. June 2003, C++ Experts Forum,
 *  http://www.cuj.com.
 *
 *  http://www.cuj.com/documents/s=8250/cujcexp2106alexandr
 */
#define LASS_ENFORCE(expression__)\
	(*lass::util::impl::makeEnforcer<lass::util::impl::DefaultPredicate,\
									 lass::util::impl::DefaultRaiser>(\
		(expression__), "Expression '" LASS_STRINGIFY(expression__) "' failed in '" LASS_HERE "'."))



/** Enforces a pointer to be different than the NULL pointer.
 *  @ingroup Enforcers
 */
#define LASS_ENFORCE_POINTER(pointer__) \
    (*lass::util::impl::makeEnforcer<lass::util::impl::DefaultPredicate,\
									lass::util::impl::DefaultRaiser>(\
		(pointer__), "Null pointer '" LASS_STRINGIFY(pointer__) "' detected in '" LASS_HERE "'."))



/** Enforces a stream to be in the good state.
 *  @ingroup Enforcers
 */
#define LASS_ENFORCE_STREAM(stream__)\
    (*lass::util::impl::makeEnforcer<lass::util::impl::StreamPredicate,\
									 lass::util::impl::DefaultRaiser>(\
		(stream__), "Failing stream '" LASS_STRINGIFY(stream__) "' detected in '" LASS_HERE "'."))



/** Enforces the handle to be different than -1.
 *  @ingroup Enforcers
 */
#define LASS_ENFORCE_HANDLE(handle__) \
    (*lass::util::impl::makeEnforcer<lass::util::impl::HandlePredicate,\
									 lass::util::impl::DefaultRaiser>(\
		(handle__), "Invalid handle '" LASS_STRINGIFY(handle__) "' in '" LASS_HERE "'."))



/** Enforces the HRESULT of a COM call to be 0 or more.
 *  @ingroup Enforcers
 */
#define LASS_ENFORCE_COM(comResult__) \
    (*lass::util::impl::makeEnforcer<lass::util::impl::ComPredicate,\
									 lass::util::impl::ComRaiser>(\
		(comResult__), "'" LASS_STRINGIFY(comResult__) "' in '" LASS_HERE "'."))


/** Enforces an index to be in the half open range [0, iSize).
 *  @ingroup Enforcers
 */
#define LASS_ENFORCE_INDEX(index__, size__)\
    (*lass::util::impl::makeEnforcer<lass::util::impl::IndexPredicate<(size__)>,\
                                     lass::util::impl::IndexRaiser<(size__)> >(\
		(index__), LASS_HERE))



/** Enforces a pointer to an object to be dynamic casted to related type.
 *  @ingroup Enforcers
 *  @param DestPointee__ the type of the pointee (not the pointer) to be casted to.  So if you 
 *                       want to cast to @c Foo*, @a DestPointee__ should be @c Foo.
 *  @param pointer__ a pointer to be casted (note that we don't cast references, use 
 *					 LASS_ENFORCE_DYNAMIC_REF_CAST for that)
 */ 
#define LASS_ENFORCE_DYNAMIC_CAST(DestPointee__, pointer__)\
    (*lass::util::impl::makeEnforcer<lass::util::impl::DefaultPredicate,\
									 lass::util::impl::DefaultRaiser>(\
		(dynamic_cast<DestPointee__*>(iPointer)),\
		"Unable to cast to '" LASS_STRINGIY(DestPointee__) "*': '" LASS_STRINGIFY(iPointer)\
		"' is a null pointer or is not of the correct type, '" LASS_HERE "'."))



/** Enforces a reference to an object to be dynamic casted to related type.
 *  @ingroup Enforcers
 *  @param DestReferred__ the type of referencee (not the reference) to be casted to.  So if you 
 *                        want to cast to @c Foo&, @a DestReferred__ should be @c Foo.
 *  @param reference__ a reference to be casted (note that we don't cast pointers, use 
 *					   LASS_ENFORCE_DYNAMIC_CAST for that)
 */ 
#define LASS_ENFORCE_DYNAMIC_REF_CAST(DestReferred__, reference__)\
    (*(*lass::util::impl::makeEnforcer<lass::util::impl::DefaultPredicate,\
									   lass::util::impl::DefaultRaiser>(\
		(dynamic_cast<DestReferred__*>(&reference__)),\
		"Unable to cast to '" LASS_STRINGIFY(DestReferred__) "&': '" LASS_STRINGIFY(reference__)\
		"' is not of the correct type, '" LASS_HERE "'.")))



/** Enforces a python shared pointer to be dynamic casted to a related python shared pointer.
 *  @ingroup Enforcers
 *  @param DestPointee__ the type of pointee (not the python shared pointer) to be casted to. So
 *						 if you want to cast to @c PyObjectPtr<Foo>::Type, @a DestPointee__ should
 *                       be @c Foo.
 *  @param pyObjectPointer__ a python shared pointer to be casted.
 */
#define LASS_ENFORCE_DYNAMIC_PY_CAST(DestPointee__, pyObjectPointer__)\
	(lass::python::PyObjectPtr<DestPointee__>::Type(lass::python::PyPlus_INCREF(\
		(*lass::util::impl::makeEnforcer<lass::util::impl::DefaultPredicate,\
										 lass::util::impl::DefaultRaiser>(\
			(dynamic_cast<DestPointee__*>((pyObjectPointer__).get())),\
			"Unable to cast to 'PyObjectPtr<" LASS_STRINGIFY(DestPointee__) ">::Type': '"\
			LASS_STRINGIFY(pyObjectPointer__) "' is a null pointer or is not of the correct type, "\
			LASS_HERE "'.")))))

#endif
