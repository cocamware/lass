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



#include "python_common.h"
#include "pyobject_plus.h"
#include "py_tuple.h"
#include "pyiteratorrange.h"
#include <iostream>

namespace lass
{
namespace python
{

impl::ClassDefinition PyObjectPlus::_lassPyClassDef(
	"PyObjectPlus", 0, sizeof(PyObjectPlus), 0, 0, &PyObjectPlus::_lassPyClassRegisterHook);

/** This function can be used to execute some code at injection time.
 *  By default it doens't do anything, but you can override it for
 *  any type.  Be aware that the top level implementation is the
 *  only one that counts.  Consider it as a 'virtual' function at
 *  static level
 */
void PyObjectPlus::_lassPyClassRegisterHook()
{
}

PyObjectPlus::PyObjectPlus()
{
	// initializing the type to NULL, when the object is exported to python the type is fixed
	this->ob_type = NULL;
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
	dict_ = PyDict_New();
#endif
	_Py_NewReference( this );
};

PyObjectPlus::~PyObjectPlus()
{
	if (this->ob_refcnt>0)
	{
		if (this->ob_refcnt>1)
		{
			std::cerr << "[LASS RUN MSG] DANGLING REFERENCE to lass::python::PyObjectPlus" 
				<< std::endl;
		}
		--this->ob_refcnt;
		_Py_ForgetReference( this );

	}
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
	//if (dict_)
	//	Py_XDECREF(dict_);
#endif
	LASS_ASSERT(this->ob_refcnt==0);
};

PyObjectPlus::PyObjectPlus(const PyObjectPlus& other)
{
	this->ob_type = NULL;
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
	// [TDM] or just copy the reference? 
	dict_ = PyDict_Copy(iOther.dict_);
#endif
	_Py_NewReference( this );
	impl::forceObjectType(this, other.ob_type);
}

PyObjectPlus& PyObjectPlus::operator =(const PyObjectPlus& LASS_UNUSED(iOther))
{
	LASS_ASSERT(!this->ob_type || this->ob_type == iOther.ob_type);
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
	// [TDM] or just copy the reference? 
	dict_ = PyDict_Copy(iOther.dict_);
#endif
	return *this;
}

// --- impl ----------------------------------------------------------------------------------------


namespace
{

inline Py_ssize_t LASS_CALL fastSequenceSize(PyObject* obj)
{
#ifdef Py_LIMITED_API
	return PyList_Check(obj) ? PyList_Size(obj) : PyTuple_Size(obj);
#else
	return PySequence_Fast_GET_SIZE(obj);
#endif
}

}


namespace impl
{

/** @internal
*/
bool checkSequenceSize(PyObject* obj, Py_ssize_t expectedSize)
{
	LockGIL LASS_UNUSED(lock);
	if (!PySequence_Check(obj))
	{
		PyErr_SetString(PyExc_TypeError, "not a python sequence (tuple, list, ...)");
		return false;
	}
	const Py_ssize_t size = PySequence_Size(obj);
	if (size != expectedSize)
	{
		std::ostringstream buffer;
		buffer << "tuple/list is not of expected size " << expectedSize << " (size is " << size << ")";
		PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
		return false;
	}
	return true;
}

/** @internal
 *  Check that @a obj is a sequence, and return it as a "FAST sequence"
 *  so that you can use PySequence_Fast_GET_ITEM to get its items with borrowed references.
 */
TPyObjPtr checkedFastSequence(PyObject* obj)
{
	LockGIL LASS_UNUSED(lock);
	return TPyObjPtr(PySequence_Fast(obj, "expected a sequence (tuple, list, ...)"));
}

/** @internal
 *  Check that @a obj is a sequence and of expected size, and return it as a "FAST sequence"
 *  so that you can use PySequence_Fast_GET_ITEM to get its items with borrowed references.
 */
TPyObjPtr checkedFastSequence(PyObject* obj, Py_ssize_t expectedSize)
{
	LockGIL LASS_UNUSED(lock);
	TPyObjPtr result = checkedFastSequence(obj);
	if (result)
	{
		const Py_ssize_t size = fastSequenceSize(result.get());
		if (size != expectedSize)
		{
			std::ostringstream buffer;
			buffer << "expected a sequence of size " << expectedSize << " (your size is " << size << ")";
			PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
			result.reset();
		}
	}
	return result;
}

/** @internal
 *  Check that @a obj is a sequence and of expected size, and return it as a "FAST sequence"
 *  so that you can use PySequence_Fast_GET_ITEM to get its items with borrowed references.
 */
TPyObjPtr checkedFastSequence(PyObject* obj, Py_ssize_t minimumSize, Py_ssize_t maximumSize)
{
	LockGIL LASS_UNUSED(lock);
	TPyObjPtr result = checkedFastSequence(obj);
	if (result)
	{
		const Py_ssize_t size = fastSequenceSize(result.get());
		if (size < minimumSize || size > maximumSize)
		{
			std::ostringstream buffer;
			buffer << "expected a sequence of size between " << minimumSize << " and " << maximumSize
				<< " inclusive (your size is " << size << ")";
			PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
			result.reset();
		}
	}
	return result;
}


FastSequence::FastSequence(PyObject* obj, Py_ssize_t size):
	fast_(checkedFastSequence(obj, size))
{
#ifndef Py_LIMITED_API
	objects_ = fast_ ? PySequence_Fast_ITEMS(fast_.get()) : 0;
#endif
	size_ = fast_ ? size : 0; // by construction
}

FastSequence::FastSequence(PyObject* obj, Py_ssize_t minSize, Py_ssize_t maxSize):
	fast_(checkedFastSequence(obj, minSize, maxSize))
{
#ifndef Py_LIMITED_API
	objects_ = fast_ ? PySequence_Fast_ITEMS(fast_.get()) : 0;
#endif
	size_ = fast_ ? fastSequenceSize(fast_.get()) : 0;
}


/** Here, we try to fix some lifetime issues to guarantee some lifetime requirements on self.
 */
PyObject* establishMagicalBackLinks(PyObject* result, PyObject* self)
{
	if (!result)
	{
		return 0;
	}
	if (result->ob_type == PyIteratorRange::_lassPyClassDef.type())
	{
		LockGIL LASS_UNUSED(lock);
		PyIteratorRange* iter = static_cast<PyIteratorRange*>(result);
		if (!iter->owner())
		{
			iter->setOwner(fromNakedToSharedPtrCast<PyObject>(self));
		}
	}
	return result;
}

}
}
}

// EOF
