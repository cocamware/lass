/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/
#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_UTIL_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_UTIL_H
namespace lass
{
	namespace python
	{
		template<class C> 
		PyObject* pyBuildSimpleObject( const std::complex<C>& iV )
		{
			PyObject* newTuple = PyTuple_New(2);
			PyTuple_SetItem( newTuple, 0, pyBuildSimpleObject( iV.real() ) );
			PyTuple_SetItem( newTuple, 1, pyBuildSimpleObject( iV.imag() ) );
			return newTuple;
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
			PyObject* newArray = PyList_New(iV.size());
			int i;
			for (i = 0;i < int(iV.size()); ++i)
				PyList_SetItem( newArray, i, pyBuildSimpleObject( iV.at(i) ) );
			return newArray;
		}
		template<class C> 
		PyObject* pyBuildSimpleObject( const std::list<C>& iV )
		{
			PyObject* newArray = PyList_New(iV.size());
			int i;
			std::list<C>::const_iterator it = iV.begin();
			std::list<C>::const_iterator eit = iV.end();
			for (i=0;it != eit; ++it,++i)
				PyList_SetItem( newArray, i, pyBuildSimpleObject( *it ) );
			return newArray;
		}
		template<class K, class V> 
		PyObject* pyBuildSimpleObject( const std::map<K,V>& iV )
		{
			PyObject* newDict = PyDict_New();
			int i;
			std::map<K,V>::const_iterator it = iV.begin();
			std::map<K,V>::const_iterator eit = iV.end();
			for (i=0;it != eit; ++it,++i)
				PyDict_SetItem( newDict, pyBuildSimpleObject( it->first ), pyBuildSimpleObject( it->second ) );
			return newDict;
		}
		template<class C>
		int pyGetSimpleObject( PyObject* iValue, std::complex<C>& oV )
		{
			C	r,i;
			int error = !PyTuple_Check(iValue);
			if (!error)
			{
				error = pyGetSimpleObject( PyTuple_GetItem(iValue,0), r ) ||
						pyGetSimpleObject( PyTuple_GetItem(iValue,1), i );
			}
			else
			{
				PyErr_BadArgument();
				return error;
			}
			oV = std::complex<C>( r, i );
			return error;
		}
		template<class C1, class C2>
		int pyGetSimpleObject( PyObject* iValue, std::pair<C1, C2>& oV )
		{
			std::pair<C1, C2> result;
			int error = !PyTuple_Check(iValue);
			if (!error)
			{
				error = pyGetSimpleObject( PyTuple_GetItem(iValue,0), result.first ) ||
						pyGetSimpleObject( PyTuple_GetItem(iValue,1), result.second );
			}
			else
			{
				PyErr_BadArgument();
				return error;
			}
			oV = result;
			return error;
		}
		template<class C> 
		int pyGetSimpleObject( PyObject* iValue, std::vector<C>& oV )
		{
			oV.clear();
			int i;
			int r;
			int size = PyList_Size( iValue );
			oV.reserve( size );
			for (i = 0;i < size; ++i)
			{
				C temp;
				r = pyGetSimpleObject( PyList_GetItem(iValue,i) , temp );
				if (r!=0)
				{
					PyErr_BadArgument();
					return r;
				}
				oV.push_back( temp );
			}
			return 0;
		}
		template<class C> 
		int pyGetSimpleObject( PyObject* iValue, std::list<C>& oV )
		{
			oV.clear();
			int i;
			int r;
			int size = PyList_Size( iValue );
			for (i = 0;i < size; ++i)
			{
				C temp;
				r = pyGetSimpleObject( PyList_GetItem(iValue,i) , temp );
				if (r!=0)
				{
					PyErr_BadArgument();
					return r;
				}
				oV.push_back( temp );
			}
			return 0;
		}
		template<class K,class V> 
		int pyGetSimpleObject( PyObject* iValue, std::map<K,V>& oV )
		{
			oV.clear();
			int i;
			int r;
			
			PyObject *key, *value;
			int pos = 0;

			while (PyDict_Next(iValue, &pos, &key, &value)) 
			{
				K tempKey;
				V tempValue;
				r = pyGetSimpleObject( key , tempKey );
				if (r)
					r = pyGetSimpleObject( value, tempValue );
				if (r)
					oV.insert( std::map<K,V>::value_type( tempKey, tempValue ) );
				else
				{
					PyErr_BadArgument();
					return r;
				}
			}
			return 0;
		}
	}
}

#endif