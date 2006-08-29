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
#include "pyiteratorrange.h"
#include "pyobject_macros.h"

namespace lass
{
namespace python
{
	PY_DECLARE_CLASS( PyIteratorRange )
	bool PyIteratorRange::isInitialized = false;

	void PyIteratorRange::initialize()
	{
		if (!isInitialized)
		{
			PyIteratorRange::Type.tp_iternext = (iternextfunc) &PyIteratorRange::PyIteratorRange_IterNext;
			finalizePyType( PyIteratorRange::Type, 
							*PyIteratorRange::GetParentType(), 
							PyIteratorRange::Methods,
							PyIteratorRange::GetSetters, 
							PyIteratorRange::Statics, NULL, NULL );
			LASS_ENFORCE( PyType_Ready( &Type ) >= 0 );
			isInitialized = true;
		}
	}
}

}
