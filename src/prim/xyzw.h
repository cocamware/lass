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



/** @class lass::prim::XYZW
 *  @brief cyclic iterator over xyzw indices
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_XYZW_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_XYZW_H

#include "prim_common.h"
#include "../num/modulo.h"

namespace lass
{
namespace prim
{

class XYZW
{
public:


    enum { dimension = 4 };

    XYZW() throw();
	XYZW(char iAxis);
    explicit XYZW(int iValue) throw();
	explicit XYZW(const std::string& iAxis);

	const char axis() const throw();
	operator int() const throw() { return value_; } /**< convert axis to integer. 
												        0 == @a x, 1 == @a y, 2 == @a z, 3 == @a w */

    XYZW& operator++() throw();
    XYZW& operator--() throw();
    XYZW operator++(int) throw();
    XYZW operator--(int) throw();

    XYZW& operator+=(int iOffset) throw();
    XYZW& operator-=(int iOffset) throw();

private:

    typedef num::Modulo<4, int> TValue;

    friend bool operator==(const XYZW& iA, const XYZW& iB) throw();

    void reset(char iAxis);

    TValue value_;
};

bool operator==(const XYZW& iA, const XYZW& iB) throw();
bool operator==(const XYZW& iA, char iB) throw();
bool operator==(char iA, const XYZW& iB) throw();
bool operator==(const XYZW& iA, const std::string& iB);
bool operator==(const std::string& iA, const XYZW& iB);

bool operator!=(const XYZW& iA, const XYZW& iB) throw();
bool operator!=(const XYZW& iA, char iB) throw();
bool operator!=(char iA, const XYZW& iB) throw();
bool operator!=(const XYZW& iA, const std::string&  iB);
bool operator!=(const std::string&  iA, const XYZW& iB);

XYZW operator+(const XYZW& iA, int iOffset) throw();
XYZW operator-(const XYZW& iA, int iOffset) throw();

std::ostream& operator<<(std::ostream& ioS, const XYZW& iXY);



}

}

#define LASS_PRIM_PYOBJECT_UTIL_XYZW
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_UTIL_H
#	include "pyobject_util.h"
#endif

#endif

// EOF
