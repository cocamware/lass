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



/** @class lass::prim::XYZ
 *  @brief cyclic iterator over xyz indices
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_XYZ_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_XYZ_H

#include "prim_common.h"
#include "../num/modulo.h"

namespace lass
{
namespace prim
{

class LASS_DLL XYZ
{
public:


	enum { dimension = 3 };

	XYZ();
	XYZ(char iAxis);
	explicit XYZ(int iValue);
	explicit XYZ(const std::string& iAxis);

	char axis() const;
	operator int() const { return value_; } /**< convert axis to integer. 0 == @a x, 1 == @a y, 2 == @a z */
	operator size_t() const { return static_cast<size_t>(value_); } /**< convert axis to integer. 0 == @a x, 1 == @a y, 2 == @a z */

	XYZ& operator++();
	XYZ& operator--();
	XYZ operator++(int);
	XYZ operator--(int);

	XYZ& operator+=(int iOffset);
	XYZ& operator-=(int iOffset);

private:

	typedef num::Modulo<3, int> TValue;

	friend LASS_DLL bool LASS_CALL operator==(const XYZ& iA, const XYZ& iB);

	void reset(char iAxis);

	TValue value_;
};

LASS_DLL bool LASS_CALL operator==(const XYZ& iA, const XYZ& iB);
LASS_DLL bool LASS_CALL operator==(const XYZ& iA, char iB);
LASS_DLL bool LASS_CALL operator==(char iA, const XYZ& iB);
LASS_DLL bool LASS_CALL operator==(const XYZ& iA, const std::string& iB);
LASS_DLL bool LASS_CALL operator==(const std::string& iA, const XYZ& iB);

LASS_DLL bool LASS_CALL operator!=(const XYZ& iA, const XYZ& iB);
LASS_DLL bool LASS_CALL operator!=(const XYZ& iA, char iB);
LASS_DLL bool LASS_CALL operator!=(char iA, const XYZ& iB);
LASS_DLL bool LASS_CALL operator!=(const XYZ& iA, const std::string&  iB);
LASS_DLL bool LASS_CALL operator!=(const std::string&  iA, const XYZ& iB);

LASS_DLL XYZ LASS_CALL operator+(const XYZ& iA, int iOffset);
LASS_DLL XYZ LASS_CALL operator-(const XYZ& iA, int iOffset);

/** @relates lass::prim::XY
 */
template <typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& ioS, const XYZ& iXYZ)
{
	ioS << iXYZ.axis();
	return ioS;
}



}

}

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_XYZ
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "../python/export_traits_prim.h"
#endif

#endif

// EOF
