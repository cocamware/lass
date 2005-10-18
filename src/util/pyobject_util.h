/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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

			template<typename InputIterator>
			PyObject* pyBuildList(InputIterator iB, InputIterator iE )
			{
				PyObject* r = PyList_New(std::distance(iB,iE));
				if (r==NULL)
					return NULL;
                for (int i=0;iB!=iE;++iB,++i)
				{
					PyList_SET_ITEM(r,i,pyBuildSimpleObject(*iB));
				}
				return r;
			}


			inline void addMessageHeader(const std::string& iHeader)
			{
				if (!PyErr_Occurred() || !PyErr_ExceptionMatches(PyExc_TypeError))
				{
					return;
				}
				PyObject *type, *value, *traceback;
				PyErr_Fetch(&type, &value, &traceback);
				try
				{
					if (PyString_Check(value))
					{
						std::ostringstream buffer;
						buffer << iHeader << ": " << PyString_AsString(value);
						PyObject* temp = PyString_FromString(buffer.str().c_str());
						std::swap(value, temp);
						Py_DECREF(temp);
					}
				}
				catch (const std::exception&)
				{
				}
				PyErr_Restore(type, value, traceback);
			}

			inline bool checkSequenceSize(PyObject* iValue, int iExpectedSize)
			{
				if (!PyTuple_Check(iValue) && !PyList_Check(iValue))
				{
					PyErr_SetString(PyExc_TypeError, "not a python list/tuple");
					return false;
				}
				const int size = PySequence_Size(iValue);
				if (size != iExpectedSize)
				{
					std::ostringstream buffer;
					buffer << "tuple/list is not of expected size " << iExpectedSize
						<< " (size is " << size << ")";
					PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
					return false;
				}
				return true;
			}

			template <typename Sequence>
			int pyGetSequenceObject( PyObject* iValue, Sequence& oV )
			{
				if (!PySequence_Check(iValue))
				{
					PyErr_SetString(PyExc_TypeError, "not a python sequence");
					return 1;
				}
				// check if we have our own PySequence object, then take a shortcut
				if (isOfType( iValue, &PySequence::Type ) && ((PySequence*)iValue)->pointsToSameContainer(oV))
				{
					return 0;
				}
				else
				{
					Sequence result;
					const int size = PySequence_Length(iValue);
					for (int i = 0; i < size; ++i)
					{
						typename Sequence::value_type temp;
						if (pyGetSimpleObject( PySequence_ITEM(iValue, i) , temp ) != 0)
						{
							impl::addMessageHeader(
								std::string("sequence element ") + util::stringCast<std::string>(i));
							return 1;
						}
						result.push_back( temp );
					}
					oV.swap(result);
				}
				return 0;
			}
		}

		/** @ingroup Python
		*  get value of a python object by the name of the python object rather than the pointer
		*/
		template <typename T>
		int pyGetSimpleObjectByName(const std::string& iName, T& oV)
		{
			PyObject* module = PyImport_AddModule("__main__");
			PyObject* dict = PyModule_GetDict(module);
			PyObject* temp = PyDict_GetItemString(dict, iName.c_str());
			return python::pyGetSimpleObject(temp, oV);
		}
	}
}

#endif
