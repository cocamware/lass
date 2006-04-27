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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PY_TUPLE_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PY_TUPLE_INL

#include "util_common.h"

namespace lass
{
namespace python
{

inline const PyObjectPtr<PyObject>::Type makeTuple()
{
	typedef PyObjectPtr<PyObject>::Type TPyPtr;
	TPyPtr tuple(PyTuple_New(0));
	return tuple;
}

$[
template <$(typename P$x)$>
const PyObjectPtr<PyObject>::Type makeTuple($(const P$x& iP$x)$)
{
	typedef PyObjectPtr<PyObject>::Type TPyPtr;
	TPyPtr tuple(PyTuple_New($x));
	$(if (PyTuple_SetItem(tuple.get(), $x - 1, pyBuildSimpleObject(iP$x)) != 0) return TPyPtr();
	)$
	return tuple;
}
]$

inline int decodeTuple(PyObject* iTuple)
{
	LASS_ASSERT(iTuple);
	return impl::checkSequenceSize(iTuple, 0) ? 0 : 1;
}

inline int decodeTuple(const PyObjectPtr<PyObject>::Type& iTuple)
{
	return decodeTuple(iTuple.get());
}

$[
template <$(typename P$x)$>
int decodeTuple(PyObject* iTuple, $(P$x& oP$x)$)
{
	PyObject $(*p$x)$;

	LASS_ASSERT(iTuple);
	if (!PyTuple_Check(iTuple))
	{
		PyErr_SetString(PyExc_TypeError, "iTuple isn't a tuple");
		return 1;
	}


	if (!PyArg_UnpackTuple(iTuple, "", $x, $x, $(&p$x)$))
	{
		return 1;
	}

	$(if (pyGetSimpleObject(p$x, oP$x) != 0)
	{
		impl::addMessageHeader("Bad Argument on $xth position");
		return 1;
	}
	)$
	return 0;
}

template <$(typename P$x)$> inline
int decodeTuple(const PyObjectPtr<PyObject>::Type& iTuple, $(P$x& oP$x)$)
{
	return decodeTuple(iTuple.get(), $(oP$x)$);
}
]$

}

}

#endif

// EOF
