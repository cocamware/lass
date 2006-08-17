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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_UTIL_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_UTIL_H

namespace lass
{
	namespace python
	{
		namespace impl
		{
			
			inline bool isOfType(PyObject* iObject, PyTypeObject* iTypeObject)
			{
				return (iObject->ob_type == iTypeObject);
			}

			template<typename ForwardIterator>
			PyObjectPtr<PyObject>::Type pyBuildTuple(ForwardIterator iB, ForwardIterator iE )
			{
				const int size = static_cast<int>(std::distance(iB,iE));
				LASS_ASSERT(size >= 0);
				PyObjectPtr<PyObject>::Type r(PyTuple_New(size));
				if (r)
				{
					for (int i=0;iB!=iE;++iB,++i)
					{
						PyTuple_SET_ITEM(r.get(),i,pyBuildSimpleObject(*iB));
					}
				}
				return r;
			}

			template<typename ForwardIterator>
			PyObjectPtr<PyObject>::Type pyBuildList(ForwardIterator iB, ForwardIterator iE )
			{
				const int size = static_cast<int>(std::distance(iB,iE));
				LASS_ASSERT(size >= 0);
				PyObjectPtr<PyObject>::Type r(PyList_New(size));
				if (r)
				{
					for (int i=0;iB!=iE;++iB,++i)
					{
						PyList_SET_ITEM(r.get(),i,pyBuildSimpleObject(*iB));
					}
				}
				return r;
			}

			template<typename InputIterator>
			PyObjectPtr<PyObject>::Type pyBuildMap(InputIterator iB, InputIterator iE )
			{
				PyObjectPtr<PyObject>::Type r = PyDict_New();
				if (r)
				{
					for (int i=0;iB!=iE;++iB,++i)
					{
						if (PyDict_SetItem( r.get(), pyBuildSimpleObject(iB->first), pyBuildSimpleObject(iB->second)))
						{
							// failed
							PyDict_Clear(r.get());	// should we clean up more than this?!
							return 0;
						}
					}
				}
				return r;
			}
		}
	}
}

#endif
