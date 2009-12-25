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
#include "pycallback.h"
#include "pyobject_macros.h"

namespace lass
{
namespace python
{
namespace impl
{

const std::string MultiCallbackImplBase::repr() const
{
	// convert container to python equivalent and ask repr of that.
	return std::string("MultiCallbackRepr");
}

}
	PY_DECLARE_CLASS( MultiCallback )
	PY_CLASS_METHOD( MultiCallback, reset )
	PY_CLASS_PY_METHOD_EX( MultiCallback, callVar, "call", "" )
	PY_CLASS_PY_METHOD_EX( MultiCallback, addVar, "add", "" )

	bool MultiCallback::isInitialized = false;

	MultiCallback::~MultiCallback()
	{
	}

	MultiCallback::MultiCallback(std::auto_ptr<impl::MultiCallbackImplBase> pimpl)
	{
		init(pimpl);
	}

	// wrapping the call ourselves as the macro's and templates have difficulties
	// automatically wrapping everything up
	PyObject * MultiCallback::_tp_call(PyObject * self, PyObject *args, PyObject* kwargs)
	{
		if (!PyType_IsSubtype(self->ob_type , MultiCallback::_lassPyClassDef.type() ))
		{
			PyErr_SetString(PyExc_TypeError,"not castable to MultiCallback");
			return 0;
		}
		if (!_PyArg_NoKeywords("function", kwargs))
		{
			return 0;
		}
		return static_cast<MultiCallback*>(self)->callVar(args);
	}

	void MultiCallback::initializeType()
	{
		if (!isInitialized)
		{
			_lassPyClassDef.type()->tp_call = &MultiCallback::_tp_call;
			_lassPyClassDef.freezeDefinition();
			isInitialized = true;
		}
	}

	void MultiCallback::init(std::auto_ptr<impl::MultiCallbackImplBase> pimpl)
	{
		initializeType();
		impl::fixObjectType(this);
		pimpl_.reset(pimpl);
	}

	std::string MultiCallback::doPyStr(void) 
	{ 
		return doPyRepr(); 
	}
	
	std::string MultiCallback::doPyRepr()
	{
		return pimpl_->repr();
	}
	
	void MultiCallback::reset()
	{
		pimpl_->reset();
	}

	void MultiCallback::call(const python::TPyObjPtr& args)
	{
		pimpl_->call(args,this);
	}
	PyObject* MultiCallback::callVar(PyObject* args) 
	{ 
		call(python::TPyObjPtr(args)); 
		Py_RETURN_NONE;
	}
	const std::type_info& MultiCallback::type() const
	{
		return pimpl_->type();
	}
	void* MultiCallback::raw(bool writable) const
	{
		return pimpl_->raw(writable);
	}

	Py_ssize_t MultiCallback::length( PyObject* self)
	{
		return static_cast<MultiCallback*>(self)->pimpl_->length();
	}
	void MultiCallback::add(const python::TPyObjPtr& args)
	{
		pimpl_->add(args);
	}
	PyObject* MultiCallback::addVar(PyObject* args)
	{
		add(python::TPyObjPtr(args)); 
		Py_RETURN_NONE;
	}

}

}
