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

#include "atomic_poor_man.h"

namespace lass
{
namespace util
{
namespace impl
{

#if LASS_KUSER_HELPER_VERSION >= 2
#define LASS_HAVE_KUSER_CMPXCHG 1
typedef int (kuser_cmpxchg_t)(int oldval, int newval, volatile int *ptr);
#define lass_util_impl_kuser_cmpxchg (*(kuser_cmpxchg_t *)0xffff0fc0)
#endif

#if LASS_KUSER_HELPER_VERSION >= 5
#define LASS_HAVE_KUSER_CMPXCHG64 1
typedef int (kuser_cmpxchg64_t)(const int64_t *oldval, const int64_t *newval, volatile int64_t *ptr);
#define lass_util_impl_kuser_cmpxchg64 (*(kuser_cmpxchg64_t *)0xffff0f60)
#endif

#if LASS_HAVE_BIG_ENDIAN
#   error "code below assumes little endian. can easily be adapted for big endian too, but that's not done yet ;-)"
#endif


// --- compareAndSwap ---------------------------------------------------------

#if LASS_HAVE_LDREXB_STREXB
template <>
template <typename T>
inline bool AtomicOperations<1>::compareAndSwap(volatile T& dest, T expected, T desired)
{   
    uint8_t tmp;
    bool failure = false;
    __asm__ __volatile__(
        "ldrexb %[tmp], [%[dest]]\n\t"
        "mov %[failure], #1\n\t"
        "teq %[tmp], %[expected]\n\t"
        "it eq\n\t"
        "strexbeq %[failure], %[desired], [%[dest]]\n\t"
        : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
        : [dest]"r"(&dest), [expected]"r"(expected), [desired]"r"(desired)
        : "cc", "memory");
    return !failure;
}
#endif

#if LASS_HAVE_LDREXH_STREXH
template <>
template <typename T>
inline bool AtomicOperations<2>::compareAndSwap(volatile T& dest, T expected, T desired)
{   
    LASS_ASSERT((reinterpret_cast<num::TuintPtr>(&dest) & 0x1) == 0); // requires half word alignment

    uint16_t tmp;
    bool failure = false;
    __asm__ __volatile__(
        "ldrexh %[tmp], [%[dest]]\n\t"
        "mov %[failure], #1\n\t"
        "teq %[tmp], %[expected]\n\t"
        "it eq\n\t"
        "strexheq %[failure], %[desired], [%[dest]]\n\t"
        : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
        : [dest]"r"(&dest), [expected]"r"(expected), [desired]"r"(desired)
        : "cc", "memory");
    return !failure;
}
#endif

#if LASS_HAVE_LDREX_STREX
template <>
template <typename T>
inline bool AtomicOperations<4>::compareAndSwap(volatile T& dest, T expected, T desired)
{   
    LASS_ASSERT((reinterpret_cast<num::TuintPtr>(&dest) & 0x3) == 0); // requires word alignment
    
    uint32_t tmp;
    bool failure = false;
    __asm__ __volatile__(
        "ldrex %[tmp], [%[dest]]\n\t"
        "mov %[failure], #1\n\t"
        "teq %[tmp], %[expected]\n\t"
        "it eq\n\t"
        "strexeq %[failure], %[desired], [%[dest]]\n\t"
        : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
        : [dest]"r"(&dest), [expected]"r"(expected), [desired]"r"(desired)
        : "cc", "memory");
    return !failure;
}
#elif LASS_HAVE_KUSER_CMPXCHG
template <>
template <typename T>
inline bool AtomicOperations<4>::compareAndSwap(volatile T& dest, T expected, T desired)
{
    return lass_util_impl_kuser_cmpxchg(
        *reinterpret_cast<int32_t*>(&expected),
        *reinterpret_cast<int32_t*>(&desired),
        reinterpret_cast<volatile int32_t*>(&dest)
        ) == 0;
}
#endif

#if LASS_HAVE_LDREXD_STREXD
template <>
template <typename T> 
inline bool AtomicOperations<8>::compareAndSwap(volatile T& dest, T expected, T desired)
{
    LASS_ASSERT((reinterpret_cast<num::TuintPtr>(&dest) & 0x7) == 0); // requires double word alignment
    
    uint64_t tmp;
    bool failure = false;
    __asm__ __volatile__(
        "ldrexd %[tmp], %H[tmp], [%[dest]]\n\t"
        "mov %[failure], #1\n\t"
        "teq %[tmp], %[expected]\n\t"
        "itt eq\n\t"
        "teqeq %H[tmp], %H[expected]\n\t"
        "strexdeq %[failure], %[desired], %H[desired], [%[dest]]\n\t"
        : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
        : [dest]"r"(&dest), [expected]"r"(expected), [desired]"r"(desired)
        : "cc", "memory");
    return !failure;
}
#elif LASS_HAVE_KUSER_CMPXCHG64
template <>
template <typename T> 
inline bool AtomicOperations<8>::compareAndSwap(volatile T& dest, T expected, T desired)
{
    return lass_util_impl_kuser_cmpxchg64(
        reinterpret_cast<const int64_t*>(&expected),
        reinterpret_cast<const int64_t*>(&desired),
        reinterpret_cast<volatile int64_t*>(&dest)
        ) == 0;
}
#endif



// --- adjacent compareAndSwap ------------------------------------------------

#if LASS_HAVE_LDREXH_STREXH
template <>
template <typename T1, typename T2> 
inline bool AtomicOperations<1>::compareAndSwap(volatile T1& dest1, T1 expected1, T2 expected2, T1 desired1, T2 desired2)
{
    LASS_ASSERT((reinterpret_cast<num::TuintPtr>(&dest1) & 0x1) == 0); // requires half word alignment

    uint16_t tmp;
    bool failure = false;
    __asm__ __volatile__(
        "orr %[expected1], %[expected1], %[expected2], lsl #8\n\t"
        "orr %[desired1], %[desired1], %[desired2], lsl #8\n\t"
        "ldrexh %[tmp], [%[dest]]\n\t"
        "mov %[failure], #1\n\t"
        "teq %[tmp], %[expected1]\n\t"
        "it eq\n\t"
        "strexheq %[failure], %[desired1], [%[dest]]\n\t"
        : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
        : [dest]"r"(&dest1), [expected1]"r"(expected1), [expected2]"r"(expected2),  [desired1]"r"(desired1),  [desired2]"r"(desired2)
        : "cc", "memory");
    return !failure;
}
#endif

#if LASS_HAVE_LDREX_STREX
template <>
template <typename T1, typename T2> 
inline bool AtomicOperations<2>::compareAndSwap(volatile T1& dest1, T1 expected1, T2 expected2, T1 desired1, T2 desired2)
{
    LASS_ASSERT((reinterpret_cast<num::TuintPtr>(&dest1) & 0x3) == 0); // requires word alignment
    
    uint32_t tmp;
    bool failure = false;
    __asm__ __volatile__(
        "orr %[expected1], %[expected1], %[expected2], lsl #16\n\t"
        "orr %[desired1], %[desired1], %[desired2], lsl #16\n\t"
        "ldrex %[tmp], [%[dest]]\n\t"
        "mov %[failure], #1\n\t"
        "teq %[tmp], %[expected1]\n\t"
        "it eq\n\t"
        "strexeq %[failure], %[desired1], [%[dest]]\n\t"
        : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
        : [dest]"r"(&dest1), [expected1]"r"(expected1), [expected2]"r"(expected2),  [desired1]"r"(desired1),  [desired2]"r"(desired2)
        : "cc", "memory");
    return !failure;
}
#elif LASS_HAVE_KUSER_CMPXCHG
template <>
template <typename T1, typename T2> 
inline bool AtomicOperations<2>::compareAndSwap(volatile T1& dest1, T1 expected1, T2 expected2, T1 desired1, T2 desired2)
{
    const int32_t expected = static_cast<int32_t>(*reinterpret_cast<int16_t*>(&expected1)) | (static_cast<int32_t>(*reinterpret_cast<int16_t*>(&expected2)) << 16);
    const int32_t desired = static_cast<int32_t>(*reinterpret_cast<int16_t*>(&desired1)) | (static_cast<int32_t>(*reinterpret_cast<int16_t*>(&desired2)) << 16);
    return lass_util_impl_kuser_cmpxchg(expected, desired, reinterpret_cast<volatile int32_t*>(&dest1)) == 0;
}
#endif

#if LASS_HAVE_LDREXD_STREXD
template <>
template <typename T1, typename T2> 
inline bool AtomicOperations<4>::compareAndSwap(volatile T1& dest1, T1 expected1, T2 expected2, T1 desired1, T2 desired2)
{
    LASS_ASSERT((reinterpret_cast<num::TuintPtr>(&dest1) & 0x7) == 0); // requires double word alignment
    
    uint64_t desired;
    uint64_t tmp;
    bool failure = false;
    __asm__ __volatile__(
        "mov %[desired], %[desired1]\n\t"
        "mov %H[desired], %[desired2]\n\t"
        "ldrexd %[tmp], %H[tmp], [%[dest]]\n\t"
        "mov %[failure], #1\n\t"
        "teq %[tmp], %[expected1]\n\t"
        "itt eq\n\t"
        "teqeq %H[tmp], %[expected2]\n\t"
        "strexdeq %[failure], %[desired], %H[desired], [%[dest]]\n\t"
        : [desired]"=&r"(desired), [tmp]"=&r"(tmp), [failure]"=&r"(failure)
        : [dest]"r"(&dest1), [expected1]"r"(expected1), [expected2]"r"(expected2),  [desired1]"r"(desired1),  [desired2]"r"(desired2)
        : "cc", "memory");
    return !failure;
}
#elif LASS_HAVE_KUSER_CMPXCHG64
template <>
template <typename T1, typename T2> 
inline bool AtomicOperations<4>::compareAndSwap(volatile T1& dest1, T1 expected1, T2 expected2, T1 desired1, T2 desired2)
{
    const int64_t expected = static_cast<int64_t>(*reinterpret_cast<int32_t*>(&expected1)) | (static_cast<int64_t>(*reinterpret_cast<int32_t*>(&expected2)) << 32);
    const int64_t desired = static_cast<int64_t>(*reinterpret_cast<int32_t*>(&desired1)) | (static_cast<int64_t>(*reinterpret_cast<int32_t*>(&desired2)) << 32);
    return lass_util_impl_kuser_cmpxchg64(&expected, &desired, reinterpret_cast<volatile int64_t*>(&dest1)) == 0;
}
#endif



// --- increment --------------------------------------------------------------

#if LASS_HAVE_LDREXB_STREXB
template <>
template <typename T> 
inline void AtomicOperations<1>::increment(volatile T& value)
{
    uint8_t tmp;
    bool failure = false;
    do
    {
        __asm__ __volatile__(
            "ldrexb %[tmp], [%[value]]\n\t"
            "add %[tmp], %[tmp], #1\n\t"
            "strexb %[failure], %[tmp], [%[value]]\n\t"
            : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
            : [value]"r"(&value)
            : "cc", "memory");
    }
    while (failure);
}
#endif

#if LASS_HAVE_LDREXH_STREXH
template <>
template <typename T> 
inline void AtomicOperations<2>::increment(volatile T& value)
{
    LASS_ASSERT((reinterpret_cast<num::TuintPtr>(&value) & 0x1) == 0); // requires half word alignment

    uint16_t tmp;
    bool failure = false;
    do
    {
        __asm__ __volatile__(
            "ldrexh %[tmp], [%[value]]\n\t"
            "add %[tmp], %[tmp], #1\n\t"
            "strexh %[failure], %[tmp], [%[value]]\n\t"
            : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
            : [value]"r"(&value)
            : "cc", "memory");
    }
    while (failure);
}
#endif

#if LASS_HAVE_LDREX_STREX
template <>
template <typename T> 
inline void AtomicOperations<4>::increment(volatile T& value)
{
    LASS_ASSERT((reinterpret_cast<num::TuintPtr>(&value) & 0x3) == 0); // requires word alignment
    
    uint32_t tmp;
    bool failure = false;
    do
    {
        __asm__ __volatile__(
            "ldrex %[tmp], [%[value]]\n\t"
            "add %[tmp], %[tmp], #1\n\t"
            "strex %[failure], %[tmp], [%[value]]\n\t"
            : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
            : [value]"r"(&value)
            : "cc", "memory");
    }
    while (failure);
}
#endif

#if LASS_HAVE_LDREXD_STREXD
template <>
template <typename T> 
inline void AtomicOperations<8>::increment(volatile T& value)
{
    LASS_ASSERT((reinterpret_cast<num::TuintPtr>(&value) & 0x7) == 0); // requires double word alignment
    
    uint64_t tmp;
    bool failure = false;
    do
    {
        __asm__ __volatile__(
            "ldrexd %[tmp], %H[tmp], [%[value]]\n\t"
            "adds %[tmp], %[tmp], #1\n\t"
            "adc %H[tmp], %H[tmp], #0\n\t"
            "strexd %[failure], %[tmp], %H[tmp], [%[value]]\n\t"
            : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
            : [value]"r"(&value)
            : "cc", "memory");
    }
    while (failure);
}
#endif



// --- decrement --------------------------------------------------------------

#if LASS_HAVE_LDREXB_STREXB
template <>
template <typename T> 
inline void AtomicOperations<1>::decrement(volatile T& value)
{
    uint8_t tmp;
    bool failure = false;
    do
    {
        __asm__ __volatile__(
            "ldrexb %[tmp], [%[value]]\n\t"
            "sub %[tmp], %[tmp], #1\n\t"
            "strexb %[failure], %[tmp], [%[value]]\n\t"
        : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
        : [value]"r"(&value)
        : "cc", "memory");
    }
    while (failure);
}
#endif

#if LASS_HAVE_LDREXH_STREXH
template <>
template <typename T> 
inline void AtomicOperations<2>::decrement(volatile T& value)
{
    LASS_ASSERT((reinterpret_cast<num::TuintPtr>(&value) & 0x1) == 0); // requires half word alignment

    uint16_t tmp;
    bool failure = false;
    do
    {
        __asm__ __volatile__(
            "ldrexh %[tmp], [%[value]]\n\t"
            "sub %[tmp], %[tmp], #1\n\t"
            "strexh %[failure], %[tmp], [%[value]]\n\t"
        : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
        : [value]"r"(&value)
        : "cc", "memory");
    }
    while (failure);
}
#endif

#if LASS_HAVE_LDREX_STREX
template <>
template <typename T> 
inline void AtomicOperations<4>::decrement(volatile T& value)
{
    LASS_ASSERT((reinterpret_cast<num::TuintPtr>(&value) & 0x3) == 0); // requires word alignment
    
    uint32_t tmp;
    bool failure = false;
    do
    {
        __asm__ __volatile__(
            "ldrex %[tmp], [%[value]]\n\t"
            "sub %[tmp], %[tmp], #1\n\t"
            "strex %[failure], %[tmp], [%[value]]\n\t"
        : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
        : [value]"r"(&value)
        : "cc", "memory");
    }
    while (failure);
}
#endif

#if LASS_HAVE_LDREXD_STREXD
template <>
template <typename T> 
inline void AtomicOperations<8>::decrement(volatile T& value)
{
    LASS_ASSERT((reinterpret_cast<num::TuintPtr>(&value) & 0x7) == 0); // requires double word alignment
    
    uint64_t tmp;
    bool failure = false;
    do
    {
        __asm__ __volatile__(
            "ldrexd %[tmp], %H[tmp], [%[value]]\n\t"
            "subs %[tmp], %[tmp], #1\n\t"
            "sbc %H[tmp], %H[tmp], #0\n\t"
            "strexd %[failure], %[tmp], %H[tmp], [%[value]]\n\t"
            : [tmp]"=&r"(tmp), [failure]"=&r"(failure)
            : [value]"r"(&value)
            : "cc", "memory");
    }
    while (failure);
}
#endif

}
}
}

// EOF

