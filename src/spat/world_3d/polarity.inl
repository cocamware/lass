/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2004
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_POLARITY_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_POLARITY_INL

#include "../spat_common.h"
#include "polarity.h"

namespace lass
{
namespace spat
{
namespace world_3d
{

// --- public --------------------------------------------------------------------------------------

/** Construct polarity by unsigned int
 *  @param iValue
 *		@arg 0: negative
 *		@arg 1: positive
 */
inline Polarity::Polarity(unsigned iValue):
    value_(iValue)
{
    LASS_ASSERT(value_ == 0 || value_ == 1);
}



// --- PUBLIC OPERATORS --------------------------------------------------------

/** Get polarity as unsigned int. 
 *  @return 
 *		@arg 0: negative
 *      @arg 1: positive
 */
inline Polarity::operator unsigned() const
{ 
    return value_; 
}



/** NOT operation. 
 *  @return
 *		@arg positive if this is negative.
 *		@arg negative if this is positive.
 */
inline Polarity Polarity::operator!() const
{
    return Polarity(1 - value_);
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** AND operation.
 *  @return
 *		@arg positive: both arguments are positive.
 *		@arg negative: at least one of the arguments is negative.
 */
inline Polarity operator&&(Polarity iA, Polarity iB)
{
    return Polarity(iA.value_ & iB.value_);
}
 


/** OR operation.
 *  @return
 *		@arg positive: at least one of the arguments is positive.
 *		@arg negative: both arguments are negative.
 */
inline Polarity operator||(Polarity iA, Polarity iB)
{
    return Polarity(iA.value_ | iB.value_);
}



/** Equal compare operation.
 *  @return 
 *		@arg positive: both arguments are equal, i.e. both are positive or both are negative.
 *		@arg negative: both arguments differ, i.e. one is positive and one is negative.
 */
inline Polarity operator==(Polarity iA, Polarity iB)
{
    return Polarity(iA.value_ == iB.value_);
}



/** XOR operation.
 *  @return
 *		@arg positive: both arguments differ, i.e. one is positive and one is negative.
 *		@arg negative: both arguments are equal, i.e. both are positive or both are negative.
 */
inline Polarity operator!=(Polarity iA, Polarity iB)
{
    return Polarity(iA.value_ != iB.value_);
}



}

}

}

#endif

// EOF
