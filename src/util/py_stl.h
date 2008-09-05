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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PY_STL_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PY_STL_H

#include "pyobject_plus.h"
#include "py_tuple.h"
#include <complex>

namespace lass
{
	namespace python
	{
		/** @ingroup Python
		 *  @internal
		 */
		template <>
		struct PyExportTraits<std::string>
		{
			static PyObject* build( const std::string& iV )
			{
				return PyString_FromString(iV.c_str());
			}
			static int get( PyObject* iValue, std::string& oV )
			{
				if (!PyString_Check(iValue))
				{
					PyErr_SetString(PyExc_TypeError, "not a string");
					return 1;
				}
				oV = std::string( PyString_AS_STRING( iValue ) );
				return 0;
			}
		};



		/** @ingroup Python
		 *  @internal
		 */
		template <typename T>
		struct PyExportTraits< std::complex<T> >
		{
			static PyObject* build( const std::complex<T>& iV )
			{
				PyObject* newComplex = PyComplex_FromDoubles(iV.real(),iV.imag());
				return newComplex;
			}
			static int get( PyObject* iValue, std::complex<T>& oV )
			{
				T re, im;
				if (impl::pyGetFloatObject( iValue, re ) == 0)
				{
					oV = std::complex<T>(re, 0);
					return 0;
				}
				PyErr_Clear();
				if (!PyComplex_Check( iValue ))
				{
					PyErr_SetString(PyExc_TypeError, "not a complex number");
					return 1;
				}
				re = static_cast<T>( PyComplex_RealAsDouble( iValue ) );
				im = static_cast<T>( PyComplex_ImagAsDouble( iValue ) );
				oV = std::complex<T>(re, im);
				return 0;
			}
		};


		/** @ingroup Python
		 *  @internal
		 */
		template <typename T1, typename T2>
		struct PyExportTraits< std::pair<T1, T2> >
		{
			static PyObject* build( const std::pair<T1, T2>& iV )
			{
				return fromSharedPtrToNakedCast(makeTuple(iV.first, iV.second));
			}
			static int get( PyObject* iValue, std::pair<T1, T2>& oV )
			{
				if (decodeTuple(iValue, oV.first, oV.second) != 0)
				{
					impl::addMessageHeader("pair");
					return 1;
				}
				return 0;
			}
		};

}
}
#include "pymap.h"
#include "pysequence.h"

#endif
