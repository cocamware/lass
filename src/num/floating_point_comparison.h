/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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
 *  You should have received iA copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_TOLERANCE_COMPARISON_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_TOLERANCE_COMPARISON_H

#include <cmath>

namespace lass
{
namespace num
{

template <typename T>
bool almostEqual(T iA, T iB, T iRelativeTolerance)
{
	return num::abs(iA - iB) < iRelativeTolerance * std::max(iA, iB);
}

template <typename T>
bool notAlmostEqual(T iA, T iB, T iRelativeTolerance)
{
	return !almostEqual(iA, iB, iRelativeTolerance);
}

template <typename T>
bool almostLess(T iA, T iB, T iRelativeTolerance)
{
	return iA < iB && !almostEqual(iA, iB, iRelativeTolerance);
}

template <typename T>
bool almostGreater(T iA, T iB, T iRelativeTolerance)
{
	return iA > iB && !almostEqual(iA, iB, iRelativeTolerance);
}

template <typename T>
bool almostLessOrEqual(T iA, T iB, T iRelativeTolerance)
{
	return iA <= iB || almostEqual(iA, iB, iRelativeTolerance);
}

template <typename T>
bool almostGreaterOrEqual(T iA, T iB, T iRelativeTolerance)
{
	return iA >= iB || almostEqual(iA, iB, iRelativeTolerance);
}

template <typename T>
bool almostInOpenRange(T iA, T iMin, T iMax, T iRelativeTolerance)
{
	return almostGreater(iA, iMin, iRelativeTolerance) && 
		almostLess(iA, iMax, iRelativeTolerance);
}

template <typename T>
bool almostInClosedRange(T iA, T iMin, T iMax, T iRelativeTolerance)
{
	return almostGreaterOrEqual(iA, iMin, iRelativeTolerance) && 
		almostLessOrEqual(iA, iMax, iRelativeTolerance);
}

}

}

#endif
