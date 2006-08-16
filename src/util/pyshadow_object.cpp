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

// normally, we would #include "pyshadow_object.h" here.  But!  that one needs pyobject_plus.h which 
// includes pyobject_call.inl which needs pyshadow_object.h.  But at that moment pyshadow_object.h
// is being included itself, so pyobject_call.inl doesn't get what it need at the right time.
//
// Instead, we include poobject_plus.h directly, so everything gets defined in the right order.
//
// [Bramz]
//
#include "pyobject_plus.h" 

namespace lass
{
namespace python
{
namespace impl
{

PyShadowBaseCommon::PyShadowBaseCommon(PyTypeObject* iType)
{
	this->ob_type = iType;
}

PyShadowBaseCommon::PyShadowBaseCommon(const PyShadowBaseCommon& iOther)
//: PyObjectPlus(iOther.ob_type)
{
	this->ob_type = iOther.ob_type;
}

PyShadowBaseCommon::~PyShadowBaseCommon()
{
}

PyShadowBaseCommon& PyShadowBaseCommon::operator=(const PyShadowBaseCommon&)
{
	PyObjectPlus::operator=(*this);
	return *this;
}

}

}

}

// EOF
