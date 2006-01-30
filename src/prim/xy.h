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



/** @class lass::prim::XY
 *  @brief cyclic iterator over xy indices
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_XY_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_XY_H

#include "prim_common.h"
#include "../num/modulo.h"

namespace lass
{
namespace prim
{

class LASS_DLL XY
{
public:


	enum { dimension = 2 };

	XY();
	XY(char iAxis);
	explicit XY(int iValue);
	explicit XY(const std::string& iAxis);

	const char axis() const;
	operator int() const { return value_; } /**< convert axis to integer.
														0 == @a x, 1 == @a y */

	XY& operator++();
	XY& operator--();
	XY operator++(int);
	XY operator--(int);

	XY& operator+=(int iOffset);
	XY& operator-=(int iOffset);

private:

	typedef num::Modulo<2, int> TValue;

	friend LASS_DLL bool LASS_CALL operator==(const XY& iA, const XY& iB);

	void reset(char iAxis);

	TValue value_;
};

LASS_DLL bool LASS_CALL operator==(const XY& iA, const XY& iB);
LASS_DLL bool LASS_CALL operator==(const XY& iA, char iB);
LASS_DLL bool LASS_CALL operator==(char iA, const XY& iB);
LASS_DLL bool LASS_CALL operator==(const XY& iA, const std::string& iB);
LASS_DLL bool LASS_CALL operator==(const std::string& iA, const XY& iB);

LASS_DLL bool LASS_CALL operator!=(const XY& iA, const XY& iB);
LASS_DLL bool LASS_CALL operator!=(const XY& iA, char iB);
LASS_DLL bool LASS_CALL operator!=(char iA, const XY& iB);
LASS_DLL bool LASS_CALL operator!=(const XY& iA, const std::string&  iB);
LASS_DLL bool LASS_CALL operator!=(const std::string&  iA, const XY& iB);

LASS_DLL XY LASS_CALL operator+(const XY& iA, int iOffset);
LASS_DLL XY LASS_CALL operator-(const XY& iA, int iOffset);

/** @relates lass::prim::XY
 */
template <typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& ioS, const XY& iXY)
{
	ioS << iXY.axis();
	return ioS;
}



}

}

#define LASS_PRIM_PYOBJECT_UTIL_XY
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#endif

// EOF
