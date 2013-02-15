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

#include "pylass_common.h"
#include "splines.h"

namespace pylass
{

PY_DECLARE_CLASS(SplineLinear)
PY_CLASS_CONSTRUCTOR_0(SplineLinear)
PY_CLASS_CONSTRUCTOR_1(SplineLinear, const SplineLinear::TPairSequence&)
PY_CLASS_CONSTRUCTOR_2(SplineLinear, const SplineLinear::TSequence&, const SplineLinear::TSequence&)
PY_CLASS_METHOD_NAME(SplineLinear, operator(), "__call__")
PY_CLASS_METHOD(SplineLinear, derivative)
PY_CLASS_METHOD(SplineLinear, derivative2)
PY_CLASS_METHOD(SplineLinear, integral)
PY_CLASS_METHOD(SplineLinear, isEmpty)

PY_DECLARE_CLASS(SplineCubic)
PY_CLASS_CONSTRUCTOR_0(SplineCubic)
PY_CLASS_CONSTRUCTOR_1(SplineCubic, const SplineCubic::TPairSequence&)
PY_CLASS_CONSTRUCTOR_2(SplineCubic, const SplineCubic::TSequence&, const SplineCubic::TSequence&)
PY_CLASS_METHOD_NAME(SplineCubic, operator(), "__call__")
PY_CLASS_METHOD(SplineCubic, derivative)
PY_CLASS_METHOD(SplineCubic, derivative2)
PY_CLASS_METHOD(SplineCubic, integral)
PY_CLASS_METHOD(SplineCubic, isEmpty)

}
