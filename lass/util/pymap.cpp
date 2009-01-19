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



#include "lass_common.h"
#include "pymap.h"
#include "pyobject_macros.h"

namespace lass
{
namespace python
{
namespace impl
{
	PY_DECLARE_CLASS( PyMap )
	PY_CLASS_METHOD( PyMap, keys )
	PY_CLASS_METHOD( PyMap, values )

	bool PyMap::isInitialized = false;

	PyMappingMethods PyMap::pyMappingMethods = {
		(lenfunc)PyMap::PyMap_Length, /*mp_length*/
		(binaryfunc)PyMap::PyMap_Subscript, /*mp_subscript*/
		(objobjargproc)PyMap::PyMap_AssSubscript, /*mp_ass_subscript*/
	};

	void PyMap::initialize()
	{
		if (!isInitialized)
		{
			PyMap::_lassPyType.tp_as_mapping = &pyMappingMethods;
			PyMap::_lassPyType.tp_iter = (getiterfunc) &PyMap::PyMap_Iter;
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
			// [TDM] for some reason the dict member is not getting properly initialized on PyMap?!
			// switch off inheritance
			//&reinterpret_cast<const volatile char&>((((s *)0)->m))
			PyMap::_lassPyType.tp_dictoffset = 0;
			PyMap::_lassPyType.tp_flags &= ~Py_TPFLAGS_BASETYPE;
#endif
			finalizePyType( PyMap::_lassPyType, *PyMap::_lassPyGetParentType(), PyMap::_lassPyMethods, 
				PyMap::_lassPyGetSetters, PyMap::_lassPyStatics, NULL, NULL);
			LASS_ENFORCE( PyType_Ready( &_lassPyType ) >= 0 );
			isInitialized = true;
		}
	}


	PyMap::PyMap() 
	{
		LASS_THROW( "PyMap needs a std::map to initialize" );
	}

	PyMap::~PyMap()
	{
		delete pimpl_;
	}
	

#pragma LASS_TODO("check the IPO for a PyMap type")

	Py_ssize_t PyMap::PyMap_Length( PyObject* iPO)
	{
		return static_cast<PyMap*>(iPO)->pimpl_->PyMap_Length();
	}

	PyObject* PyMap::PyMap_Subscript( PyObject* iPO, PyObject* iKey)
	{
		return static_cast<PyMap*>(iPO)->pimpl_->PyMap_Subscript(iKey);
	}

	int PyMap::PyMap_AssSubscript( PyObject* iPO, PyObject* iKey, PyObject* iValue)
	{
		return static_cast<PyMap*>(iPO)->pimpl_->PyMap_AssSubscript(iKey, iValue);
	}


	PyObject* PyMap::PyMap_Iter( PyObject* iPO)
	{
		return static_cast<PyMap*>(iPO)->pimpl_->PyMap_Iter();
	}


}

}

}
