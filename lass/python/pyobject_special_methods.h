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
 *	Copyright (C) 2004-2025 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_SPECIAL_METHODS_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_SPECIAL_METHODS_H

#include "python_common.h"

namespace lass
{
namespace python
{

namespace impl
{
	struct SpecialSlot
	{ 
		SpecialSlot( const char* name, int slot ) : name(name), slot(slot) {}
		const char* name;
		int slot;
	};

	struct UnarySlot : public SpecialSlot { UnarySlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct BinarySlot : public SpecialSlot { BinarySlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct TernarySlot : public SpecialSlot { TernarySlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct LenSlot : public SpecialSlot { LenSlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct ComparatorSlot : public SpecialSlot { ComparatorSlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct SsizeArgSlot : public SpecialSlot { SsizeArgSlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct SsizeSsizeArgSlot : public SpecialSlot { SsizeSsizeArgSlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct SsizeObjArgSlot : public SpecialSlot { SsizeObjArgSlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct SsizeSsizeObjArgSlot : public SpecialSlot { SsizeSsizeObjArgSlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct ObjObjSlot : public SpecialSlot { ObjObjSlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct ObjObjArgSlot : public SpecialSlot { ObjObjArgSlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct IterSlot : public SpecialSlot { IterSlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct IterNextSlot : public SpecialSlot { IterNextSlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct ArgKwSlot : public SpecialSlot { ArgKwSlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };
	struct InquirySlot: public SpecialSlot { InquirySlot( const char* name, int slot ) : SpecialSlot(name, slot) {} };

	const int charPtrSlot = 1;
	const int unarySlot = 2;
	const int binarySlot = 3;
	const int ternarySlot = 4;
	const int lenSlot = 5;
	const int comparatorSlot = 6;
	const int ssizeArgSlot = 7;
	const int ssizeSsizeArgSlot = 8;
	const int ssizeObjArgSlot = 9;
	const int ssizeSsizeObjArgSlot = 10;
	const int objObjSlot = 11;
	const int objObjArgSlot = 12;
	const int iterSlot = 13;
	const int iterNextSlot = 14;
	const int argKwSlot = 15;
	const int inquirySlot = 16;


	template<int value> struct MetaInt { volatile const char dummy[value]; };

	struct IsSpecialSlotTester
	{
		static MetaInt<charPtrSlot> test(char *);
		static MetaInt<charPtrSlot> test(const char *);
		static MetaInt<unarySlot> test(const UnarySlot&);
		static MetaInt<binarySlot> test(const BinarySlot&);
		static MetaInt<ternarySlot> test(const TernarySlot&);
		static MetaInt<lenSlot> test(const LenSlot&);
		static MetaInt<comparatorSlot> test(const ComparatorSlot&);
		static MetaInt<ssizeArgSlot> test(const SsizeArgSlot&);
		static MetaInt<ssizeSsizeArgSlot> test(const SsizeSsizeArgSlot&);
		static MetaInt<ssizeObjArgSlot> test(const SsizeObjArgSlot&);
		static MetaInt<ssizeSsizeObjArgSlot> test(const SsizeSsizeObjArgSlot&);
		static MetaInt<objObjSlot> test(const ObjObjSlot&);
		static MetaInt<objObjArgSlot> test(const ObjObjArgSlot&);
		static MetaInt<iterSlot> test(const IterSlot&);
		static MetaInt<iterNextSlot> test(const IterNextSlot&);
		static MetaInt<argKwSlot> test(const ArgKwSlot&);
		static MetaInt<inquirySlot> test(const InquirySlot&);
	};

	template<int value>
	struct IsSpecialSlotTesterValue
	{ 
	};
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<charPtrSlot>)> { typedef char* Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<unarySlot>)> { typedef UnarySlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<binarySlot>)> { typedef BinarySlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<ternarySlot>)> { typedef TernarySlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<lenSlot>)> { typedef LenSlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<comparatorSlot>)> { typedef ComparatorSlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<ssizeArgSlot>)> { typedef SsizeArgSlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<ssizeSsizeArgSlot>)> { typedef SsizeSsizeArgSlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<ssizeObjArgSlot>)> { typedef SsizeObjArgSlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<ssizeSsizeObjArgSlot>)> { typedef SsizeSsizeObjArgSlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<objObjSlot>)> { typedef ObjObjSlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<objObjArgSlot>)> { typedef ObjObjArgSlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<iterSlot>)> { typedef IterSlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<iterNextSlot>)> { typedef IterNextSlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<argKwSlot>)> { typedef ArgKwSlot Type; };
	template<> struct IsSpecialSlotTesterValue<sizeof(MetaInt<inquirySlot>)> { typedef InquirySlot Type; };

	#define SPECIAL_SLOT_TYPE( x ) lass::python::impl::IsSpecialSlotTesterValue<sizeof(lass::python::impl::IsSpecialSlotTester::test(x))>::Type

}

namespace methods
{
	// special methods
	const lass::python::impl::ArgKwSlot _call_("__call__", Py_tp_call);
	const lass::python::impl::UnarySlot _repr_("__repr__", Py_tp_repr);
	const lass::python::impl::UnarySlot _str_("__str__", Py_tp_str);

	// PyNumberMethods
	// unary
	const lass::python::impl::UnarySlot _neg_("__neg__", Py_nb_negative);
	const lass::python::impl::UnarySlot _pos_("__pos__", Py_nb_positive);
	const lass::python::impl::UnarySlot _abs_("__abs__", Py_nb_absolute);
	const lass::python::impl::UnarySlot _invert_("__invert__", Py_nb_invert);
	const lass::python::impl::UnarySlot _int_("__int__", Py_nb_int);
	const lass::python::impl::UnarySlot _long_("__long__", Py_nb_int); // deprecated
	const lass::python::impl::UnarySlot _float_("__float__", Py_nb_float);

	// binary
	const lass::python::impl::BinarySlot _add_("__add__", Py_nb_add);
	const lass::python::impl::BinarySlot _sub_("__sub__", Py_nb_subtract);
	const lass::python::impl::BinarySlot _mul_("__mul__", Py_nb_multiply);
	const lass::python::impl::BinarySlot _mod_("__mod__", Py_nb_remainder);
	const lass::python::impl::BinarySlot _lshift_("__lshift__", Py_nb_lshift);
	const lass::python::impl::BinarySlot _rshift_("__rshift__", Py_nb_rshift);
	const lass::python::impl::BinarySlot _and_("__and__", Py_nb_and);
	const lass::python::impl::BinarySlot _xor_("__xor__", Py_nb_xor);
	const lass::python::impl::BinarySlot _or_("__or__", Py_nb_or);
	const lass::python::impl::BinarySlot _iadd_("__iadd__", Py_nb_inplace_add);
	const lass::python::impl::BinarySlot _isub_("__isub__", Py_nb_inplace_subtract);
	const lass::python::impl::BinarySlot _imul_("__imul__", Py_nb_inplace_multiply);
	const lass::python::impl::BinarySlot _imod_("__imod__", Py_nb_inplace_remainder);
	const lass::python::impl::BinarySlot _ilshift_("__ilshift__", Py_nb_inplace_lshift);
	const lass::python::impl::BinarySlot _irshift_("__irshift__", Py_nb_inplace_rshift);
	const lass::python::impl::BinarySlot _iand_("__iand__", Py_nb_inplace_and);
	const lass::python::impl::BinarySlot _ixor_("__ixor__", Py_nb_inplace_xor);
	const lass::python::impl::BinarySlot _ior_("__ior__", Py_nb_inplace_or);
	const lass::python::impl::BinarySlot _truediv_("__truediv__", Py_nb_true_divide);
	const lass::python::impl::BinarySlot _itruediv_("__itruediv__", Py_nb_inplace_true_divide);
	const lass::python::impl::BinarySlot _floordiv_("__floordiv__", Py_nb_floor_divide);
	const lass::python::impl::BinarySlot _ifloordiv_("__ifloordiv__", Py_nb_inplace_floor_divide);
	const lass::python::impl::BinarySlot _matmul_("__matmul__", Py_nb_matrix_multiply);
	const lass::python::impl::BinarySlot _imatmul_("__imatmul__", Py_nb_inplace_matrix_multiply);
	const lass::python::impl::InquirySlot _bool_("__bool__", Py_nb_bool);
	const lass::python::impl::InquirySlot _nonzero_ = _bool_; // the 2.x version of __bool__

	const lass::python::impl::TernarySlot _pow_("__pow__", Py_nb_power);
	const lass::python::impl::TernarySlot _ipow_("__ipow__", Py_nb_inplace_power);

	// Comparators
	const lass::python::impl::ComparatorSlot _lt_("__lt__", Py_LT);
	const lass::python::impl::ComparatorSlot _le_("__le__", Py_LE);
	const lass::python::impl::ComparatorSlot _eq_("__eq__", Py_EQ);
	const lass::python::impl::ComparatorSlot _ne_("__ne__", Py_NE);
	const lass::python::impl::ComparatorSlot _gt_("__gt__", Py_GT);
	const lass::python::impl::ComparatorSlot _ge_("__ge__", Py_GE);


	// shared between PySequenceMethods and PyMappingMethods
	// the PySequenceMethods gets priority in this case
	const lass::python::impl::LenSlot _len_("__seq_len__", Py_sq_length);
	const lass::python::impl::SsizeArgSlot _getitem_("__seq_getitem__", Py_sq_item);
	const lass::python::impl::SsizeObjArgSlot _setitem_("__seq_setitem__", Py_sq_ass_item);


	// PySequenceMethods
	const lass::python::impl::LenSlot seq_len_("__seq_len__", Py_sq_length);
	const lass::python::impl::BinarySlot _concat_("__concat__", Py_sq_concat);
	const lass::python::impl::SsizeArgSlot _repeat_("__repeat__", Py_sq_repeat);
	const lass::python::impl::SsizeArgSlot seq_getitem_("__seq_getitem__", Py_sq_item);	/**< to disambiguate with the PyMapping protocol */
	const lass::python::impl::SsizeObjArgSlot seq_setitem_("__seq_setitem__", Py_sq_ass_item);
	const lass::python::impl::ObjObjSlot _contains_("__contains__", Py_sq_contains);
	const lass::python::impl::BinarySlot _iconcat_("__iconcat__", Py_sq_inplace_concat);
	const lass::python::impl::SsizeArgSlot _irepeat_("__irepeat__", Py_sq_inplace_repeat);

	// PyMappingMethods
	const lass::python::impl::LenSlot map_len_("__map_len__", Py_mp_length);
	const lass::python::impl::BinarySlot map_getitem_("__map_getitem__", Py_mp_subscript);
	const lass::python::impl::ObjObjArgSlot map_setitem_("__map_setitem__", Py_mp_ass_subscript);
	const lass::python::impl::ObjObjArgSlot map_delitem_ = map_setitem_; // they share the same slot

	// Iterator Protocol
	const lass::python::impl::IterSlot _iter_("__iter__", Py_tp_iter);
	const lass::python::impl::IterNextSlot next("next", Py_tp_iternext);
}

}
}

#endif
