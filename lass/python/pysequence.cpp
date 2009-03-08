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
#include "py_stl.h"

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

	PySequenceMethods Sequence::pySequenceMethods = {
		&Sequence::length,
		&Sequence::concat,
		&Sequence::repeat,
		&Sequence::item,
		&Sequence::slice,
		&Sequence::assItem,
		&Sequence::assSlice,
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

	Sequence::Sequence(std::auto_ptr<PySequenceImplBase> pimpl)
	{
		init(pimpl);
	}

	void Sequence::init(std::auto_ptr<PySequenceImplBase> pimpl)
	{
		initializeType();
		impl::fixObjectType(this);
		pimpl_.reset(pimpl);
	}

	void Sequence::initializeType()
	{
		if (!isInitialized)
		{
			_lassPyClassDef.type_.tp_as_sequence= &Sequence::pySequenceMethods;
			_lassPyClassDef.type_.tp_as_mapping= &Sequence::pyMappingMethods;
			_lassPyClassDef.type_.tp_iter = &Sequence::iter;
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
			// [TDM] for some reason the dict member is not getting properly initialized on Sequence?!
			// switch off inheritance
			Sequence::_lassPyType.tp_dictoffset = 0;
			Sequence::_lassPyType.tp_flags &= ~Py_TPFLAGS_BASETYPE;
#endif
			_lassPyClassDef.freezeDefinition();
			isInitialized = true;
		}
	}

	const TPyObjPtr Sequence::asList() const
	{
		return fromNakedToSharedPtrCast<PyObject>(pimpl_->slice(0, pimpl_->length(), 1));
	}

	const TSequencePtr Sequence::copy() const
	{
		return TSequencePtr(new Sequence(pimpl_->copy()));
	}
	void Sequence::clear()
	{
		if (!pimpl_->clear())
		{
			impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}
	}
	void Sequence::reserve(size_t n)
	{
		if (!pimpl_->reserve(n))
		{
			impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}
	}
	void Sequence::append(const TPyObjPtr& obj)
	{
		if (!pimpl_->append(obj))
		{
			impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}
	}
	const TPyObjPtr Sequence::pop(Py_ssize_t i)
	{
		TPyObjPtr popped(pimpl_->item(i));
		if (!pimpl_->pop(i))
		{
			impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}
		return popped;
	}
	const TPyObjPtr Sequence::pop_back()
	{
		return pop(pimpl_->length() - 1);
	}

	const std::type_info& Sequence::type() const
	{
		return pimpl_->type();
	}
	void* const Sequence::raw(bool writable) const
	{
		return pimpl_->raw(writable);
	}

	std::string Sequence::doPyRepr()
	{
		const TPyObjPtr list = asList();
		const TPyObjPtr repr(PyObject_Repr(list.get()));
		std::string result;
		if (pyGetSimpleObject(repr.get(), result) != 0)
		{
			impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}
		return result;
	}
	std::string Sequence::doPyStr()
	{
		return doPyRepr();
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
	PyObject* Sequence::slice(PyObject* self, Py_ssize_t low, Py_ssize_t high)
	{
		return static_cast<Sequence*>(self)->pimpl_->slice(low, high, 1);
	}
	int Sequence::assItem(PyObject* self, Py_ssize_t i, PyObject* obj)
	{
		return static_cast<Sequence*>(self)->pimpl_->assItem(i, obj);
	}
	int Sequence::assSlice(PyObject* self, Py_ssize_t low, Py_ssize_t high, PyObject* slice)
	{
		return static_cast<Sequence*>(self)->pimpl_->assSlice(low, high, 1, slice);
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
			if (PySlice_GetIndicesEx(reinterpret_cast<PySliceObject*>(key), pimpl.length(), &start, &stop, &step, &slicelength) != 0)
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
			if (PySlice_GetIndicesEx(reinterpret_cast<PySliceObject*>(key), pimpl.length(), &start, &stop, &step, &slicelength) != 0)
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
	PyObject* Sequence::iter( PyObject* self)
	{
		return static_cast<Sequence*>(self)->pimpl_->items();
	}
}

}

}
