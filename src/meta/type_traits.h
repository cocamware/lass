/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
 *	<td><b>T</b></td>                   <td><b>isConst</b></td> <td><b>TNonConst</b></td>   <td><b>TConst</b></td>      <td><b>isPointer</b></td>   <td><b>TPointee</b></td>    <td><b>isReference</b></td> <td><b>TReferred</b></td>   <td><b>TReference</b></td>  <td><b>TStorage</b></td>
 *	</tr><tr>
 *  <td><b>int</b></td>                 <td>false</td>          <td>int</td>	            <td>const int</td>          <td>false</td>              <td>NullType</td>           <td>false</td>              <td>int</td>                <td>int&</td>               <td>int</td>
 *	</tr><tr>
 *	<td><b>int*</b></td>                <td>false</td>          <td>int*</td>	            <td>int* const</td>         <td>true</td>               <td>int</td>                <td>false</td>              <td>int*</td>               <td>int*&</td>              <td>int*</td>
 *	</tr><tr> 
 *  <td><b>int* const</b></td>          <td>true</td>           <td>int*</td>               <td>int* const</td>         <td>true</td>               <td>int</td>                 <td>false</td>              <td>int* const</td>         <td>int* const&</td>        <td>int*</td>
 *	</tr><tr>
 *  <td><b>int&</b></td>                <td>false</td>          <td>int&</td>               <td>const int&</td>         <td>false</td>              <td>NullType</td>           <td>true</td>               <td>int</td>                <td>int&</td>               <td>int</td>
 *	</tr><tr>
 *  <td><b>int*&</b></td>               <td>false</td>          <td>int*&</td>              <td>int* const&</td>        <td>true</td>               <td>int</td>                <td>true</td>               <td>int*</td>               <td>int*&</td>              <td>int*</td>
 *	</tr><tr>
 *  <td><b>int* const&</b></td>         <td>true</td>           <td>int*&</td>              <td>int* const&</td>        <td>true</td>               <td>int</td>                <td>true</td>               <td>int* const</td>         <td>int* const&</td>        <td>int*</td>
 *	</tr><tr>
 *  <td><b>const int</b></td>           <td>true</td>           <td>int</td>                <td>const int</td>          <td>false</td>              <td>NullType</td>           <td>false</td>              <td>const int</td>          <td>const int&</td>         <td>int</td>
 *	</tr><tr>
 *  <td><b>const int*</b></td>          <td>false</td>          <td>const int*</td>         <td>const int* const</td>   <td>true</td>               <td>const int</td>          <td>false</td>              <td>const int*</td>         <td>const int*&</td>        <td>const int*</td>
 *	</tr><tr>
 *  <td><b>const int* const</b></td>    <td>true</td>           <td>const int*</td>         <td>const int* const</td>   <td>true</td>               <td>const int</td>          <td>false</td>              <td>const int* const</td>   <td>const int* const&</td>  <td>const int*</td>
 *	</tr><tr>
 *  <td><b>const int&</b></td>          <td>true</td>           <td>int&</td>               <td>const int&</td>         <td>false</td>              <td>NullType</td>           <td>true</td>               <td>const int</td>          <td>const int&</td>         <td>int</td>
 *	</tr><tr>
 *  <td><b>const int*&</b></td>         <td>false</td>          <td>const int*&</td>        <td>const int* const&</td>  <td>true</td>               <td>const int</td>          <td>true</td>               <td>const int*</td>         <td>const int*&</td>        <td>const int*</td>
 *	</tr><tr>
 *  <td><b>const int* const&</b></td>   <td>true</td>           <td>const int*&</td>        <td>const int* const&</td>  <td>true</td>               <td>const int</td>          <td>true</td>               <td>const int* const</td>   <td>const int* const&</td>  <td>const int*</td>
 *	</tr></table>
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

#if !defined(LASS_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template <typename T>
struct TypeTraits
{
private:

    template <typename U> struct ConstTraits
    {
        enum { isConst = false };
        typedef typename U TNonConst;
        typedef typename const U TConst;
    };
    template <typename U> struct ConstTraits<const U>
    {
        enum { isConst = true };
        typedef typename U TNonConst;
        typedef typename const U TConst;
    };
    template <typename U> struct ConstTraits<U&>
    {
        enum { isConst = false };
        typedef typename U& TNonConst;
        typedef typename const U& TConst;
    };
    template <typename U> struct ConstTraits<const U&>
    {
        enum { isConst = true };
        typedef typename U& TNonConst;
        typedef typename const U& TConst;
    };

    template <typename U> struct PointerTraits
    {
        enum { isPointer = false };
        typedef typename NullType TPointee;
    };
    template <typename U> struct PointerTraits<U*>
    {
        enum { isPointer = true };
        typedef typename U TPointee;
    };
    template <typename U> struct PointerTraits<U&>
    {
        enum { isPointer = false };
        typedef typename NullType TPointee;
    };
    template <typename U> struct PointerTraits<U*&>
    {
        enum { isPointer = true };
        typedef typename U TPointee;
    };

	template <typename U> struct ReferenceTraits
    {
        enum { isReference = false };
        typedef typename U TReferred;
        typedef typename U& TReference;
    };
    template <typename U> struct ReferenceTraits<U&>
    {
        enum { isReference = true };
        typedef typename U TReferred;
        typedef typename U& TReference;
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

#define LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(type) /**/

#else

template <typename T>
struct TypeTraits
{
    LASS_META_ASSERT(false, lass_meta_TypeTraits_is_not_specialized_for_type);
};

#define LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(type)\
    namespace lass\
    {\
    namespace meta\
    {\
    template <>\
    struct TypeTraits<type >\
    {\
        typedef type Type;\
        enum { isConst = false };\
        typedef type TNonConst;\
        typedef const type TConst;\
        enum { isPointer = false };\
        typedef NullType TPointee;\
        enum { isReference = false };\
        typedef type TReferred;\
        typedef type& TReference;\
        typedef type TStorage;\
    };\
    template <>\
    struct TypeTraits<const type >\
    {\
        typedef const type Type;\
        enum { isConst = true };\
        typedef type TNonConst;\
        typedef const type TConst;\
        enum { isPointer = false };\
        typedef NullType TPointee;\
        enum { isReference = false };\
        typedef const type TReferred;\
        typedef const type& TReference;\
        typedef type TStorage;\
    };\
    template <>\
    struct TypeTraits<type*>\
    {\
        typedef type* Type;\
        enum { isConst = false };\
        typedef type* TNonConst;\
        typedef type* const TConst;\
        enum { isPointer = true };\
        typedef type TPointee;\
        enum { isReference = false };\
        typedef type* TReferred;\
        typedef type*& TReference;\
        typedef type* TStorage;\
    };\
    template <>\
    struct TypeTraits<const type*>\
    {\
        typedef const type* Type;\
        enum { isConst = false };\
        typedef const type* TNonConst;\
        typedef const type* const TConst;\
        enum { isPointer = true };\
        typedef const type TPointee;\
        enum { isReference = false };\
        typedef const type* TReferred;\
        typedef const type*& TReference;\
        typedef const type* TStorage;\
    };\
    template <>\
    struct TypeTraits<type* const>\
    {\
        typedef type* const Type;\
        enum { isConst = true };\
        typedef type* TNonConst;\
        typedef type* const TConst;\
        enum { isPointer = true };\
        typedef type TPointee;\
        enum { isReference = false };\
        typedef type* const TReferred;\
        typedef type* const& TReference;\
        typedef type* TStorage;\
    };\
    template <>\
    struct TypeTraits<const type* const>\
    {\
        typedef const type* const Type;\
        enum { isConst = true };\
        typedef const type* TNonConst;\
        typedef const type* const TConst;\
        enum { isPointer = true };\
        typedef const type TPointee;\
        enum { isReference = false };\
        typedef const type* const TReferred;\
        typedef const type* const& TReference;\
        typedef const type* TStorage;\
    };\
    template <>\
    struct TypeTraits<type&>\
    {\
        typedef type& Type;\
        enum { isConst = false };\
        typedef type& TNonConst;\
        typedef const type& TConst;\
        enum { isPointer = false };\
        typedef NullType TPointee;\
        enum { isReference = true };\
        typedef type TReferred;\
        typedef type& TReference;\
        typedef type TStorage;\
    };\
    template <>\
    struct TypeTraits<const type&>\
    {\
        typedef const type& Type;\
        enum { isConst = true };\
        typedef type& TNonConst;\
        typedef const type& TConst;\
        enum { isPointer = false };\
        typedef NullType TPointee;\
        enum { isReference = true };\
        typedef const type TReferred;\
        typedef const type& TReference;\
        typedef type TStorage;\
    };\
    template <>\
    struct TypeTraits<type*&>\
    {\
        typedef type*& Type;\
        enum { isConst = false };\
        typedef type*& TNonConst;\
        typedef type* const& TConst;\
        enum { isPointer = true };\
        typedef type TPointee;\
        enum { isReference = true };\
        typedef type* TReferred;\
        typedef type*& TReference;\
        typedef type* TStorage;\
    };\
    template <>\
    struct TypeTraits<const type*&>\
    {\
        typedef const type*& Type;\
        enum { isConst = false };\
        typedef const type*& TNonConst;\
        typedef const type* const& TConst;\
        enum { isPointer = true };\
        typedef const type TPointee;\
        enum { isReference = true };\
        typedef const type* TReferred;\
        typedef const type*& TReference;\
        typedef const type* TStorage;\
    };\
    template <>\
    struct TypeTraits<type* const&>\
    {\
        typedef type* const& Type;\
        enum { isConst = true };\
        typedef type*& TNonConst;\
        typedef type* const& TConst;\
        enum { isPointer = true };\
        typedef type TPointee;\
        enum { isReference = true };\
        typedef type* const TReferred;\
        typedef type* const& TReference;\
        typedef type* TStorage;\
    };\
    template <>\
    struct TypeTraits<const type* const&>\
    {\
        typedef const type* const& Type;\
        enum { isConst = true };\
        typedef const type*& TNonConst;\
        typedef const type* const& TConst;\
        enum { isPointer = true };\
        typedef const type TPointee;\
        enum { isReference = true };\
        typedef const type* const TReferred;\
        typedef const type* const& TReference;\
        typedef const type* TStorage;\
    };\
    }\
    }

#endif

}

}

LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(bool)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(char)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(wchar_t)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(signed char)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(signed short)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(signed int)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(signed long)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(signed __int64)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(unsigned char)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(unsigned short)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(unsigned int)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(unsigned long)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(unsigned __int64)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(float)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(double)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(long double)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(std::string)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(std::complex<float>)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(std::complex<double>)
LASS_META_BROKEN_TYPE_TRAITS_SPECIALISATION(std::complex<long double>)

#endif

// EOF
