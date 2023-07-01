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



#include "python_common.h"
#include "pysequence.h"
#include "pyobject_macros.h"

namespace lass
{
namespace python
{
namespace impl
{
	PY_DECLARE_CLASS( Sequence )
	//typedef meta::type_list::Make<PyObject*>::Type TArguments;
	//PY_CLASS_CONSTRUCTOR(Sequence, TArguments) // constructor with some arguments. *

	PY_CLASS_METHOD( Sequence, append )
	PY_CLASS_METHOD( Sequence, pop )
	PY_CLASS_METHOD_NAME( Sequence, pop_back, "pop" )
	PY_CLASS_METHOD( Sequence, clear )
	PY_CLASS_METHOD( Sequence, reserve )
	PY_CLASS_METHOD( Sequence, copy )
	PY_CLASS_METHOD( Sequence, asList )
	PY_CLASS_METHOD_NAME( Sequence, iter, methods::_iter_ );
	PY_CLASS_METHOD_NAME( Sequence, repr, methods::_repr_ );

	PySequenceMethods Sequence::pySequenceMethods = {
		&Sequence::length,
		&Sequence::concat,
		&Sequence::repeat,
		&Sequence::item,
		0, // was slice
		&Sequence::assItem,
		0, // was assSlice
		&Sequence::contains,
		&Sequence::inplaceConcat,
		&Sequence::inplaceRepeat,
	};

	PyMappingMethods Sequence::pyMappingMethods = {
		&Sequence::length,
		&Sequence::subscript,
		&Sequence::assSubscript,
	};

	bool Sequence::isInitialized = false;

	Sequence::Sequence(TPimpl&& pimpl)
	{
		init(std::move(pimpl));
	}

	void Sequence::init(TPimpl&& pimpl)
	{
		LockGIL LASS_UNUSED(lock);
		initializeType();
		impl::fixObjectType(this);
		pimpl_ = std::move(pimpl);
	}

	void Sequence::initializeType()
	{
		LockGIL LASS_UNUSED(lock);
		if (!isInitialized)
		{
			_lassPyClassDef.type()->tp_as_sequence= &Sequence::pySequenceMethods;
			_lassPyClassDef.type()->tp_as_mapping= &Sequence::pyMappingMethods;
			_lassPyClassDef.freezeDefinition();
			isInitialized = true;
		}
	}

	const TSequencePtr Sequence::copy() const
	{
		LockGIL LASS_UNUSED(lock);
		Sequence::TPimpl pimpl = pimpl_->copy();
		return TSequencePtr(new Sequence(std::move(pimpl)));
	}
	void Sequence::clear()
	{
		LockGIL LASS_UNUSED(lock);
		if (!pimpl_->clear())
		{
			impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}
	}
	void Sequence::reserve(Py_ssize_t n)
	{
		LockGIL LASS_UNUSED(lock);
		if (!pimpl_->reserve(n))
		{
			impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}
	}
	void Sequence::append(const TPyObjPtr& obj)
	{
		LockGIL LASS_UNUSED(lock);
		if (!pimpl_->append(obj))
		{
			impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}
	}
	const TPyObjPtr Sequence::pop(Py_ssize_t i)
	{
		LockGIL LASS_UNUSED(lock);
		TPyObjPtr popped(pimpl_->item(i));
		if (!pimpl_->pop(i))
		{
			impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}
		return popped;
	}
	const TPyObjPtr Sequence::pop_back()
	{
		LockGIL LASS_UNUSED(lock);
		return pop(pimpl_->length() - 1);
	}

	const std::type_info& Sequence::type() const
	{
		return pimpl_->type();
	}
	void* Sequence::raw(bool writable) const
	{
		return pimpl_->raw(writable);
	}

	std::string Sequence::repr() const
	{
		LockGIL LASS_UNUSED(lock);
		return pimpl_->repr();
	}

	const TPyObjPtr Sequence::asList() const
	{
		LockGIL LASS_UNUSED(lock);
		return pimpl_->asNative();
	}
	const TPyObjPtr Sequence::iter() const
	{ 
		LockGIL LASS_UNUSED(lock);
		return fromNakedToSharedPtrCast<PyObject>(pimpl_->items()); 
	}

	Py_ssize_t Sequence::length(PyObject* self)
	{
		return static_cast<Sequence*>(self)->pimpl_->length();
	}
	PyObject* Sequence::concat(PyObject* self, PyObject* other)
	{
		TSequencePtr result = static_cast<Sequence*>(self)->copy();
		return inplaceConcat(result.get(), other);
	}
	PyObject* Sequence::repeat(PyObject* self, Py_ssize_t n)
	{
		TSequencePtr result = static_cast<Sequence*>(self)->copy();
		return inplaceRepeat(result.get(), n);
	}
	PyObject* Sequence::item(PyObject* self, Py_ssize_t i)
	{
		return static_cast<Sequence*>(self)->pimpl_->item(i);
	}
	int Sequence::assItem(PyObject* self, Py_ssize_t i, PyObject* obj)
	{
		return static_cast<Sequence*>(self)->pimpl_->assItem(i, obj);
	}
	int Sequence::contains(PyObject* self, PyObject* obj)
	{
		return static_cast<Sequence*>(self)->pimpl_->contains(obj);
	}
	PyObject* Sequence::inplaceConcat(PyObject* self, PyObject* other)
	{
		if (!static_cast<Sequence*>(self)->pimpl_->inplaceConcat(other))
		{
			return 0;
		}
		Py_INCREF(self);
		return self;
	}
	PyObject* Sequence::inplaceRepeat(PyObject* self, Py_ssize_t n)
	{
		if (!static_cast<Sequence*>(self)->pimpl_->inplaceRepeat(n))
		{
			return 0;
		}
		Py_INCREF(self);
		return self;
	}
	PyObject* Sequence::subscript(PyObject* self, PyObject* key)
	{
		PySequenceImplBase& pimpl = *static_cast<Sequence*>(self)->pimpl_;
		if (PySlice_Check(key))
		{
			Py_ssize_t start, stop, step, slicelength;
			if (PySlice_GetIndicesEx(key, pimpl.length(), &start, &stop, &step, &slicelength) != 0)
			{
				return 0;
			}
			return pimpl.slice(start, stop, step);
		}
		Py_ssize_t i;
		if (pyGetSimpleObject(key, i) != 0) 
		{
			return 0;
		}
		if (i < 0)
		{
			i += pimpl.length();
		}
		return pimpl.item(i);		
	}
	int Sequence::assSubscript( PyObject* self, PyObject* key, PyObject* value)
	{
		PySequenceImplBase& pimpl = *static_cast<Sequence*>(self)->pimpl_;
		if (PySlice_Check(key))
		{
			Py_ssize_t start, stop, step, slicelength;
			if (PySlice_GetIndicesEx(key, pimpl.length(), &start, &stop, &step, &slicelength) != 0)
			{
				return -1;
			}
			return pimpl.assSlice(start, stop, step, value);
		}
		Py_ssize_t i;
		if (pyGetSimpleObject(key, i) != 0) 
		{
			return -1;
		}
		if (i < 0)
		{
			i += pimpl.length();
		}
		return pimpl.assItem(i, value);
	}
}

}

}
