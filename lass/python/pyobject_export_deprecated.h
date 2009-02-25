/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
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
 *	Copyright (C) 2004-2008 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_EXPORT_DEPRECATED_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_EXPORT_DEPRECATED_H

#include "python_common.h"
#include "shadowee_traits.h"
#include "../util/shared_ptr.h"

namespace lass
{
namespace python
{
	/** @ingroup Python
	*   A traits class that replaces the now deprecated pyBuildSimpleObject functionality.
	*   Reason is to support the two-phase name lookup during template instantiation. (gcc >=3.4 )
	*/
	template <typename T> 
	struct PyExportTraits
	{
		typedef typename ShadoweeTraits<T>::TShadowTraits TShadowTraits;
		static PyObject* build(const T& v)
		{
			const typename TShadowTraits::TCppClassPtr p(new T(v));
			return fromSharedPtrToNakedCast(TShadowTraits::buildObject(p));
		}
		static int get(PyObject* obj, T& v)
		{
			if (obj == Py_None || obj->ob_type != T::_lassPyClassDef.type())
			{
				PyErr_Format(PyExc_TypeError, "PyObject not a %s", T::_lassPyClassDef.name());
				return 1;
			}
			typename TShadowTraits::TConstCppClassPtr p;
			if (TShadowTraits::getObject(obj, p) != 0)
			{
				return 1;
			}
			// try
			v = *p;
			// catch
			return 0;
		}
	};

	/** T may be const
	 */
	template <typename T, template <typename, typename> class S, typename C>
	struct PyExportTraits< util::SharedPtr<T, S, C> >
	{
		typedef typename ShadoweeTraits<T>::TShadowTraits TShadowTraits;
		typedef typename ShadoweeTraits<T>::TPointerTraits::TPtr TPtr;
		static PyObject* build(const TPtr& value)
		{
			return fromSharedPtrToNakedCast(TShadowTraits::buildObject(value));
		}
		static int get(PyObject* obj, TPtr& value)
		{
			if (obj == Py_None)
			{
				value = TPtr();
				return 0;
			}
			return TShadowTraits::getObject(obj, value);
		}
	};

	template <typename T>
	struct PyExportTraits< std::auto_ptr<T> >
	{
		typedef typename ShadoweeTraits<T>::TShadowTraits TShadowTraits;
		typedef typename ShadoweeTraits<T>::TPointerTraits::TPtr TPtr;
		static PyObject* build(std::auto_ptr<T>& iv)
		{
			TPtr p(iv);
			return fromSharedPtrToNakedCast(TShadowTraits::buildObject(p));
		}
	};

	/** @ingroup Python
	 *  @deprecated
	 *  a utility macro to easen the transition to the template based infrastructure
	 */
	#define PYEXPORTTRAITS_USINGDEPRECATED( t_basicType )	\
	template<>\
	struct PyExportTraits< t_basicType >\
	{\
		static PyObject* build(const t_basicType & iv) { return pyBuildSimpleObject_deprecated(iv); }\
		static int get(PyObject* iv, t_basicType & ov) { return pyGetSimpleObject_deprecated(iv,ov); }\
	};\

	/** @ingroup Python
	 *  @deprecated
	 *  a utility macro to easen the transition to the template based infrastructure
	 */
	#define PYEXPORTTRAITS_USINGDEPRECATED_TEMPL( t_basicType )	\
	template< typename T >\
	struct PyExportTraits< t_basicType< T > >\
	{\
		static PyObject* build(const t_basicType< T > & iv) { return pyBuildSimpleObject_deprecated(iv); }\
		static int get(PyObject* iv, t_basicType< T > & ov) { return pyGetSimpleObject_deprecated(iv,ov); }\
	};

	/** @ingroup Python
	 *  @deprecated
	 *  a utility macro to easen the transition to the template based infrastructure
	 */
	#define PYEXPORTTRAITS_USINGDEPRECATED_TEMPL_2( t_basicType )	\
	template< typename T, typename U >\
	struct PyExportTraits< t_basicType< T, U > >\
	{\
		static PyObject* build(const t_basicType< T, U > & iv) { return pyBuildSimpleObject_deprecated(iv); }\
		static int get(PyObject* iv, t_basicType< T, U> & ov) { return pyGetSimpleObject_deprecated(iv,ov); }\
	};

	/** @ingroup Python
	 *  @deprecated
	 *  a utility macro to easen the transition to the template based infrastructure
	 */
	#define PYEXPORTTRAITS_USINGDEPRECATED_TEMPL_3( t_basicType )	\
	template< typename T, typename U, typename V >\
	struct PyExportTraits< t_basicType< T, U, V > >\
	{\
		static PyObject* build(const t_basicType< T, U, V > & iv) { return pyBuildSimpleObject_deprecated(iv); }\
		static int get(PyObject* iv, t_basicType< T, U, V   > & ov) { return pyGetSimpleObject_deprecated(iv,ov); }\
	};

	/** @ingroup Python
	 *  @deprecated
	 *  a utility macro to easen the transition to the template based infrastructure
	 */
	#define PYEXPORTTRAITS_USINGDEPRECATED_TEMPL_4( t_basicType )	\
	template< typename T, typename U, typename V, typename W >\
	struct PyExportTraits< t_basicType< T, U, V, W > >\
	{\
		static PyObject* build(const t_basicType< T, U, V, W > & iv) { return pyBuildSimpleObject_deprecated(iv); }\
		static int get(PyObject* iv, t_basicType< T, U, V, W > & ov) { return pyGetSimpleObject_deprecated(iv,ov); }\
	};
}
}

#endif

