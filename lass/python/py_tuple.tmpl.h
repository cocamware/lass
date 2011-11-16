/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#include "python_common.h"
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
	template <typename P>
	inline bool decodeObject(PyObject* in, P& out, size_t index)
	{
		if (lass::python::pyGetSimpleObject(in, out) != 0)
		{
			std::ostringstream buffer;
			buffer << "Bad Argument on " << index << "th position";
			impl::addMessageHeader(buffer.str().c_str());
			return false;
		}
		return true;
	}
}

/** @ingroup Python
 */
inline const TPyObjPtr makeTuple()
{
	return TPyObjPtr(PyTuple_New(0));
}

// we don't use PyTuple_Pack, because we want to steal references from pyBuildSimpleObject [Bramz]
$[
/** @ingroup Python
 */
template <$(typename P$x)$>
const TPyObjPtr makeTuple($(const P$x& p$x)$)
{
	LockGIL LASS_UNUSED(lock);
	TPyObjPtr tuple(PyTuple_New($x));
 	$(if (PyTuple_SetItem(tuple.get(), $w, pyBuildSimpleObject(p$x)) != 0) return TPyObjPtr();
 	)$
	return tuple;
}
]$

/** @ingroup Python
 */
inline int decodeTuple(PyObject* obj)
{
	LASS_ASSERT(obj);
	return impl::checkSequenceSize(obj, 0) ? 0 : 1;
}

/** @ingroup Python
 */
inline int decodeTuple(const TPyObjPtr& obj)
{
	return decodeTuple(obj.get());
}

$[
/** @ingroup Python
 */
template <$(typename P$x)$>
int decodeTuple(PyObject* obj, $(P$x& p$x)$)
{
	LockGIL LASS_UNUSED(lock);
	const TPyObjPtr tuple = impl::checkedFastSequence(obj, $x);
	if (!tuple)
	{
		return 1;
	}
	PyObject** objects = PySequence_Fast_ITEMS(tuple.get());
	return true
$(		&& impl::decodeObject(objects[$w], p$x, $x) 
)$		? 0 : 1;

}

/** @ingroup Python
 */
template <$(typename P$x)$> inline
int decodeTuple(const TPyObjPtr& obj, $(P$x& p$x)$)
{
	return decodeTuple(obj.get(), $(p$x)$);
}
]$

$[
/** @ingroup Python
 */
template <$(typename P$x)$>
int decodeTupleMinimum(PyObject* obj, Py_ssize_t minumumLength, $(P$x& p$x)$)
{
	LockGIL LASS_UNUSED(lock);
	const TPyObjPtr tuple = impl::checkedFastSequence(obj, minumumLength, $x);
	if (!tuple)
	{
		return 1;
	}
	const Py_ssize_t n = PySequence_Fast_GET_SIZE(tuple.get());
	PyObject** objects = PySequence_Fast_ITEMS(tuple.get());
	return true
$(		&& ($x > n || impl::decodeObject(objects[$w], p$x, $x))
)$		? 0 : 1;

}

/** @ingroup Python
 */
template <$(typename P$x)$> inline
int decodeTupleMinimum(const TPyObjPtr& obj, Py_ssize_t minumumLength, $(P$x& p$x)$)
{
	return decodeTupleMinimum(obj.get(), minumumLength, $(p$x)$);
}
]$

}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif

// EOF
