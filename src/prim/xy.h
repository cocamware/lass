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

class XY
{
public:


    enum { dimension = 2 };

    XY() throw();
	XY(char iAxis);
    explicit XY(int iValue) throw();
	explicit XY(const std::string& iAxis);

	const char axis() const throw();
	operator int() const throw() { return value_; } /**< convert axis to integer. 
												        0 == @a x, 1 == @a y */

    XY& operator++() throw();
    XY& operator--() throw();
    XY operator++(int) throw();
    XY operator--(int) throw();

    XY& operator+=(int iOffset) throw();
    XY& operator-=(int iOffset) throw();

private:

    typedef num::Modulo<2, int> TValue;

    friend bool operator==(const XY& iA, const XY& iB) throw();

    void reset(char iAxis);

    TValue value_;
};

bool operator==(const XY& iA, const XY& iB) throw();
bool operator==(const XY& iA, char iB) throw();
bool operator==(char iA, const XY& iB) throw();
bool operator==(const XY& iA, const std::string& iB);
bool operator==(const std::string& iA, const XY& iB);

bool operator!=(const XY& iA, const XY& iB) throw();
bool operator!=(const XY& iA, char iB) throw();
bool operator!=(char iA, const XY& iB) throw();
bool operator!=(const XY& iA, const std::string&  iB);
bool operator!=(const std::string&  iA, const XY& iB);

XY operator+(const XY& iA, int iOffset) throw();
XY operator-(const XY& iA, int iOffset) throw();

std::ostream& operator<<(std::ostream& ioS, const XY& iXY);



}

}

#define LASS_PRIM_PYOBJECT_UTIL_XY
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#endif

// EOF
