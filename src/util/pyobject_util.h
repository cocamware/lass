/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_UTIL_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_UTIL_H

namespace lass
{
	namespace python
	{
		namespace impl
		{
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
                catch (const std::exception& iError)
				{
				}
				PyErr_Restore(type, value, traceback);
			}

			template <typename Sequence>
			int pyGetSequenceObject( PyObject* iValue, Sequence& oV )
			{
				if (!PyList_Check(iValue) && !PyTuple_Check(iValue))
				{
					PyErr_SetString(PyExc_TypeError, "not a python list/tuple");
					return 1;
				}
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
				return 0;
			}
		}

		template<class C> 
		PyObject* pyBuildSimpleObject( const std::complex<C>& iV )
		{
			PyObject* newComplex = PyComplex_FromDoubles(iV.real(),iV.imag());
			return newComplex;
		}
		template<class C1, class C2> 
		PyObject* pyBuildSimpleObject( const std::pair<C1, C2>& iV )
		{
			PyObject* newTuple = PyTuple_New(2);
			PyTuple_SetItem( newTuple, 0, pyBuildSimpleObject( iV.first ) );
			PyTuple_SetItem( newTuple, 1, pyBuildSimpleObject( iV.second ) );
			return newTuple;
		}
		template<class C> 
		PyObject* pyBuildSimpleObject( const std::vector<C>& iV )
		{
			PyObject* newTuple = PyTuple_New(iV.size());
			int i;
			for (i = 0;i < int(iV.size()); ++i)
				PyTuple_SetItem( newTuple, i, pyBuildSimpleObject( iV[i] ) );
			return newTuple;
		}
		template<class C> 
		PyObject* pyBuildSimpleObject( const std::list<C>& iV )
		{
			PyObject* newTuple = PyTuple_New(iV.size());
			int i;
			typename std::list<C>::const_iterator it = iV.begin();
			typename std::list<C>::const_iterator eit = iV.end();
			for (i=0;it != eit; ++it,++i)
				PyTuple_SetItem( newTuple, i, pyBuildSimpleObject( *it ) );
			return newTuple;
		}
		template<class K, class V> 
		PyObject* pyBuildSimpleObject( const std::map<K,V>& iV )
		{
			PyObject* newDict = PyDict_New();
			int i;
			typename std::map<K,V>::const_iterator it = iV.begin();
			typename std::map<K,V>::const_iterator eit = iV.end();
			for (i=0;it != eit; ++it,++i)
				PyDict_SetItem( newDict, pyBuildSimpleObject( it->first ), pyBuildSimpleObject( it->second ) );
			return newDict;
		}

		template<class C>
		int pyGetSimpleObject( PyObject* iValue, std::complex<C>& oV )
		{
			if (!PyComplex_Check( iValue ))
			{	
				impl::addMessageHeader("complex");
				return 1;
			}
			oV = std::complex(PyComplex_RealAsDouble( iValue ), PyComplex_ImagAsDouble( iValue ));
			return 0;
		}

		template<class C1, class C2>
		int pyGetSimpleObject( PyObject* iValue, std::pair<C1, C2>& oV )
		{
			std::pair<C1, C2> result;
			if (!impl::checkSequenceSize(iValue, 2))
			{
				impl::addMessageHeader("pair");
				return 1;
			}
			if (pyGetSimpleObject( PySequence_GetItem(iValue,0), result.first ) != 0)
			{
				impl::addMessageHeader("pair: first");
				return 1;
			}
			if (pyGetSimpleObject( PySequence_GetItem(iValue,1), result.second ) != 0)
			{
				impl::addMessageHeader("pair: second");
				return 1;
			}
			oV = result;
			return 0;
		}

		template<class C> 
		int pyGetSimpleObject( PyObject* iValue, std::vector<C>& oV )
		{
			return impl::pyGetSequenceObject( iValue, oV );
		}

		template<class C> 
		int pyGetSimpleObject( PyObject* iValue, std::list<C>& oV )
		{
			return impl::pyGetSequenceObject( iValue, oV );
		}

		template<class K,class D> 
		int pyGetSimpleObject( PyObject* iValue, std::map<K,D>& oV )
		{
			typedef std::map<K,D> TMap;
			if (!PyDict_Check(iValue))
			{
				PyErr_SetString(PyExc_TypeError, "not a dict");
				return 1;
			}
			TMap result;

			PyObject *pyKey, *pyData;
			int pos = 0;

			while (PyDict_Next(iValue, &pos, &pyKey, &pyData)) 
			{
				typename TMap::key_type cKey;
				if (pyGetSimpleObject( pyKey , cKey ) != 0)
				{
					impl::addMessageHeader("dict: key");
					return 1;
				}
				D cData;
				if (pyGetSimpleObject( pyData , cData ) != 0)
				{
					impl::addMessageHeader("dict: data");
					return 1;
				}
				result.insert(typename TMap::value_type(cKey, cData));
			}

			oV.swap(result);
			return 0;
		}
	}
}

#endif
