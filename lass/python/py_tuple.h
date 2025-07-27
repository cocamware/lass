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
 *	Copyright (C) 2004-2025 the Initial Developer.
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
#include <utility>
#include <tuple>

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
	inline bool tupleSetItems(PyObject* tuple, Py_ssize_t index, const P& p)
	{
		return PyTuple_SetItem(tuple, index, pyBuildSimpleObject(p)) == 0;
	}

	/** @ingroup Python
	 *  @internal
	 */
	template <typename P, typename... Ptail>
	inline bool tupleSetItems(PyObject* tuple, Py_ssize_t index, const P& p, Ptail&... tail)
	{
		return PyTuple_SetItem(tuple, index, pyBuildSimpleObject(p)) == 0
			&& tupleSetItems(tuple, index + 1, tail...);
	}

	/** @ingroup Python
	 *  @internal
	 */
	template <typename P>
	inline bool decodeObject(PyObject* in, Py_ssize_t index, P& out)
	{
		if (lass::python::pyGetSimpleObject(in, out) != 0)
		{
			std::ostringstream buffer;
			buffer << "Bad Argument on " << (index + 1) << "th position";
			impl::addMessageHeader(buffer.str().c_str());
			return false;
		}
		return true;
	}

	/** @ingroup Python
	 *  @internal
	 */
	template <typename P>
	inline bool decodeObjects(const FastSequence& objects, Py_ssize_t index, P& p)
	{
		return impl::decodeObject(objects[index], index, p);
	}

	/** @ingroup Python
	 *  @internal
	 */
	template <typename P, typename... Ptail>
	inline bool decodeObjects(const FastSequence& objects, Py_ssize_t index, P& p, Ptail&... tail)
	{
		return impl::decodeObject(objects[index], index, p)
			&& decodeObjects(objects, index + 1, tail...);
	}

	/** @ingroup Python
	 *  @internal
	 */
	template <typename P>
	inline bool decodeObjectsMinimum(const FastSequence& objects, Py_ssize_t index, P& p)
	{
		return index >= objects.size()
			|| impl::decodeObject(objects[index], index, p);
	}

	/** @ingroup Python
	 *  @internal
	 */
	template <typename P, typename... Ptail>
	inline bool decodeObjectsMinimum(const FastSequence& objects, Py_ssize_t index, P& p, Ptail&... tail)
	{
		return index >= objects.size()
			|| (impl::decodeObject(objects[index], index, p)
				&& decodeObjectsMinimum(objects, index + 1, tail...));
	}
}



/** @ingroup Python
 */
inline const TPyObjPtr makeTuple()
{
	return TPyObjPtr(PyTuple_New(0));
}

/** @ingroup Python
 */
template <typename... P>
const TPyObjPtr makeTuple(const P&... p)
{
	LockGIL LASS_UNUSED(lock);
	TPyObjPtr tuple(PyTuple_New(sizeof...(P)));
	return impl::tupleSetItems(tuple.get(), 0, p...)
		? tuple
		: TPyObjPtr();
}



/** @ingroup Python
 */
inline int decodeTuple(PyObject* obj)
{
	LASS_ASSERT(obj);
	return impl::checkSequenceSize(obj, 0) ? 0 : 1;
}

/** @ingroup Python
 */
template <typename... P>
int decodeTuple(PyObject* obj, P&... p)
{
	LockGIL LASS_UNUSED(lock);
	impl::FastSequence tuple(obj, sizeof...(P));
	if (!tuple)
	{
		return 1;
	}
	return impl::decodeObjects(tuple, 0, p...)
		? 0 
		: 1;
}



/** @ingroup Python
 */
inline int decodeTuple(const TPyObjPtr& obj)
{
	return decodeTuple(obj.get());
}

/** @ingroup Python
 */
template <typename... P>
inline int decodeTuple(const TPyObjPtr& obj, P&... p)
{
	return decodeTuple(obj.get(), p...);
}



/** @ingroup Python
 */
template <typename... P>
int decodeTupleMinimum(PyObject* obj, Py_ssize_t minumumLength, P&... p)
{
	LockGIL LASS_UNUSED(lock);
	impl::FastSequence tuple(obj, minumumLength, sizeof...(P));
	if (!tuple)
	{
		return 1;
	}
	return impl::decodeObjectsMinimum(tuple, 0, p...)
		? 0 
		: 1;
}



/** @ingroup Python
 */
template <typename... P>
int decodeTupleMinimum(const TPyObjPtr& obj, Py_ssize_t minumumLength, P&... p)
{
	return decodeTupleMinimum(obj.get(), minumumLength, p...);
}



// --- pairs ---------------------------------------------------------------------------------------

/** @ingroup Python
 *  std::pair translates to a tuple by copy.
 */
template <typename T1, typename T2>
struct PyExportTraits< std::pair<T1, T2> >
{
	constexpr static const char* py_typing = "tuple[T1, T2]";

	static PyObject* build(const std::pair<T1, T2>& v)
	{
		return fromSharedPtrToNakedCast(makeTuple(v.first, v.second));

	}
	static int get(PyObject* obj, std::pair<T1, T2>& v)
	{
		if (decodeTuple(obj, v.first, v.second) != 0)
		{
			impl::addMessageHeader("pair");
			return 1;
		}
		return 0;
	}
};


/** @ingroup Python
 *  std::tuple translates to a tuple by copy.
 */
template <typename... T>
struct PyExportTraits< std::tuple<T...> >
{
	constexpr static const char* py_typing = "tuple[T...]";

	static PyObject* build(const std::tuple<T...>& v)
	{
		return doBuild(v, std::index_sequence_for<T...>());
	}
	static int get(PyObject* obj, std::tuple<T...>& v)
	{
		return doGet(obj, v, std::index_sequence_for<T...>());
	}

private:
	template <std::size_t... I>
	static PyObject* doBuild(const std::tuple<T...>& v, std::index_sequence<I...>)
	{
		return fromSharedPtrToNakedCast(makeTuple(std::get<I>(v)...));
	}
	template <std::size_t... I>
	static int doGet(PyObject* obj, std::tuple<T...>& v, std::index_sequence<I...>)
	{
		if (decodeTuple(obj, std::get<I>(v)...) != 0)
		{
			impl::addMessageHeader("tuple");
			return 1;
		}
		return 0;
	}
};

}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif

// EOF
