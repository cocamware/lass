/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PY_TUPLE_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PY_TUPLE_H

#include "util_common.h"
#include "pyobject_plus.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4267) // conversion from 'size_t' to 'unsigned int', possible loss of data
#endif

namespace lass
{
namespace python
{
namespace impl
{
	/** @ingroup Python
	 *  @internal
	 */
	inline bool checkTuple(PyObject* iTuple)
	{
		LASS_ASSERT(iTuple);
		if (!PyTuple_Check(iTuple))
		{
			PyErr_SetString(PyExc_TypeError, "iTuple isn't a tuple");
			return false;
		}
		return true;
	}

	/** @ingroup Python
	 *  @internal
	 */
	template <typename P>
	inline bool decodeObject(PyObject* in, P& out, int index)
	{
		if (lass::python::pyExportTraitGet(in, out) != 0)
		{
			std::ostringstream buffer;
			buffer << "Bad Argument on " << index << "th position";
			impl::addMessageHeader(buffer.str());
			return false;
		}
		return true;
	}
}

/** @ingroup Python
 */
inline const PyObjectPtr<PyObject>::Type makeTuple()
{
	return TPyObjPtr(PyTuple_New(0));
}

// we don't use PyTuple_Pack, because we want to steal references from pyBuildSimpleObject [Bramz]
$[
/** @ingroup Python
 */
template <$(typename P$x)$>
const PyObjectPtr<PyObject>::Type makeTuple($(const P$x& iP$x)$)
{
	typedef PyObjectPtr<PyObject>::Type TPyObjPtr;
	TPyObjPtr tuple(PyTuple_New($x));
 	$(if (PyTuple_SetItem(tuple.get(), $x - 1, lass::python::PyExportTraits< P$x >::build(iP$x)) != 0) return TPyObjPtr();
 	)$
	return tuple;
}
]$

/** @ingroup Python
 */
inline int decodeTuple(PyObject* iTuple)
{
	LASS_ASSERT(iTuple);
	return impl::checkSequenceSize(iTuple, 0) ? 0 : 1;
}

/** @ingroup Python
 */
inline int decodeTuple(const PyObjectPtr<PyObject>::Type& iTuple)
{
	return decodeTuple(iTuple.get());
}

$[
/** @ingroup Python
 */
template <$(typename P$x)$>
int decodeTuple(PyObject* iTuple, $(P$x& oP$x)$)
{
	PyObject $(*p$x)$;
	return impl::checkTuple(iTuple)
		&& PyArg_UnpackTuple(iTuple, "", $x, $x, $(&p$x)$)
$(		&& impl::decodeObject(p$x, oP$x, $x) 
)$		? 0 : 1;

}

/** @ingroup Python
 */
template <$(typename P$x)$> inline
int decodeTuple(const PyObjectPtr<PyObject>::Type& iTuple, $(P$x& oP$x)$)
{
	return decodeTuple(iTuple.get(), $(oP$x)$);
}
]$

}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif

// EOF
