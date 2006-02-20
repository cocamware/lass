/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
 *
 *  LASS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

	const char axis() const;
	operator int() const { return value_; } /**< convert axis to integer.
														0 == @a x, 1 == @a y, 2 == @a z */

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

#include "pyobject_util.inl"

#endif

// EOF
