/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#include "../meta/type_traits.h"
#include "../meta/is_same.h"

#define LASS_TEST_META_TYPE_TRAITS_EVAL(type)\
	LASS_COUT << LASS_STRINGIFY(type) << std::endl;\
	LASS_COUT << "Type: " << typeid(TypeTraits<type>::Type).name() << std::endl;\
	LASS_COUT << (TypeTraits<type>::isConst ? "isConst" : "---") << ", ";\
	LASS_COUT << (TypeTraits<type>::isPointer ? "isPointer" : "---") << ", ";\
	LASS_COUT << (TypeTraits<type>::isReference ? "isReference" : "---") << std::endl;\
	LASS_COUT << "TNonConst: " << typeid(typename TypeTraits<type>::TNonConst).name() << std::endl;\
	LASS_COUT << "TConst: " << typeid(typename TypeTraits<type>::TConst).name() << std::endl;\
	LASS_COUT << "TPointee: " << typeid(typename TypeTraits<type>::TPointee).name() << std::endl;\
	LASS_COUT << "TReferred: " << typeid(typename TypeTraits<type>::TReferred).name() << std::endl;\
	LASS_COUT << "TReference: " << typeid(typename TypeTraits<type>::TReference).name() << std::endl;\
	LASS_COUT << "TStorage: " << typeid(typename TypeTraits<type>::TStorage).name() << std::endl;\
	LASS_COUT << std::endl

namespace lass
{
namespace test
{

// --- public --------------------------------------------------------------------------------------

template <typename T>
void testMetaTypeTraits()
{
	using namespace meta;

	typedef TypeTraits<T> TTraits;
	LASS_TEST_CHECK(!TTraits::isConst);
	LASS_TEST_CHECK(!TTraits::isPointer);
	LASS_TEST_CHECK(!TTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TTraits::Type, T>::value));
	LASS_TEST_CHECK((IsSame<typename TTraits::TNonConst, T>::value));
	LASS_TEST_CHECK((IsSame<typename TTraits::TConst, T const>::value));
	LASS_TEST_CHECK((IsSame<typename TTraits::TPointee, NullType>::value));
	LASS_TEST_CHECK((IsSame<typename TTraits::TReferred, T>::value));
	LASS_TEST_CHECK((IsSame<typename TTraits::TReference, T&>::value));
	LASS_TEST_CHECK((IsSame<typename TTraits::TStorage, T>::value));

	typedef TypeTraits<const T> TConstTraits;
	LASS_TEST_CHECK(TConstTraits::isConst);
	LASS_TEST_CHECK(!TConstTraits::isPointer);
	LASS_TEST_CHECK(!TConstTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TConstTraits::Type, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstTraits::TNonConst, T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstTraits::TConst, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstTraits::TPointee, NullType>::value));
	LASS_TEST_CHECK((IsSame<typename TConstTraits::TReferred, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstTraits::TReference, const T&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstTraits::TStorage, T>::value));

	typedef TypeTraits<T*> TPtrTraits;
	LASS_TEST_CHECK(!TPtrTraits::isConst);
	LASS_TEST_CHECK(TPtrTraits::isPointer);
	LASS_TEST_CHECK(!TPtrTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::Type, T*>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::TNonConst, T*>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::TConst, T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::TPointee, T>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::TReferred, T*>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::TReference, T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrTraits::TStorage, T*>::value));

	typedef TypeTraits<const T*> TConstPtrTraits;
	LASS_TEST_CHECK(!TConstPtrTraits::isConst);
	LASS_TEST_CHECK(TConstPtrTraits::isPointer);
	LASS_TEST_CHECK(!TConstPtrTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::Type, const T*>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::TNonConst, const T*>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::TConst, const T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::TPointee, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::TReferred, const T*>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::TReference, const T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrTraits::TStorage, const T*>::value));

	typedef TypeTraits<T* const> TPtrConstTraits;
	LASS_TEST_CHECK(TPtrConstTraits::isConst);
	LASS_TEST_CHECK(TPtrConstTraits::isPointer);
	LASS_TEST_CHECK(!TPtrConstTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::Type, T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::TNonConst, T*>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::TConst, T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::TPointee, T>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::TReferred, T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::TReference, T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstTraits::TStorage, T*>::value));

	typedef TypeTraits<const T* const> TConstPtrConstTraits;
	LASS_TEST_CHECK(TConstPtrConstTraits::isConst);
	LASS_TEST_CHECK(TConstPtrConstTraits::isPointer);
	LASS_TEST_CHECK(!TConstPtrConstTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::Type, const T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::TNonConst, const T*>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::TConst, const T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::TPointee, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::TReferred, const T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::TReference, const T* const &>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstTraits::TStorage, const T*>::value));

	typedef TypeTraits<T&> TRefTraits;
	LASS_TEST_CHECK(!TRefTraits::isConst);
	LASS_TEST_CHECK(!TRefTraits::isPointer);
	LASS_TEST_CHECK(TRefTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TRefTraits::Type, T&>::value));
	LASS_TEST_CHECK((IsSame<typename TRefTraits::TNonConst, T&>::value));
	LASS_TEST_CHECK((IsSame<typename TRefTraits::TConst, const T&>::value));
	LASS_TEST_CHECK((IsSame<typename TRefTraits::TPointee, NullType>::value));
	LASS_TEST_CHECK((IsSame<typename TRefTraits::TReferred, T>::value));
	LASS_TEST_CHECK((IsSame<typename TRefTraits::TReference, T&>::value));
	LASS_TEST_CHECK((IsSame<typename TRefTraits::TStorage, T>::value));

	typedef TypeTraits<const T&> TConstRefTraits;
	LASS_TEST_CHECK(TConstRefTraits::isConst);
	LASS_TEST_CHECK(!TConstRefTraits::isPointer);
	LASS_TEST_CHECK(TConstRefTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::Type, const T&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::TNonConst, T&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::TConst, const T&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::TPointee, NullType>::value));
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::TReferred, const T >::value));
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::TReference, const T&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstRefTraits::TStorage, T>::value));

	typedef TypeTraits<T*&> TPtrRefTraits;
	LASS_TEST_CHECK(!TPtrRefTraits::isConst);
	LASS_TEST_CHECK(TPtrRefTraits::isPointer); //?????????
	LASS_TEST_CHECK(TPtrRefTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::Type, T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::TNonConst, T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::TConst, T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::TPointee, T>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::TReferred, T*>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::TReference, T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrRefTraits::TStorage, T*>::value));

	typedef TypeTraits<const T*&> TConstPtrRefTraits;
	LASS_TEST_CHECK(!TConstPtrRefTraits::isConst);
	LASS_TEST_CHECK(TConstPtrRefTraits::isPointer);
	LASS_TEST_CHECK(TConstPtrRefTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::Type, const T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::TNonConst, const T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::TConst, const T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::TPointee, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::TReferred, const T*>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::TReference, const T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrRefTraits::TStorage, const T*>::value));

	typedef TypeTraits<T* const&> TPtrConstRefTraits;
	LASS_TEST_CHECK(TPtrConstRefTraits::isConst);
	LASS_TEST_CHECK(TPtrConstRefTraits::isPointer);
	LASS_TEST_CHECK(TPtrConstRefTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::Type, T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::TNonConst, T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::TConst, T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::TPointee, T>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::TReferred, T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::TReference, T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TPtrConstRefTraits::TStorage, T*>::value));

	typedef TypeTraits<const T* const&> TConstPtrConstRefTraits;
	LASS_TEST_CHECK(TConstPtrConstRefTraits::isConst);
	LASS_TEST_CHECK(TConstPtrConstRefTraits::isPointer);
	LASS_TEST_CHECK(TConstPtrConstRefTraits::isReference);
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::Type, const T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::TNonConst, const T*&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::TConst, const T* const&>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::TPointee, const T>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::TReferred, const T* const>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::TReference, const T* const &>::value));
	LASS_TEST_CHECK((IsSame<typename TConstPtrConstRefTraits::TStorage, const T*>::value));
}


TUnitTests test_meta_type_traits()
{
	TUnitTests suite;
	suite.push_back(LASS_UNIT_TEST(testMetaTypeTraits<int>));
	suite.push_back(LASS_UNIT_TEST(testMetaTypeTraits<float>));
	return suite;
}

}

}
