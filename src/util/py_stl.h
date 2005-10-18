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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PY_STL_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PY_STL_H

#include <complex>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <deque>
#include "pyobject_plus.h"
#include "pymap.h"
#include "pysequence.h"

namespace lass
{
	namespace python
	{

		/** @ingroup Python
			*/
		inline PyObject* pyBuildSimpleObject( const std::string& iV )
		{
			return PyString_FromString(iV.c_str());
		}

		/** @ingroup Python
		 *  build a copy of a std::complex as a Python complex number
		 *  @note you build a COPY of the std::complex, not a reference to it!
		 */
		template<class C>
		PyObject* pyBuildSimpleObject( const std::complex<C>& iV )
		{
			PyObject* newComplex = PyComplex_FromDoubles(iV.real(),iV.imag());
			return newComplex;
		}

		/** @ingroup Python
		 *  build a copy of a std::pair as a Python tuple
		 *  @note you build a COPY of the std::pair, not a reference to it!
		 */
		template<class C1, class C2>
		PyObject* pyBuildSimpleObject( const std::pair<C1, C2>& iV )
		{
			PyObject* newTuple = PyTuple_New(2);
			PyTuple_SetItem( newTuple, 0, pyBuildSimpleObject( iV.first ) );
			PyTuple_SetItem( newTuple, 1, pyBuildSimpleObject( iV.second ) );
			return newTuple;
		}


		/** @ingroup Python
			*/
		inline int pyGetSimpleObject( PyObject* iValue, std::string& oV )
		{
			if (!PyString_Check(iValue))
			{
				PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_ARG_NOT_STRING);
				return 1;
			}
			oV = std::string( PyString_AS_STRING( iValue ) );
			return 0;
		}

		/** @ingroup Python
		 *  get a copy of a Python complex number as a std::complex.
		 *  @note you get a COPY of the complex number, not the original number itself!
		 */
		template<class C>
		int pyGetSimpleObject( PyObject* iValue, std::complex<C>& oV )
		{
			if (!PyComplex_Check( iValue ))
			{
				impl::addMessageHeader("complex");
				return 1;
			}
			const C re = static_cast<C>( PyComplex_RealAsDouble( iValue ) );
			const C im = static_cast<C>( PyComplex_ImagAsDouble( iValue ) );
			oV = std::complex<C>(re, im);
			return 0;
		}

		/** @ingroup Python
		 *  get a copy of a Python sequence of two elements as a std::pair.
		 *  @note you get a COPY of the sequence, not the original sequence itself!
		 */
		template<class C1, class C2>
		int pyGetSimpleObject( PyObject* iValue, std::pair<C1, C2>& oV )
		{
			std::pair<C1, C2> result;
			if (!impl::checkSequenceSize(iValue, 2))
			{
				impl::addMessageHeader("pair");
				return 1;
			}
			if (pyGetSimpleObject( PySequence_Fast_GET_ITEM(iValue,0), result.first ) != 0)
			{
				impl::addMessageHeader("pair: first");
				return 1;
			}
			if (pyGetSimpleObject( PySequence_Fast_GET_ITEM(iValue,1), result.second ) != 0)
			{
				impl::addMessageHeader("pair: second");
				return 1;
			}
			oV = result;
			return 0;
		}

		/** @ingroup Python
		 *  get a copy of a Python Dictionary as a std::map.
		 *  @note you get a COPY of the dictionary, not the original dictionary itself!
		 */
		template<class K, class D, typename P, typename A>
		int pyGetSimpleObject( PyObject* iValue, std::map<K, D, P, A>& oV )
		{
			typedef std::map<K, D, P, A> TMap;
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
