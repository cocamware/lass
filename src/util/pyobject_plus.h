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

/** @defgroup Python
 *  @brief interface library to Python
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H

#include "util_common.h"

#if defined(_DEBUG) && !defined(LASS_PYTHON_HAS_DEBUG_BUILD)
#	undef _DEBUG
#	include "Python.h"
#	define _DEBUG
#else
#	include "Python.h"
#endif

#include "../meta/bool.h"
#include "../meta/is_derived_type.h"
#include "../meta/type_traits.h"
#include "../meta/type_2_type.h"
#include "../meta/int_2_type.h"
#include "../num/num_traits.h"
#include "shared_ptr.h"
#include "string_cast.h"
#include "pyobject_util.h"
#include "pyobject_macros.h"
#include "call_traits.h"
#include <cstdlib>

typedef PyTypeObject * PyParentObject;// Define the PyParent Object


namespace lass
{
	namespace python
	{

		//inline PyMethodDef createPyMethodDef(  char *ml_name, PyCFunction  ml_meth, int ml_flags, char  *ml_doc );
		//inline PyGetSetDef createPyGetSetDef ( char* name, getter get, setter set = NULL, char* doc = NULL, void* closure = NULL );

		/** PyObjectPlus.  Base class for pythonable objects.
		*   @ingroup Python
		*   @author Tom De Muer
		*   @date 2003
		*   Usage:
		*   The usage will be given by providing an example.  See the test case and look
		*   for the files bar.h and bar.cpp.
		*/
		class LASS_DLL PyObjectPlus :
			public PyObject
		{
			PY_HEADER_INTERNAL;
		public:
			typedef PyObjectPlus    TSelf;

			PyObjectPlus(PyTypeObject *T); 
			virtual ~PyObjectPlus();

			PyObjectPlus* PyPlus_INCREF(void);// incref method
			PyObjectPlus* PyPlus_DECREF(void);// decref method

			static void __dealloc(PyObject *P);

			static  PyObject*   __repr(PyObject *PyObj);
			virtual std::string pyRepr(void) { return std::string(ob_type->tp_name) + " object at " + util::stringCast<std::string>(this); }

			static  PyObject*   __str(PyObject *PyObj);
			virtual std::string pyStr(void) { return std::string(ob_type->tp_name) + " object string at " + util::stringCast<std::string>(this); }

		protected:

			PyObjectPlus(const PyObjectPlus& iOther);
			PyObjectPlus& operator=(const PyObjectPlus& iOther);
		};

		/** @class PyObjectStorage
		*  @ingroup Python
		*  @brief Recommended storage policy for single PyObject objects, implementation of StoragePolicy concept
		*  @author Tom De Muer [TDM]
		*  @see ObjectStorage
		*/
		template <typename T, typename Cascade = meta::EmptyType>
		class PyObjectStorage: public Cascade
		{
		public:

			typedef PyObjectStorage<T, Cascade> TSelf;
			typedef T* TStorage;
			typedef T* TPointer;
			typedef T& TReference;

			TStorage& storage() { return storage_; }
			const TStorage& storage() const { return storage_; }

		protected:

			PyObjectStorage(): Cascade(), storage_(defaultStorage()) {}
			PyObjectStorage(T* iPointee): Cascade(), storage_(iPointee) {}
			TPointer pointer() const { return storage_; }
			void dispose() { storage_ = 0; }
			bool isNull() const { return !storage_; }
			void swap(TSelf& iOther) { std::swap(storage_, iOther.storage_);   }
			static TStorage defaultStorage() { return 0; }
		private:
			TStorage storage_;
		};


		/** @class PyObjectCounter
		*  @ingroup Python
		*  @brief The recommended counter for the pyobject pointers, implementation of CounterPolicy concept.
		*  @author Tom De Muer [TDM]
		*  @see DefaultCounter
		*/
		class PyObjectCounter
		{
		public:
			typedef int TCount;
		protected:
			PyObjectCounter() {}
			template <typename TStorage> void init(TStorage& iPointee) {}
			template <typename TStorage> void dispose(TStorage& iPointee) {}
			template <typename TStorage> void increment(TStorage& iPointee)
			{
				Py_INCREF(iPointee);
			}
			template <typename TStorage> bool decrement(TStorage& iPointee)
			{
				bool r = iPointee->ob_refcnt <=1;
				Py_DECREF(iPointee);
				return r;
			}
			template <typename TStorage> TCount count(TStorage& iPointee) const
			{
				LASS_ASSERT(iPointee);
				return iPointee->ob_refcnt;
			}
			void swap(PyObjectCounter& iOther) {}
		private:
			//TCount counterToKeepCompilerFromDoingStupidThings_;
		};

		/** templated "typedef" to a python shared pointer
		 *  @ingroup Python
		 */
		template<class T>
		struct PyObjectPtr
		{
			typedef util::SharedPtr<T, PyObjectStorage, PyObjectCounter> Type;
		};

		template<class T>   T*  PyPlus_INCREF(T* iObj)  { return (T*)(iObj->PyPlus_INCREF()); }
		template<class T>   T*  PyPlus_DECREF(T* iObj)  { return (T*)(iObj->PyPlus_DECREF()); }

		/** fromPySharedPtrCast.
		*   @ingroup Python
		*   Helper function casting a PyObject coming from the Python interface to a SharedPtr
		*   object for use in C++.  Reference counts are taken care of.
		*/
		template<class T>
		lass::util::SharedPtr<T, PyObjectStorage, PyObjectCounter>
		fromPySharedPtrCast(PyObject* iObj)
		{
			// TODO: think of a way to assert this without breaking for T == PyObject [Bramz]
			// LASS_ASSERT( PyType_IsSubtype(iObj->ob_type, &T::Type) );
			Py_INCREF(iObj);
			return lass::util::SharedPtr<T,PyObjectStorage,PyObjectCounter>( static_cast<T*>(iObj) );
		}

		/** fromPySharedPtrCast.
		*   @ingroup Python
		*   Helper function casting a PyObject coming from the Python interface to a SharedPtr
		*   object for use in C++.  Reference counts are taken care of.
		*/
		template<> inline
		lass::util::SharedPtr<PyObject, PyObjectStorage, PyObjectCounter>
		fromPySharedPtrCast(PyObject* iObj)
		{
			Py_INCREF(iObj);
			return lass::util::SharedPtr<PyObject,PyObjectStorage,PyObjectCounter>( static_cast<PyObject*>(iObj) );
		}

		/** toPySharedPtrCast.
		*   @ingroup Python
		*   Helper function casting an object used in C++ for use in Python.  The key operation
		*   done here is to take care of the reference counting. Failing to use this function may
		*   yield unexpected reference count.
		*/
		template<class T>
		PyObject*
		toPySharedPtrCast(T& iObj)
		{
			if (iObj.get())
				return iObj.get()->PyPlus_INCREF();
			else
			{
				Py_INCREF( Py_None );
				return Py_None;
			}
		}

		/** meta function to detect if a type is a PyObject-derived type
		*   @ingroup Python
		*/
		template <typename T>
		struct IsPyObject
		{
			enum { value = meta::IsDerivedType<T, PyObject>::value };
			typedef typename meta::Bool<value>::Type Type;
		};

		/* conversion from PyObject* to given types, a check should be performed
		*  wether the conversion is possible, if not a returnvalue of 1 should be used
		*/
		inline int pyGetSimpleObject( PyObject* iValue, bool& oV );
		inline int pyGetSimpleObject( PyObject* iValue, signed char& oV );
		inline int pyGetSimpleObject( PyObject* iValue, unsigned char& oV );
		inline int pyGetSimpleObject( PyObject* iValue, signed short& oV );
		inline int pyGetSimpleObject( PyObject* iValue, unsigned short& oV );
		inline int pyGetSimpleObject( PyObject* iValue, signed int& oV );
		inline int pyGetSimpleObject( PyObject* iValue, unsigned int& oV );
		inline int pyGetSimpleObject( PyObject* iValue, signed long& oV );
		inline int pyGetSimpleObject( PyObject* iValue, unsigned long& oV );
		inline int pyGetSimpleObject( PyObject* iValue, float& oV );
		inline int pyGetSimpleObject( PyObject* iValue, double& oV );
		inline int pyGetSimpleObject( PyObject* iValue, long double& oV );
		inline int pyGetSimpleObject( PyObject* iValue, std::string& oV );
		inline int pyGetSimpleObject( PyObject* iValue, PyObject*& oV );
		template <typename T> int pyGetSimpleObject( PyObject* iValue, T*& oV );

		inline PyObject* pyBuildSimpleObject( bool iV );
		inline PyObject* pyBuildSimpleObject( signed char iV );
		inline PyObject* pyBuildSimpleObject( unsigned char iV );
		inline PyObject* pyBuildSimpleObject( signed short iV );
		inline PyObject* pyBuildSimpleObject( unsigned short iV );
		inline PyObject* pyBuildSimpleObject( signed int iV );
		inline PyObject* pyBuildSimpleObject( unsigned int iV );
		inline PyObject* pyBuildSimpleObject( signed long iV );
		inline PyObject* pyBuildSimpleObject( unsigned long iV );
		inline PyObject* pyBuildSimpleObject( float iV );
		inline PyObject* pyBuildSimpleObject( double iV );
		inline PyObject* pyBuildSimpleObject( long double iV );
		inline PyObject* pyBuildSimpleObject( const char* iV );
		inline PyObject* pyBuildSimpleObject( const std::string& iV );
		inline PyObject* pyBuildSimpleObject( PyObject* iV );

		template <typename T> int pyGetSimpleObjectByName(const std::string& iName, T& oV);

		namespace impl
		{
			/** @internal
			*/
			struct StaticMember
			{
				PyObject* object;
				PyTypeObject* parentType;
				std::vector<PyMethodDef>* methods;
				std::vector<PyGetSetDef>* getSetters;
				const std::vector<StaticMember>* statics;
				const char* name;
				const char* doc;
			};

			/**	@internal
			*	predicate to check of a python method has the correct name.
			*/
			class LASS_DLL PyMethodEqual
			{
			public:
				PyMethodEqual( const char* iName );
				bool operator()(const PyMethodDef& iMethod) const;
			private:
				const char* name_;
			};

			LASS_DLL PyMethodDef createPyMethodDef( char *ml_name, PyCFunction ml_meth, int ml_flags, char *ml_doc );
			LASS_DLL PyGetSetDef createPyGetSetDef( char* name, getter get, setter set, char* doc, void* closure );

			LASS_DLL void injectStaticMembers(PyTypeObject& iPyType, const std::vector<StaticMember>& iStatics);
			LASS_DLL void finalizePyType(PyTypeObject& iPyType, PyTypeObject& iPyParentType, 
				std::vector<PyMethodDef>& iMethods, std::vector<PyGetSetDef>& iGetSetters, 
				const std::vector<StaticMember>& iStatics, const char* iModuleName, const char* iDocumentation);
			LASS_DLL void addModuleFunction(std::vector<PyMethodDef>& ioModuleMethods, char* iMethodName, char* iDocumentation,
				PyCFunction iMethodDispatcher, PyCFunction& oOverloadChain);

			template <typename CppClass> void injectClassInModule(PyObject* iModule, const char* iClassDocumentation);
			template <typename CppClass> void addClassMethod(char* iMethodName, char* iDocumentation, 
				PyCFunction iMethodDispatcher, PyCFunction& oOverloadChain);
			template <typename CppClass, typename T> void addClassStaticConst(const char* iName, const T& iValue);
			template <typename InnerCppClass> void addClassInnerClass(std::vector<StaticMember>& oOuterStatics, 
                const char* iInnerClassName, const char* iDocumentation);

			template <typename In, typename Out> int pyNumericCast( In iIn, Out& oV );
			template <typename Integer> int pyGetSignedObject( PyObject* iValue, Integer& oV );
			template <typename Integer> int pyGetUnsignedObject( PyObject* iValue, Integer& oV );
			template <typename Float> int pyGetFloatObject( PyObject* iValue, Float& oV );

		}

	}
}

#include "pyobject_plus.inl"
#include "pyshadow_object.h"
#include "pyobject_call.inl"
#include "callback_python.h"
#include "../prim/pyobject_util.h"
#include "py_stl.h"

#endif
