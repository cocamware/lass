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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_ARGUMENT_TRAITS_H
#define LASS_GUARDIAN_OF_INCLUSION_PYTHON_ARGUMENT_TRAITS_H

#include "python_common.h"
#include "pyobject_plus.h"
#include "shadowee_traits.h"

namespace lass
{
namespace python
{
namespace impl
{

// --- ArgumentTraitsBuiltin -----------------------------------------------------------------------

/** @ingroup Python
 *  @internal
 *  For built-in types like int, float, bool, std::string ...
 *
 *  In case of built-in types, a Python type has to be converted to a C++ type.  Therefore a "copy"
 *  of the object is created.  And thus, ArgumentTraitsBuiltin does not support the cases in which
 *  you would be able to alter the original object: non-const pointers and non-const references.
 */
template <typename T>
struct ArgumentTraitsBuiltin
{
	typedef T TStorage;
	static const T& arg(const TStorage& storage) { return storage; }
};

/** @ingroup Python
 *  @internal
 *  Non-const pointers to builtin types are not supported as you would be acting on a copy anyway ...
 */
template <typename T>
struct ArgumentTraitsBuiltin<T*>
{
};

/** @ingroup Python
 *  @internal
 *  For built-in types, by const pointer is @e almost equivalent to by copy.
 */
template <typename T>
struct ArgumentTraitsBuiltin<const T*>
{
	typedef T TStorage;
	static const T* const arg(const TStorage& storage) { return &storage; }
};

/** @ingroup Python
 *  @internal
 *  Non-const references to builtin types are not supported as you would be acting on a copy anyway ...
 */
template <typename T>
struct ArgumentTraitsBuiltin<T&>
{
};

// --- ArgumentTraitsPyObject ----------------------------------------------------------------------

/** @ingroup Python
 *  @internal
 *  For types that derive from PyObject.  These are fully supported.
 */
template <typename T>
struct ArgumentTraitsPyObject
{
	typedef typename PyObjectPtr<T>::Type TStorage;
	static const T& arg(const TStorage& storage) { return *storage; }
};

/** @ingroup Python
 *  @internal
 */
template <typename T>
struct ArgumentTraitsPyObject<T*>
{
	typedef typename PyObjectPtr<T>::Type TStorage;
	static T* arg(const TStorage& storage) { return storage.get(); }
};

/** @ingroup Python
 *  @internal
 */
template <typename T>
struct ArgumentTraitsPyObject<const T*>
{
	typedef typename PyObjectPtr<T>::Type TStorage;
	static const T* arg(const TStorage& storage) { return storage.get(); }
};

/** @ingroup Python
 *  @internal
 */
template <typename T>
struct ArgumentTraitsPyObject<T&>
{
	typedef typename PyObjectPtr<T>::Type TStorage;
	static T& arg(const TStorage& storage) { return *storage; }
};

/** by PyObjectPtr.
 *  @ingroup Python
 */
template <typename T>
struct ArgumentTraitsPyObject< util::SharedPtr<T, PyObjectStorage, PyObjectCounter> >
{
	typedef typename PyObjectPtr<T>::Type TStorage;
	static const TStorage& arg(const TStorage& storage) { return storage; }
};


// --- ArgumentTraitsShadowee ------------------------------------------------------------------------

/** @ingroup Python
 *  @internal
 *  For types that have a shadow pyobject.  These are fully supported.
 */
template <typename T>
struct ArgumentTraitsShadowee
{
	typedef typename ShadoweeTraits<T>::TPointerTraits TPointerTraits;
	typedef typename TPointerTraits::TPtr TStorage;
	static const T& arg(const TStorage& storage) { return *storage; }
};

/** @ingroup Python
 *  @internal
 *  For types that have a shadow pyobject.  These are fully supported.
 */
template <typename T>
struct ArgumentTraitsShadowee<T&>
{
	typedef typename ShadoweeTraits<T>::TPointerTraits TPointerTraits;
	typedef typename TPointerTraits::TPtr TStorage;
	static T& arg(const TStorage& storage) { return *storage; }
};

/** @ingroup Python
 *  @internal
 *  For types that have a shadow pyobject (T may be const).
 */
template <typename T>
struct ArgumentTraitsShadowee<T*>
{
	typedef typename ShadoweeTraits<T>::TPointerTraits TPointerTraits;
	typedef typename TPointerTraits::TPtr TStorage;
	static T* arg(const TStorage& storage) { return storage; }
};

/** by SharedPtr (T may be const).
 *  @ingroup Python
 */
template <typename T, template <typename, typename> class S, typename C>
struct ArgumentTraitsShadowee< util::SharedPtr<T, S, C> >
{
	typedef util::SharedPtr<T, S, C> TStorage;
	static const util::SharedPtr<T, S, C>& arg(const TStorage& storage)  { return storage; }
};



// --- ArgumentTraitsSelector ----------------------------------------------------------------------

/** @ingroup Python
 *  @internal
 */
template <typename T, bool isPyObject, bool isShadowee> struct ArgumentTraitsSelector;

/** @ingroup Python
 *  @internal
 */
template <typename T> 
struct ArgumentTraitsSelector<T, false, false>: ArgumentTraitsBuiltin<T> 
{
};

/** @ingroup Python
 *  @internal
 */
template <typename T> 
struct ArgumentTraitsSelector<T, true, false>: ArgumentTraitsPyObject<T> 
{
};

/** @ingroup Python
 *  @internal
 */
template <typename T> 
struct ArgumentTraitsSelector<T, false, true>: ArgumentTraitsShadowee<T> 
{
};

}

// --- ArgumentTraits ------------------------------------------------------------------------------

/** by copy.
 *  @ingroup Python
 */
template <typename T> 
struct ArgumentTraits: impl::ArgumentTraitsSelector<T, IsPyObject<T>::value, ShadoweeTraits<T>::value>
{
};

/** by const copy.
 *  @ingroup Python
 *
 *  Is same as by copy.
 */
template <typename T> 
struct ArgumentTraits<const T>: ArgumentTraits<T>
{
};

/** by non-const pointer to non-const object.
 *  @ingroup Python
 */
template <typename T> 
struct ArgumentTraits<T*>: impl::ArgumentTraitsSelector<T*, IsPyObject<T>::value, ShadoweeTraits<T>::value>
{
};

/** by non-const pointer to const object.
 *  @ingroup Python
 */
template <typename T> 
struct ArgumentTraits<const T*>: impl::ArgumentTraitsSelector<const T*, IsPyObject<T>::value, ShadoweeTraits<T>::value>
{
};

/** by reference to non-const object.
 *  @ingroup Python
 */
template <typename T> 
struct ArgumentTraits<T&>: impl::ArgumentTraitsSelector<T&, IsPyObject<T>::value, ShadoweeTraits<T>::value>
{
};

/** by reference to const object.
 *  @ingroup Python
 *  
 *  Is same as by copy:
 *	@arg For built-in types, we must make a copy anyway to make the conversion, and we get a const ref to that copy.
 *  @arg For pyobjects and shadowees, by copy uses a pointer to the pyobject or shadow object as TStorage, 
 *		and you get a const ref to the pyobject or its shadowee.
 */
template <typename T> 
struct ArgumentTraits<const T&>: ArgumentTraits<T>
{
};

/** by SharedPtr
 *  @ingroup Python
 */
template <typename T, template <typename, typename> class S, typename C>
struct ArgumentTraits< util::SharedPtr<T, S, C> >: 
	impl::ArgumentTraitsSelector< util::SharedPtr<T, S, C> , IsPyObject<T>::value, ShadoweeTraits<T>::value>
{
};

/** by SharedPtr
 *  @ingroup Python
 */
template <typename T, template <typename, typename> class S, typename C>
struct ArgumentTraits< util::SharedPtr<const T, S, C> >: 
	impl::ArgumentTraitsSelector< util::SharedPtr<const T, S, C> , IsPyObject<T>::value, ShadoweeTraits<T>::value>
{
};

// --- odds and ends ---

template <>
struct ArgumentTraits<const char*>
{
	typedef std::string TStorage;
	static const char* arg(const TStorage& storage) { return storage.c_str(); }
};

}
}

#endif

