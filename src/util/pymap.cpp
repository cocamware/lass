/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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
		(inquiry)PyMap::PyMap_Length, /*mp_length*/
		(binaryfunc)PyMap::PyMap_Subscript, /*mp_subscript*/
		(objobjargproc)PyMap::PyMap_AssSubscript, /*mp_ass_subscript*/
	};

	void PyMap::initialize()
	{
		if (!isInitialized)
		{
			PyMap::_lassPyType.tp_as_mapping = &pyMappingMethods;
			PyMap::_lassPyType.tp_iter = (getiterfunc) &PyMap::PyMap_Iter;
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

	int PyMap::PyMap_Length( PyObject* iPO)
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
