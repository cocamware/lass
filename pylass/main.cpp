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
#include "aabb_tree_2d.h"
#include "filters.h"
#include "image.h"
#include "kd_tree_2d.h"
#include "mesh_interpolator.h"
#include "polynomial.h"
#include "splines.h"
#include "containers.h"

PY_DECLARE_MODULE_NAME(modlass, "lass")

PY_MODULE_CLASS(modlass, pylass::AabbTree2D)
PY_MODULE_CLASS(modlass, kd_tree::KdTree2D)
PY_MODULE_CLASS(modlass, mesh_interpolator::LinearMeshInterpolator)
PY_MODULE_CLASS(modlass, Containers)
PY_MODULE_CLASS(modlass, pylass::FirFilter)
PY_MODULE_CLASS(modlass, pylass::IirFilter)
PY_MODULE_CLASS(modlass, pylass::Polynomial)
PY_MODULE_CLASS(modlass, pylass::PolynomialQuotient)
PY_MODULE_CLASS(modlass, pylass::ComplexPolynomial)
PY_MODULE_CLASS(modlass, pylass::ComplexPolynomialQuotient)
PY_MODULE_CLASS(modlass, pylass::SplineLinear)
PY_MODULE_CLASS(modlass, pylass::SplineCubic)
PY_MODULE_CLASS(modlass, pylass::Image)

void postInject(PyObject*)
{
#if !defined(NDEBUG) || defined(_DEBUG)
	PY_MODULE_ADD_INTEGER_CONSTANT( modlass, "DEBUG", true )
#else
	PY_MODULE_ADD_INTEGER_CONSTANT( modlass, "DEBUG", false )
#endif
}

LASS_EXECUTE_BEFORE_MAIN(
	modlass.setPostInject(postInject);
	)

PY_MODULE_ENTRYPOINT_NAME(modlass, lass)

// EOF
