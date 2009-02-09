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
 *	Copyright (C) 2004-2009 the Initial Developer.
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



#include "lass_common.h"
#include "pysequence.h"
#include "pyobject_macros.h"

namespace lass
{
namespace python
{
namespace impl
{
	PY_DECLARE_CLASS( PySequence )
	//typedef meta::type_list::Make<PyObject*>::Type TArguments;
	//PY_CLASS_CONSTRUCTOR(PySequence, TArguments) // constructor with some arguments. *

	PY_CLASS_METHOD( PySequence, append )
	PY_CLASS_METHOD( PySequence, pop )
	PY_CLASS_METHOD_NAME( PySequence, popwo, "pop" )
	PY_CLASS_METHOD( PySequence, clear )
	PY_CLASS_METHOD( PySequence, reserve )

	PySequenceMethods PySequence::pySequenceMethods = {
	(lenfunc)PySequence_Length,			/* sq_length */
	(binaryfunc)PySequence_Concat,		/* sq_concat */
	(ssizeargfunc)PySequence_Repeat,		/* sq_repeat */
	(ssizeargfunc)PySequence_Item,			/* sq_item */
	(ssizessizeargfunc)PySequence_Slice,		/* sq_slice */
	(ssizeobjargproc)PySequence_AssItem,		/* sq_ass_item */
	(ssizessizeobjargproc)PySequence_AssSlice,	/* sq_ass_slice */
	(objobjproc)PySequence_Contains,		/* sq_contains */
	(binaryfunc)PySequence_InplaceConcat,	/* sq_inplace_concat */
	(ssizeargfunc)PySequence_InplaceRepeat,	/* sq_inplace_repeat */
	};

	bool PySequence::isInitialized = false;

	/* POSTPONED: parsing of the arguments needs some work
	PySequence::PySequence( PyObject* iP ) : PyObjectPlus(&Type)
	{
		// we will default to the vector implementation
		typedef std::vector<PyObject*>	TVPyObj;
		TVPyObj*	newContainer  = new TVPyObj;
		pyGetSimpleObject(iP, newContainer );
		pimpl_ = new PySequenceContainer<TVPyObj,ContainerOwned<TVPyObj> >(*newContainer,false);
		initialize();
	}
	*/

	PySequence::~PySequence()
	{
		delete pimpl_;
	}
	void PySequence::initialize()
	{
		if (!isInitialized)
		{
			//PySequence::Type.tp_iter = &PySequence::PySequence_ListIter;
			_lassPyClassDef.type_.tp_as_sequence= &pySequenceMethods;
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
			// [TDM] for some reason the dict member is not getting properly initialized on PySequence?!
			// switch off inheritance
			PySequence::_lassPyType.tp_dictoffset = 0;
			PySequence::_lassPyType.tp_flags &= ~Py_TPFLAGS_BASETYPE;
#endif
			_lassPyClassDef.freezeDefinition();
			isInitialized = true;
		}
	}

	Py_ssize_t PySequence::PySequence_Length( PyObject* iPO)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_Length();
	}
	PyObject* PySequence::PySequence_Concat(PyObject *iPO, PyObject *bb)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_Concat(bb);
	}
	PyObject* PySequence::PySequence_Repeat(PyObject *iPO, Py_ssize_t n)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_Repeat(n);
	}
	PyObject* PySequence::PySequence_Item(PyObject*iPO, Py_ssize_t i)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_Item(i);
	}
	PyObject* PySequence::PySequence_Slice(PyObject *iPO, Py_ssize_t ilow, Py_ssize_t ihigh)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_Slice(ilow,ihigh);
	}
	int PySequence::PySequence_AssItem(PyObject *iPO, Py_ssize_t i, PyObject *v)
	{
		return static_cast<PySequence*>(iPO)->pimpl_->PySequence_AssItem(i,v);
	}
	int PySequence::PySequence_AssSlice(PyObject *iPO, Py_ssize_t ilow, Py_ssize_t ihigh, PyObject *v)
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
		if (r)
			return NULL;
		Py_INCREF(iPO);
		return iPO;
	}
	PyObject * PySequence::PySequence_InplaceRepeat(PyObject *iPO, Py_ssize_t n)
	{
		int r = static_cast<PySequence*>(iPO)->pimpl_->PySequence_InplaceRepeat(n);
		if (r)
			return NULL;
		Py_INCREF(iPO);
		return iPO;
	}
}

}

}

