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



/** @class lass::meta::TypeTraits
 *  @brief Interesting goodies on types
 *  @author Bram de Greve [BdG]
 *
 *  @section overview
 *
 *  TypeTraits gives you compile-time information on a type @a T.  It will tell you if
 *  @a T is
 *  constant type, a pointer, a reference ...  It will also able you to derive related types
 *  at compile time, such as non-const version of a constant type.
 *
 *  In contrary to the type traits in boost[1] or loki[2], our TypeTraits tries to be smarter
 *  concerning reference.  i.e. if @a T is a reference, TypeTraits will also report
 *  properties of the referred type.  And if you request a related type, it will modify
 *  the referred type instead.  All this should make sure you get exactly what you intend.
 *  (this is still to be proven in practice [BdG]).
 *
 *  Here's a list of the different fields (we suppose @a T is the template parameter):
 *
 *  @par Type
 *  a typedef to the template paramter.  i.e. this is the same as @a T.
 *
 *  @par isConst
 *  is @c true if @a T is a constant type or a reference to a constant type, is @c false otherwise.
 *  Notice that this will yield @c false for non-const pointers to const types (<tt>const int*</tt>)
 *  but @c true for const pointers to non-const types (<tt>int* const</tt>).
 *
 *  @par TNonConst
 *  a typedef to the non-const version of @a T if @a T is const, otherwise it's @a T itself.  You
 *  can use this to get rid of the @c const qualifier of a type, e.g. <tt>const int</tt> becomes
 *  <tt>int</tt> but <tt>const int*</tt> remains <tt>const int*</tt>.  For references, the @c const
 *  qualifier is stripped of the referred type, e.g. <tt>const int&</tt> becomes <tt>int&</tt>.
 *
 *  @par TConst
 *  This is the opposite of @b TNonConst.  It appends a @c const qualifier to the (reffered) type,
 *  <tt>const int*</tt> becomes <tt>const int* const</tt>, <tt>int&</tt> becomes <tt>const int&</tt>
 *
 *  @par isPointer
 *  is @c true if @a T is a pointer or a reference to a pointer, is @c false otherwise.
 *  Notice that <tt>int*&</tt> will yield true.
 *
 *  @par TPointee
 *  if @b isPointer is @c true then @b TPointee is a typedef to the (referred) type points to,
 *  otherwise it will be equal to lass::meta::NullType.  both <tt>int* const</tt> and <tt>int*&</tt>
 *  will yield <tt>int</tt>, but <tt>int</tt> and <tt>int&</tt> will yield lass::meta::NullType.
 *
 *  @par isReference
 *  is @c true if @a T is a reference, is @c false otherwise.  both <tt>const int&</tt> and
 *  <tt>int*&</tt> are references, <tt>const int</tt> and <tt>int*</tt> are not.
 *
 *  @par TReferred
 *  if @b isReference is @c true then strips the @e ampersand to get type @a T is refering to,
 *  otherwise it's @a T itself.  both <tt>const int&</tt> and <tt>const int</tt> become
 *  <tt>const int</tt>, <tt>int*&</tt> and <tt>int*</tt> become <tt>int*</tt>.
 *
 *  @par TReference
 *  if @b isReference is @c false then adds the @e ampersand to get a type refering to a @a T,
 *  otherwise it's @a T itself.  This solves the ill-formed @e reference-to-reference-to-type
 *  problem by keeping it a @e reference-to-type (what is exactly what you want).  i.e. <tt>int</tt>
 *  becomes <tt>int&</tt>, but <tt>int&</tt> remains <tt>int&</tt>.
 *
 *  @par TStorage
 *  yields in a type that can be used to store temporary objects.  To store temporary objects,
 *  we must get rid of the reference and the @c const qualifier of the referred type.  e.g. if a
 *  <tt>const std::string&</tt> is the type of a function parameter, you would like to @e store
 *  tempories as <tt>std::string</tt>.  This @b TStorage will give you this <tt>std::string</tt>
 *  type.
 *
 *  Here we list all possible results of TypeTraits in a table.  We show it for a base type
 *  int, but it is of course valid for any other type.
 *
 *  <table><tr>
 *  <td><b>T</b></td>                   <td><b>isConst</b></td> <td><b>TNonConst</b></td>   <td><b>TConst</b></td>      <td><b>isPointer</b></td>   <td><b>TPointee</b></td>    <td><b>isReference</b></td> <td><b>TReferred</b></td>   <td><b>TReference</b></td>  <td><b>TStorage</b></td>
 *  </tr><tr>
 *  <td><b>int</b></td>                 <td>false</td>          <td>int</td>                <td>const int</td>          <td>false</td>              <td>NullType</td>           <td>false</td>              <td>int</td>                <td>int&</td>               <td>int</td>
 *  </tr><tr>
 *  <td><b>int*</b></td>                <td>false</td>          <td>int*</td>               <td>int* const</td>         <td>true</td>               <td>int</td>                <td>false</td>              <td>int*</td>               <td>int*&</td>              <td>int*</td>
 *  </tr><tr>
 *  <td><b>int* const</b></td>          <td>true</td>           <td>int*</td>               <td>int* const</td>         <td>true</td>               <td>int</td>                 <td>false</td>              <td>int* const</td>         <td>int* const&</td>        <td>int*</td>
 *  </tr><tr>
 *  <td><b>int&</b></td>                <td>false</td>          <td>int&</td>               <td>const int&</td>         <td>false</td>              <td>NullType</td>           <td>true</td>               <td>int</td>                <td>int&</td>               <td>int</td>
 *  </tr><tr>
 *  <td><b>int*&</b></td>               <td>false</td>          <td>int*&</td>              <td>int* const&</td>        <td>true</td>               <td>int</td>                <td>true</td>               <td>int*</td>               <td>int*&</td>              <td>int*</td>
 *  </tr><tr>
 *  <td><b>int* const&</b></td>         <td>true</td>           <td>int*&</td>              <td>int* const&</td>        <td>true</td>               <td>int</td>                <td>true</td>               <td>int* const</td>         <td>int* const&</td>        <td>int*</td>
 *  </tr><tr>
 *  <td><b>const int</b></td>           <td>true</td>           <td>int</td>                <td>const int</td>          <td>false</td>              <td>NullType</td>           <td>false</td>              <td>const int</td>          <td>const int&</td>         <td>int</td>
 *  </tr><tr>
 *  <td><b>const int*</b></td>          <td>false</td>          <td>const int*</td>         <td>const int* const</td>   <td>true</td>               <td>const int</td>          <td>false</td>              <td>const int*</td>         <td>const int*&</td>        <td>const int*</td>
 *  </tr><tr>
 *  <td><b>const int* const</b></td>    <td>true</td>           <td>const int*</td>         <td>const int* const</td>   <td>true</td>               <td>const int</td>          <td>false</td>              <td>const int* const</td>   <td>const int* const&</td>  <td>const int*</td>
 *  </tr><tr>
 *  <td><b>const int&</b></td>          <td>true</td>           <td>int&</td>               <td>const int&</td>         <td>false</td>              <td>NullType</td>           <td>true</td>               <td>const int</td>          <td>const int&</td>         <td>int</td>
 *  </tr><tr>
 *  <td><b>const int*&</b></td>         <td>false</td>          <td>const int*&</td>        <td>const int* const&</td>  <td>true</td>               <td>const int</td>          <td>true</td>               <td>const int*</td>         <td>const int*&</td>        <td>const int*</td>
 *  </tr><tr>
 *  <td><b>const int* const&</b></td>   <td>true</td>           <td>const int*&</td>        <td>const int* const&</td>  <td>true</td>               <td>const int</td>          <td>true</td>               <td>const int* const</td>   <td>const int* const&</td>  <td>const int*</td>
 *  </tr></table>
 *
 *  @note You can see that references behave much the same as the refered type.  e.g.
 *  <tt>TypeTraits<U>::TPointee</tt> will be the same as
 *  <tt>TypeTraits<typename TypeTraits<U>::TReferred>::TPointee</tt>, no matter what the type of @c U is.
 *  The same is valid for @c isConst and @c isPointer.  @c TConst and @c TNonConst are a bit
 *  different, for these the @e ampersand will again be added if @c U is a reference.
 *
 *  @section broken broken compilers workaround
 *
 *  Unfortunately, the have a neat implementation of TypeTraits, we need partial template
 *  specialisation, a feature not supported by MSVC6x and MSVC70.  However, TypeTraits @e can be a
 *  crucial tool to implement algorithms that would be impossible to implement if TypeTraits fail.
 *  Thus we, @e do need a way to get this TypeTraits up and running in environments without this
 *  partial specialisation, and we need to get it to the @e right thing!
 *
 *  There's really only one way to solve this, and that's exhaustive full specialisation of all
 *  possible parameters @a T.  This means we should specialise it for <tt>int</tt>,
 *  <tt>const int</tt>, <tt>const int*</tt>, ... (for each base type @c int, you have to specialise
 *  for the twelve cases as in the table above).
 *
 *  Of course, it is impossible for a library to do this, because we can never know all types
 *  that will ever be needed.  Therefore we have a twofolded solution:
 *
 *  - we have provided full specialisations for all twelve cases of all @e fundamental types and
 *    some more.  This types are: bool, char, wchar_t, signed char, signed short, signed int,
 *    signed long, (signed __int64), unsigned char, unsigned short, unsigned int, unsigned long,
 *    (unsigned __int64), float, double, long double, std::string, std::complex<float>,
 *    std::complex<double> and std::complex<long double>
 *  - we have provided a macro LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION you can use to provide
 *    full specialisation of all twelve cases of any other type:
 *
 *  @code
 *  namespace foo
 *  {
 *      struct Bar {};
 *  }
 *  LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(foo::Bar)
 *  @endcode
 *
 *  @note you can safely use this macro regardless if the compiler supports partial specialisation.
 *        If your compiler supports it, the macro will be expanded to @e nothing.
 *
 *  @warning you have to call this macro @e outside any namespaces.
 *
 *  @warning this exhaustive full specialisation @e might lead to an overflow of the compiler's
 *  internal heap while building.  You're warned!
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_META_TYPE_TRAITS_H
#define LASS_GUARDIAN_OF_INCLUSION_META_TYPE_TRAITS_H

#include "meta_common.h"
#include "null_type.h"
#include "meta_assert.h"

namespace lass
{
namespace meta
{

template <typename T>
struct TypeTraits
{
private:

	template <typename U> struct ConstTraits
	{
		enum { isConst = false };
		typedef U TNonConst;
		typedef const U TConst;
	};
	template <typename U> struct ConstTraits<const U>
	{
		enum { isConst = true };
		typedef U TNonConst;
		typedef const U TConst;
	};
	template <typename U> struct ConstTraits<U&>
	{
		enum { isConst = false };
		typedef U& TNonConst;
		typedef const U& TConst;
	};
	template <typename U> struct ConstTraits<const U&>
	{
		enum { isConst = true };
		typedef U& TNonConst;
		typedef const U& TConst;
	};

	template <typename U> struct PointerTraits
	{
		enum { isPointer = false };
		typedef NullType TPointee;
	};
	template <typename U> struct PointerTraits<U*>
	{
		enum { isPointer = true };
		typedef U TPointee;
	};
	template <typename U> struct PointerTraits<U&>
	{
		enum { isPointer = false };
		typedef NullType TPointee;
	};
	template <typename U> struct PointerTraits<U*&>
	{
		enum { isPointer = true };
		typedef U TPointee;
	};

	template <typename U> struct ReferenceTraits
	{
		enum { isReference = false };
		typedef U TReferred;
		typedef U& TReference;
	};
	template <typename U> struct ReferenceTraits<U&>
	{
		enum { isReference = true };
		typedef U TReferred;
		typedef U& TReference;
	};

	typedef typename ConstTraits<T>::TNonConst TStripped;

public:

	/** type of @a T itself
	 */
	typedef T Type;

	/** is @c true if referred type of @a T is constant. */
	enum { isConst = ConstTraits<T>::isConst };
	/** strips @c const qualifier of refered type of @a T (but keeps reference if any). */
	typedef typename ConstTraits<T>::TNonConst TNonConst;
	/** add @c const qualifier to referred type of @a T (but keeps reference if any). */
	typedef typename ConstTraits<T>::TConst TConst;

	/** is @c true if referred type of @a T is a pointer */
	enum { isPointer = PointerTraits<TNonConst>::isPointer };
	/** is type pointed to by referred type of @a T, NullType if not applyable. */
	typedef typename PointerTraits<TNonConst>::TPointee TPointee;

	/** is @c true if @a T is a reference*/
	enum { isReference = ReferenceTraits<T>::isReference };
	/** is type @a T refers to if @a T is a reference, is @a T otherwise*/
	typedef typename ReferenceTraits<T>::TReferred TReferred;
	/** is a reference to @a T if @a T is @e not a reference, is @a T otherwise*/
	typedef typename ReferenceTraits<T>::TReference TReference;

	/** strips @c const qualifier of refered type of @a T but without keeping reference if any. */
	typedef typename ConstTraits<typename ReferenceTraits<T>::TReferred>::TNonConst TStorage;
};

}

}

#endif

// EOF
