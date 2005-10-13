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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#include "util_common.h"
#include "pysequence.h"
#include "pyobject_util.h"

namespace lass
{
namespace python
{
namespace impl
{
	PY_DECLARE_CLASS( PySequence )
	PY_CLASS_METHOD( PySequence, append )

	PySequenceMethods PySequence::pySequenceMethods = {
	(inquiry)PySequence_Length,			/* sq_length */
	(binaryfunc)PySequence_Concat,		/* sq_concat */
	(intargfunc)PySequence_Repeat,		/* sq_repeat */
	(intargfunc)PySequence_Item,			/* sq_item */
	(intintargfunc)PySequence_Slice,		/* sq_slice */
	(intobjargproc)PySequence_AssItem,		/* sq_ass_item */
	(intintobjargproc)PySequence_AssSlice,	/* sq_ass_slice */
	(objobjproc)PySequence_Contains,		/* sq_contains */
	(binaryfunc)PySequence_InplaceConcat,	/* sq_inplace_concat */
	(intargfunc)PySequence_InplaceRepeat,	/* sq_inplace_repeat */
	};

	bool PySequence::isInitialized = false;

	PySequence::~PySequence()
	{
	}
	void PySequence::initialize()
	{
		if (!isInitialized)
		{
			PySequence::Type.tp_iter = &PySequence::PySequence_ListIter;
			finalizePyType( PySequence::Type, 
							*PySequence::GetParentType(), 
							PySequence::Methods,
							PySequence::GetSetters, 
							PySequence::Statics, NULL, NULL );
			LASS_ENFORCE( PyType_Ready( &Type ) >= 0 );
			isInitialized = true;
		}
	}

	int PySequence::PySequence_Clear(PyObject *iPO)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_Clear();
	}
	int PySequence::PySequence_Length( PyObject* iPO)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_Length();
	}
	PyObject* PySequence::PySequence_Concat(PyObject *iPO, PyObject *bb)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_Concat(bb);
	}
	PyObject* PySequence::PySequence_Repeat(PyObject *iPO, int n)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_Repeat(n);
	}
	PyObject* PySequence::PySequence_Item(PyObject*iPO, int i)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_Item(i);
	}
	PyObject* PySequence::PySequence_Slice(PyObject *iPO, int ilow, int ihigh)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_Slice(ilow,ihigh);
	}
	int PySequence::PySequence_AssItem(PyObject *iPO, int i, PyObject *v)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_AssItem(i,v);
	}
	int PySequence::PySequence_AssSlice(PyObject *iPO, int ilow, int ihigh, PyObject *v)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_AssSlice(ilow,ihigh,v);
	}
	int PySequence::PySequence_Contains(PyObject *iPO, PyObject *el)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_Contains(el);
	}
	PyObject * PySequence::PySequence_InplaceConcat(PyObject *iPO, PyObject *other)
	{
		int r = static_cast<PySequence*>(iPO)->pimpl_->PySequence_InplaceConcat(other);
		if (!r)
			return NULL;
		return iPO;
	}
	PyObject * PySequence::PySequence_InplaceRepeat(PyObject *iPO, int n)
	{
		int r = static_cast<PySequence*>(iPO)->pimpl_->PySequence_InplaceRepeat(n);
		if (!r)
			return NULL;
		return iPO;
	}

	typedef struct {
		PyObject_HEAD
		long it_index;
		PyListObject *it_seq; /* Set to NULL when iterator is exhausted */
	} listiterobject;

	PyTypeObject PyListIter_Type;

	PyObject * PySequence::PySequence_ListIter(PyObject* iPO)
	{
		listiterobject *it;

		if (!PyList_Check(iPO)) {
			PyErr_BadInternalCall();
			return NULL;
		}
		it = PyObject_GC_New(listiterobject, &PyListIter_Type);
		if (it == NULL)
			return NULL;
		it->it_index = 0;
		Py_INCREF(iPO);
		it->it_seq = (PyListObject *)iPO;
		_PyObject_GC_TRACK(it);
		return (PyObject *)it;
	}

}

}

}