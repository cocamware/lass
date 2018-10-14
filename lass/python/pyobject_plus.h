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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

/** @defgroup Python
 *  @brief interface library to Python
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H

#include "python_common.h"
#include "../meta/is_derived.h"

/** @ingroup Python
 *  Place as first line of your Pythonized class.    
 *
 *  For @a t_parentClass use the C++ class from which you wish the python object inherits.  
 *	@a t_parentClass must also be a Pythonized class or use lass::python::PyObjectPlus as default.
 *
 *   @remark Any declarations coming after this macro are private!
 */
#define PY_HEADER( t_parentClass ) \
	public: \
		typedef t_parentClass _lassPyParentType; \
		static ::lass::python::impl::ClassDefinition _lassPyClassDef; \
		virtual ::lass::python::impl::ClassDefinition* _lassPyGetClassDef() const { return &_lassPyClassDef; } \
	private:

#include "class_definition.h"
#include "export_traits.h"
#include "pyobject_casters.h"

namespace lass
{
	namespace python
	{
		/** @ingroup Python
		 */
		template<typename T>
		PyObject* pyBuildSimpleObject(T& iV)
		{
			LockGIL LASS_UNUSED(lock);
			return PyExportTraits<T>::build(iV);
		}

		/** @ingroup Python
		 */
		template<typename T>
		PyObject* pyBuildSimpleObject(const T& iV)
		{
			LockGIL LASS_UNUSED(lock);
			return PyExportTraits<T>::build(iV);
		}

#if LASS_HAVE_STD_UNIQUE_PTR
		// this is not really supported as it would require moving the argument ...
#else
		/** @ingroup Python
		 */
		template<typename T>
		PyObject* pyBuildSimpleObject(std::auto_ptr<T> iV)
		{
			LockGIL LASS_UNUSED(lock);
			return PyExportTraits< std::auto_ptr<T> >::build(iV);
		}
#endif

		/** @ingroup Python
		 */
		template<typename T>
		int pyGetSimpleObject(PyObject* iV, T& oV)
		{
			LockGIL LASS_UNUSED(lock);
			return PyExportTraits<T>::get(iV,oV);
		}

		/**	Helper to specialise PyExportTraits for enumerations.
		 *	@ingroup Python
		 *
		 *	When you want to export enumeration types, you need to specialize PyExportTraits for it.
		 *	This structure will help you
		 *
		 *	@code
		 *	enum MyEnumeration { meFoo, meBar, meFuz };
		 *	namespace lass
		 *	{
		 *	namespace python
		 *	{
		 *	template <> struct PyExportTraits<MyEnumeration>: public PyExportTraitsEnum<MyEnumeration> {};
		 *	}
		 *	}
		 *	@endcode
		 */
		template <typename EnumType, typename IntegerType = long>
		struct PyExportTraitsEnum
		{
			static PyObject* build(const EnumType iv) 
			{ 
				return pyBuildSimpleObject(static_cast<IntegerType>(iv));
			}
			static int get(PyObject* iv, EnumType& ov) 
			{ 
				IntegerType temp;
				if (pyGetSimpleObject(iv, temp) != 0)
				{
					return 1;
				}
				ov = static_cast<EnumType>(temp);
				return 0;
			}
		};

		/** PyObjectPlus.  Base class for pythonable objects.
		*   @ingroup Python
		*   @author Tom De Muer
		*   @date 2003
		*   Usage:
		*   The usage will be given by providing an example.  See the test case and look
		*   for the files bar.h and bar.cpp.
		*/
		class LASS_PYTHON_DLL PyObjectPlus :
			public PyObject
		{
		public:
			static impl::ClassDefinition _lassPyClassDef;
			static void _lassPyClassRegisterHook();
			virtual impl::ClassDefinition* _lassPyGetClassDef() const { return &_lassPyClassDef; }
			typedef void TCppClass;

			PyObjectPlus(); 
			virtual ~PyObjectPlus();

		protected:
			PyObjectPlus(const PyObjectPlus& other);
			PyObjectPlus& operator=(const PyObjectPlus& other);
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
		public:
			PyObject* dict_;
#endif
		private:
		};

		/** meta function to detect if a type is a PyObject-derived type
		*   @ingroup Python
		*/
		template <typename T> struct IsPyObject: meta::IsDerived<T, PyObject> {};

		namespace impl
		{
			LASS_PYTHON_DLL bool LASS_CALL checkSequenceSize(PyObject* iValue, Py_ssize_t iExpectedSize);
			LASS_PYTHON_DLL TPyObjPtr LASS_CALL checkedFastSequence(PyObject* obj);
			LASS_PYTHON_DLL TPyObjPtr LASS_CALL checkedFastSequence(PyObject* obj, Py_ssize_t expectedSize);
			LASS_PYTHON_DLL TPyObjPtr LASS_CALL checkedFastSequence(PyObject* obj, Py_ssize_t minimumSize, Py_ssize_t maximumSize);
			LASS_PYTHON_DLL PyObject* LASS_CALL establishMagicalBackLinks(PyObject* result, PyObject* self);
		}
	}
}

#include "pyobject_plus.inl"

#endif
