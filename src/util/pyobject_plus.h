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

#ifndef PySequence_ITEM
#	define PySequence_ITEM(o, i) PySequence_GetItem(o, i)
#endif

#include "../meta/bool.h"
#include "../meta/is_derived_type.h"
#include "shared_ptr.h"
#include "string_cast.h"
#include "thread.h"
#include <cstdlib>

/** @internal
 *  This macro is only used in the PyObjectPlus class and is for internal LASS
 *  use.  Do not use in custom objects... DON'T!
 */
#define PY_HEADER_INTERNAL \
	public: \
		static PyTypeObject   Type; \
		static ::std::vector<PyMethodDef>    Methods; \
		static ::std::vector<PyGetSetDef>    GetSetters; \
		virtual PyTypeObject *GetType(void) const {return &Type;};\

/** @ingroup Python
 *  Place as first line of your Pythonized class.    
 *
 *  For @a t_parentClass use the C++ class from which you wish the python object inherits.  
 *	@a t_parentClass must also be a Pythonized class or use lass::python::PyObjectPlus as default.
 *
 *   @remark Any declarations coming after this macro are public!
 */
#define PY_HEADER( t_parentClass ) \
	public: \
		typedef t_parentClass TPyParent;\
		static PyTypeObject   Type; \
		static ::std::vector<PyMethodDef>    Methods; \
		static ::std::vector<PyGetSetDef>    GetSetters; \
		static ::std::vector< ::lass::python::impl::StaticMember >	Statics; \
		static PyTypeObject* GetParentType(void)\
		{\
			return &TPyParent::Type != &::lass::python::PyObjectPlus::Type ?\
				&TPyParent::Type : &PyBaseObject_Type;\
		}\
		virtual PyTypeObject *GetType(void) const {return &Type;};

typedef PyTypeObject * PyParentObject;// Define the PyParent Object

namespace lass
{
	namespace python
	{
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

			PyObjectPlus(); 
			virtual ~PyObjectPlus();

			static void __dealloc(PyObject *P);

			static  PyObject*   __repr(PyObject *PyObj);
			virtual std::string pyRepr(void) { return std::string(ob_type->tp_name) + " object at " + util::stringCast<std::string>(this); }

			static  PyObject*   __str(PyObject *PyObj);
			virtual std::string pyStr(void) { return std::string(ob_type->tp_name) + " object string at " + util::stringCast<std::string>(this); }

		protected:

			PyObjectPlus(const PyObjectPlus& iOther);
			PyObjectPlus& operator=(const PyObjectPlus& iOther);
		};

		namespace impl
		{
			/** @internal
			 *  Fast reentrant lock for reference count operations
			 */
			extern LASS_DLL util::CriticalSection referenceMutex;

			/** @internal
			 *  On creation, PyObjectPlus are typeless (ob_type == 0), 
			 *  call this function to fix that for you.  
			 *  Objects created in C++ should call this at least once before
			 *  being used in Python.
			 */
			template <typename T> T* fixObjectType(T* iObject)
			{
				if (meta::IsDerivedType<T, PyObjectPlus>::value && 
					iObject && !iObject->ob_type)
				{
					iObject->ob_type = static_cast<PyObjectPlus*>(iObject)->GetType();
				}
				return iObject;
			}

		}

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
			PyObjectStorage(T* iPointee): 
				Cascade(), storage_(impl::fixObjectType(iPointee)) {} 
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
		class LASS_DLL PyObjectCounter
		{
		public:
			typedef int TCount;
		protected:
			PyObjectCounter() {}
			template <typename TStorage> void init(TStorage& /*iPointee*/) {}
			template <typename TStorage> void dispose(TStorage& /*iPointee*/) {}
			template <typename TStorage> void increment(TStorage& iPointee)
			{
				LASS_LOCK(impl::referenceMutex)
				{
					Py_INCREF(iPointee);
				}
			}
			template <typename TStorage> bool decrement(TStorage& iPointee)
			{
				bool r = false;
				LASS_LOCK(impl::referenceMutex)
				{
					LASS_ASSERT(iPointee);
					r = iPointee->ob_refcnt <=1;
					Py_DECREF(iPointee);
				}
				return r;
			}
			template <typename TStorage> TCount count(TStorage& iPointee) const
			{
				LASS_ASSERT(iPointee);
				return iPointee->ob_refcnt;
			}
			/** takes over the count and shares it */
			template <typename TOStorage, typename TIStorage>
			void initSharedCount(
					TOStorage& oStorage, const TIStorage& iStorage, 
					const PyObjectCounter& iOther)
			{
				oStorage->ob_refcnt = iStorage->ob_refcnt;
				LASS_ASSERT(oStorage->ob_refcnt);
			}
			void swap(PyObjectCounter& /*iOther*/) {}
		};

		/** templated "typedef" to a python shared pointer
		 *  @ingroup Python
		 */
		template<class T>
		struct PyObjectPtr
		{
			typedef util::SharedPtr<T, PyObjectStorage, PyObjectCounter> Type;
		};

		/** PyObjectPtr to a PyObject
		 *  @ingroup Python
		 */
		typedef PyObjectPtr<PyObject>::Type TPyObjPtr;

		/** fromNakedToSharedPtrCast.
		*   @ingroup Python
		*   Helper function casting a PyObject coming from the Python interface to a SharedPtr
		*   object for use in C++.  Reference counts are taken care of.
		*/
		template<class T>
		lass::util::SharedPtr<T, PyObjectStorage, PyObjectCounter>
		fromNakedToSharedPtrCast(PyObject* object)
		{
			LASS_LOCK(impl::referenceMutex) 
			{
				Py_XINCREF(object);
			}
			return util::SharedPtr<T,PyObjectStorage,PyObjectCounter>(static_cast<T*>(object));
		}

		/** fromSharedPtrToNakedCast.
		*   @ingroup Python
		*   Helper function casting an object used in C++ for use in Python.  The key operation
		*   done here is to take care of the reference counting. Failing to use this function may
		*   yield unexpected reference count.
		*/
		template<class T>
		PyObject*
		fromSharedPtrToNakedCast(const util::SharedPtr<T,PyObjectStorage,PyObjectCounter>& object)
		{
			PyObject* const obj = object.get();
			LASS_LOCK(impl::referenceMutex) 
			{
				Py_XINCREF(obj);
			}
			return obj;
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
		//inline int pyGetSimpleObject( PyObject* iValue, PyObject*& oV );
		template <typename T> int pyGetSimpleObject( 
			PyObject* iValue,  util::SharedPtr<T,PyObjectStorage,PyObjectCounter>& oV );

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
		//inline PyObject* pyBuildSimpleObject( PyObject* iV );
		template <typename T> PyObject* pyBuildSimpleObject( 
			const util::SharedPtr<T,PyObjectStorage,PyObjectCounter>& iV );

		LASS_DLL TPyObjPtr LASS_CALL getPyObjectByName(const std::string& iName);

		namespace impl
		{
			/** @internal
			 */
			class LASS_DLL OverloadLink
			{
			public:
				OverloadLink();
				void setNull();
				void setPyCFunction(PyCFunction iOverload);
				void setUnaryfunc(unaryfunc iOverload);
				void setBinaryfunc(binaryfunc iOverload);
				void setTernaryfunc(ternaryfunc iOverload);
				bool operator()(PyObject* iSelf, PyObject* iArgs, 
					PyObject*& result) const;
			private:
				PyCFunction pyCFunction_;
				unaryfunc unaryfunc_;
				binaryfunc binaryfunc_;
				ternaryfunc ternaryfunc_;
			};

			template <PyCFunction DispatcherAddress> PyObject* unaryDispatcher(
				PyObject* iSelf);
			template <PyCFunction DispatcherAddress> PyObject* binaryDispatcher(
				PyObject* iSelf, PyObject* iOther);
			template <PyCFunction DispatcherAddress> PyObject* ternaryDispatcher(
				PyObject* iSelf, PyObject* iArgs, PyObject* iKw);

			template <PyCFunction DispatcherAddress>
			struct DispatcherConvertor
			{
				static PyObject* asTernary(
						PyObject* iSelf, PyObject* iArgs, PyObject* iKw)
				{
					if (iKw)
					{
						PyErr_SetString(PyExc_TypeError, 
							"keyword arguments are not supported");
						return 0;
					}
					return DispatcherAddress(iSelf, iArgs);
				}
			};

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

			/** @internal
			 *  predicate to find a StaticMember by name.
			 */
			class LASS_DLL StaticMemberEqual
			{
			public:
				StaticMemberEqual(const char* iName);
				bool operator()(const StaticMember& iMethod) const;
			private:
				const char* name_;
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

			LASS_DLL StaticMember LASS_CALL createStaticMember(
				const char* iName, const char * iDocumentation, PyObject* iObject, 
				PyTypeObject* iParentType = 0, std::vector<PyMethodDef>* iMethods = 0, 
				std::vector<PyGetSetDef>* iGetSetters = 0, 
				const std::vector<StaticMember>* iStatics = 0);
			LASS_DLL PyMethodDef LASS_CALL createPyMethodDef(
				const char *ml_name, PyCFunction ml_meth, int ml_flags, 
				const char *ml_doc);
			LASS_DLL PyGetSetDef LASS_CALL createPyGetSetDef(
				const char* name, getter get, setter set, const char* doc, void* closure);

			LASS_DLL void LASS_CALL injectStaticMembers(
				PyTypeObject& iPyType, const std::vector<StaticMember>& iStatics);
			LASS_DLL void LASS_CALL finalizePyType(
				PyTypeObject& iPyType, PyTypeObject& iPyParentType, 
				std::vector<PyMethodDef>& iMethods, 
				std::vector<PyGetSetDef>& iGetSetters, 
				const std::vector<StaticMember>& iStatics, 
				const char* iModuleName, const char* iDocumentation);
			LASS_DLL void LASS_CALL addModuleFunction(
				std::vector<PyMethodDef>& ioModuleMethods, 
				char* iMethodName, char* iDocumentation,
				PyCFunction iMethodDispatcher, PyCFunction& oOverloadChain);
			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& ioPyType, std::vector<PyMethodDef>& ioClassMethods,
				const char* iMethodName, const char* iDocumentation,
				PyCFunction iMethodDispatcher, unaryfunc iUnaryDispatcher, 
				binaryfunc iBinaryDispatcher, ternaryfunc iTernaryDispatcher, 
				OverloadLink& oOverloadChain);

			template <typename CppClass> void injectClassInModule(
				PyObject* iModule, const char* iClassDocumentation);
			/*template <typename CppClass> void addClassMethod(
				const char* iMethodName, const char* iDocumentation, 
				PyCFunction iMethodDispatcher, unaryfunc iUnaryDispatcher, 
				binaryfunc iBinaryDispatcher, ternaryfunc iTernaryDispatcher,
				OverloadLink& oOverloadChain);*/
			template <typename CppClass> void addClassStaticMethod(
				const char* iMethodName, const char* iDocumentation,
				PyCFunction iMethodDispatcher, PyCFunction& oOverloadChain);
			template <typename CppClass, typename T> void addClassStaticConst(
				const char* iName, const T& iValue);
			template <typename InnerCppClass> void addClassInnerClass(
				std::vector<StaticMember>& oOuterStatics, 
				const char* iInnerClassName, const char* iDocumentation);

			template <typename In, typename Out> int pyNumericCast(In iIn, Out& oV);
			template <typename Integer> int pyGetSignedObject(
				PyObject* iValue, Integer& oV);
			template <typename Integer> int pyGetUnsignedObject(
				PyObject* iValue, Integer& oV);
			template <typename Float> int pyGetFloatObject(
				PyObject* iValue, Float& oV);

			LASS_DLL void LASS_CALL addMessageHeader(const std::string& iHeader);
			LASS_DLL bool LASS_CALL checkSequenceSize(PyObject* iValue, int iExpectedSize);

			namespace experimental
			{
				template <typename Out, typename In> inline
				Out	staticPyCast(const In& iIn)
				{
					return Out(::lass::python::PyPlus_INCREF(
						static_cast<typename Out::TPointer>(iIn.get())));
				}
				template <typename Out, typename In> inline
				Out	dynamicPyCast(const In& iIn)
				{
					return Out(::lass::python::PyPlus_INCREF(
						dynamic_cast<typename Out::TPointer>(iIn.get())));
				}
			}
		}
	}
}

#include "pyobject_plus.inl"

#endif
