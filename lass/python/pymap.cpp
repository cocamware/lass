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
#include "pymap.h"
#include "pyobject_macros.h"

namespace lass
{
namespace python
{
namespace impl
{
	PY_DECLARE_CLASS( Map )
	PY_CLASS_METHOD( Map, keys )
	PY_CLASS_METHOD( Map, values )
	PY_CLASS_METHOD( Map, items )
	PY_CLASS_METHOD_NAME( Map, keys, methods::_iter_ )
	PY_CLASS_METHOD_NAME( Map, repr, methods::_repr_ )
#if PY_MAJOR_VERSION < 3
	PY_CLASS_METHOD_NAME( Map, keys, "iterkeys" )
	PY_CLASS_METHOD_NAME( Map, values, "itervalues" )
	PY_CLASS_METHOD_NAME( Map, items, "iteritems" )
#endif
	PY_CLASS_METHOD( Map, get )
	PY_CLASS_METHOD_NAME( Map, getOrNone, "get" )
	PY_CLASS_METHOD( Map, clear )
	PY_CLASS_METHOD( Map, copy )
	PY_CLASS_METHOD( Map, asDict )

	bool Map::isInitialized = false;

	PyMappingMethods Map::pyMappingMethods = {
		&Map::length,
		&Map::subscript,
		&Map::assSubscript,
	};

	Map::~Map()
	{
	}

	Map::Map(TPimpl& pimpl)
	{
		init(pimpl);
	}

	void Map::initializeType()
	{
		LockGIL LASS_UNUSED(lock);
		if (!isInitialized)
		{
			_lassPyClassDef.type()->tp_as_mapping = &pyMappingMethods;
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
			// [TDM] for some reason the dict member is not getting properly initialized on Map?!
			// switch off inheritance
			//&reinterpret_cast<const volatile char&>((((s *)0)->m))
			Map::_lassPyType.tp_dictoffset = 0;
			Map::_lassPyType.tp_flags &= ~Py_TPFLAGS_BASETYPE;
#endif
			_lassPyClassDef.freezeDefinition();
			isInitialized = true;
		}
	}

	void Map::init(TPimpl& pimpl)
	{
		LockGIL LASS_UNUSED(lock);
		initializeType();
		impl::fixObjectType(this);
#if LASS_HAVE_STD_AUTO_PTR
		pimpl_.reset(pimpl);
#else
		pimpl_.reset(std::move(pimpl));
#endif
	}

	std::string Map::repr() const
	{
		LockGIL LASS_UNUSED(lock);
		return pimpl_->repr();
	}
	
	const TPyObjPtr Map::keys() const
	{ 
		LockGIL LASS_UNUSED(lock);
		return fromNakedToSharedPtrCast<PyObject>(pimpl_->keys()); 
	}
		
	const TPyObjPtr Map::values() const
	{ 
		LockGIL LASS_UNUSED(lock);
		return fromNakedToSharedPtrCast<PyObject>(pimpl_->values()); 
	}
	
	const TPyObjPtr Map::items() const
	{ 
		LockGIL LASS_UNUSED(lock);
		return fromNakedToSharedPtrCast<PyObject>(pimpl_->items()); 
	}

	const TPyObjPtr Map::get(const TPyObjPtr& key, const TPyObjPtr& defaultValue) const
	{
		LockGIL LASS_UNUSED(lock);
		TPyObjPtr result = fromNakedToSharedPtrCast<PyObject>(pimpl_->subscript(key.get()));
		if (!result && PyErr_ExceptionMatches(PyExc_KeyError))
		{
			PyErr_Clear();
			result = defaultValue;
		}
		return result;
	}

	const TPyObjPtr Map::getOrNone(const TPyObjPtr& key) const
	{
		LockGIL LASS_UNUSED(lock);
		return get(key, fromNakedToSharedPtrCast<PyObject>(Py_None));
	}

	const TMapPtr Map::copy() const
	{
		LockGIL LASS_UNUSED(lock);
		TPimpl pimpl = pimpl_->copy();
		return TMapPtr(new Map(pimpl));
	}

	const TPyObjPtr Map::asDict() const
	{
		LockGIL LASS_UNUSED(lock);
		return pimpl_->asNative();
	}

	void Map::clear()
	{
		LockGIL LASS_UNUSED(lock);
		if (!pimpl_->clear())
		{
			impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}
	}

	const std::type_info& Map::type() const
	{
		return pimpl_->type();
	}
	void* Map::raw(bool writable) const
	{
		return pimpl_->raw(writable);
	}

	Py_ssize_t Map::length( PyObject* self)
	{
		return static_cast<Map*>(self)->pimpl_->length();
	}

	PyObject* Map::subscript( PyObject* self, PyObject* key)
	{
		return static_cast<Map*>(self)->pimpl_->subscript(key);
	}

	int Map::assSubscript( PyObject* self, PyObject* key, PyObject* value)
	{
		return static_cast<Map*>(self)->pimpl_->assSubscript(key, value);
	}
}

}

}
