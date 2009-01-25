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

/** @defgroup Python
 *  @brief interface library to Python
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H

#include "python_common.h"
#include "../util/singleton.h"

// Python.h is a bit blunt in (re)defining _POSIX_C_SOURCE causing a nice warning.
// Undefing it before including Python.h will suppress that warning.
// Remove this once Python plays nice again. 
// [Bramz]
#if defined(_POSIX_C_SOURCE)
#	undef _POSIX_C_SOURCE
#endif

#if defined(_DEBUG) && LASS_PYTHON_HAS_DEBUG_BUILD == 0
#	undef _DEBUG
#	include "Python.h"
#	define _DEBUG
#else
#	include "Python.h"
#endif

#ifndef PySequence_ITEM
#	define PySequence_ITEM(o, i) PySequence_GetItem(o, i)
#endif

#ifndef PySequence_Fast_ITEMS
#	define PySequence_Fast_ITEMS(o) \
	(PyTuple_Check(o) ? ((PyTupleObject *)(o))->ob_item : ((PyListObject *)(o))->ob_item)
#endif

#ifndef Py_RETURN_FALSE
#	define Py_RETURN_FALSE return Py_INCREF(Py_False), Py_False
#endif

#ifndef Py_RETURN_TRUE
#	define Py_RETURN_TRUE return Py_INCREF(Py_True), Py_True
#endif

#ifndef Py_RETURN_NONE
#	define Py_RETURN_NONE return Py_INCREF(Py_None), Py_None
#endif

#if (PY_VERSION_HEX < 0x02050000)
#	define Py_ssize_t int
#	define lenfunc inquiry
#	define ssizeargfunc intargfunc
#	define ssizessizeargfunc intintargfunc
#	define ssizeobjargproc intobjargproc
#	define ssizessizeobjargproc intintobjargproc
#endif

#include "../meta/is_derived.h"
#include "../meta/select.h"
#include "../util/shared_ptr.h"
#include "../util/string_cast.h"
#include "../util/thread.h"
#include <cstdlib>

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4267) // conversion from 'size_t' to 'unsigned int', possible loss of data
#endif

/** @internal
 *  This macro is only used in the PyObjectPlus class and is for internal LASS
 *  use.  Do not use in custom objects... DON'T!
 */
#define PY_HEADER_INTERNAL \
	public: \
		static PyTypeObject   _lassPyType; \
		static ::std::vector<PyMethodDef>    _lassPyMethods; \
		static ::std::vector<PyGetSetDef>    _lassPyGetSetters; \
		virtual PyTypeObject *_lassPyGetType(void) const {return &_lassPyType;};\
	private:

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
		typedef t_parentClass _lassPyParentType;\
		static PyTypeObject _lassPyType; \
		static ::std::vector<PyMethodDef> _lassPyMethods; \
		static ::std::vector<PyGetSetDef> _lassPyGetSetters; \
		static ::lass::python::impl::TStaticMembers	_lassPyStatics; \
		static ::lass::python::impl::TCompareFuncs _lassPyCompareFuncs;\
		virtual PyTypeObject* _lassPyGetType(void) const {return &_lassPyType;};\
		static PyTypeObject* _lassPyGetParentType(void)\
		{\
			return &_lassPyParentType::_lassPyType != &::lass::python::PyObjectPlus::_lassPyType ?\
				&_lassPyParentType::_lassPyType : &PyBaseObject_Type;\
		}\
	private:

#include "pyobject_casters.h"
#include "pyobject_special_methods.h"
#include "pyobject_export_deprecated.h"

namespace lass
{
	namespace python
	{
		/** @ingroup Python
		 */
		template<typename T>
		PyObject* pyBuildSimpleObject(T& iV)
		{
			return PyExportTraits<T>::build(iV);
		}

		/** @ingroup Python
		 */
		template<typename T>
		PyObject* pyBuildSimpleObject(const T& iV)
		{
			return PyExportTraits<T>::build(iV);
		}

		/** @ingroup Python
		 */
		template<typename T>
		PyObject* pyBuildSimpleObject(std::auto_ptr<T> iV)
		{
			return PyExportTraits< std::auto_ptr<T> >::build(iV);
		}

		/** @ingroup Python
		 */
		template<typename T>
		int pyGetSimpleObject(PyObject* iV, T& oV)
		{
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
		class LASS_DLL PyObjectPlus :
			public PyObject
		{
			PY_HEADER_INTERNAL;
		public:
			typedef void TCppClass;
			PyObjectPlus(); 
			virtual ~PyObjectPlus();
			virtual std::string doPyRepr(void) { return std::string(ob_type->tp_name) + " object at " + util::stringCast<std::string>(this); }
			virtual std::string doPyStr(void) { return std::string(ob_type->tp_name) + " object string at " + util::stringCast<std::string>(this); }

		protected:
			PyObjectPlus(const PyObjectPlus& other);
			PyObjectPlus& operator=(const PyObjectPlus& other);
#ifdef LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
		public:
			PyObject* dict_;
#endif
		private:
		};

		namespace impl
		{
			/** @internal
			 *  Fast reentrant lock for reference count operations
			 */
			inline util::CriticalSection& referenceMutex()
			{
				using namespace util;
				return *Singleton<CriticalSection, destructionPriorityInternalPythonMutex>::instance();
			}

			/** @internal
			 *  On creation, PyObjectPlus are typeless (ob_type == 0), 
			 *  call this function to fix that for you.  
			 *  Objects created in C++ should call this at least once before
			 *  being used in Python.
			 */
			template <typename T> T* fixObjectType(T* iObject)
			{
				if (meta::IsDerived<T, PyObjectPlus>::value && 
					iObject && !iObject->ob_type)
				{
					iObject->ob_type = static_cast<PyObjectPlus*>(iObject)->_lassPyGetType();
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
			explicit PyObjectStorage(T* pointee): Cascade(), storage_(impl::fixObjectType(pointee)) {}
			PyObjectStorage(const PyObjectStorage& other): Cascade(other), storage_(other.storage_) {}
			template <typename U> PyObjectStorage(const PyObjectStorage<U, Cascade>& other): 
				Cascade(other), storage_(other.storage()) {}
			TPointer pointer() const { return storage_; }
			void dispose() { storage_ = 0; }
			bool isNull() const { return !storage_; }
			void swap(TSelf& other) { Cascade::swap(other); std::swap(storage_, other.storage_);   }
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
			template <typename TStorage> void init(TStorage& /*pointee*/) {}
			template <typename TStorage> void dispose(TStorage& /*pointee*/) {}
			template <typename TStorage> void increment(TStorage& pointee)
			{
				LASS_LOCK(impl::referenceMutex())
				{
					Py_INCREF(pointee);
				}
			}
			template <typename TStorage> bool decrement(TStorage& pointee)
			{
				bool r = false;
				LASS_LOCK(impl::referenceMutex())
				{
					LASS_ASSERT(pointee);
					r = pointee->ob_refcnt <=1;
					Py_DECREF(pointee);
				}
				return r;
			}
			template <typename TStorage> TCount count(TStorage& pointee) const
			{
				LASS_ASSERT(pointee);
				return pointee->ob_refcnt;
			}
			void swap(PyObjectCounter& /*other*/) {}
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

		namespace impl
		{
			LASS_DLL const std::string exceptionExtractMessage(
					const TPyObjPtr& type, const TPyObjPtr& value);
		}

		class PythonException: public util::Exception
		{
		public:
			PythonException(
					const TPyObjPtr& type, const TPyObjPtr& value, const TPyObjPtr& traceback, 
					const std::string& loc):
				util::Exception(impl::exceptionExtractMessage(type, value), loc),
				type_(type),
				value_(value),
				traceback_(traceback)
			{
			}
			~PythonException() throw() {}
			const python::TPyObjPtr& type() const { return type_; } 
			const python::TPyObjPtr& value() const { return value_; } 
			const python::TPyObjPtr& traceback() const { return traceback_; } 
		private:
			LASS_UTIL_EXCEPTION_PRIVATE_IMPL(PythonException)
			python::TPyObjPtr type_;
			python::TPyObjPtr value_;
			python::TPyObjPtr traceback_;
		};

		/** fromNakedToSharedPtrCast.
		*   @ingroup Python
		*   Helper function casting a PyObject coming from the Python interface to a SharedPtr
		*   object for use in C++.  Reference counts are taken care of.
		*/
		template<class T>
		lass::util::SharedPtr<T, PyObjectStorage, PyObjectCounter>
		fromNakedToSharedPtrCast(PyObject* object)
		{
			LASS_LOCK(impl::referenceMutex()) 
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
			LASS_LOCK(impl::referenceMutex()) 
			{
				Py_XINCREF(obj);
			}
			return obj;
		}

		/** static_cast for python pointers
		 *  @ingroup Python
		 */  
		template <typename Out, typename In> inline
		Out	staticPyCast(const In& in)
		{
			typedef typename Out::TPointer TPtr;
			TPtr ptr = static_cast<TPtr>(in.get());
			Py_XINCREF(ptr);
			return Out(ptr);
		}

		/** dynamic_cast for python pointers
		 *  @ingroup Python
		 */  
		template <typename Out, typename In> inline
		Out	dynamicPyCast(const In& in)
		{
			typedef typename Out::TPointer TPtr;
			TPtr ptr = dynamic_cast<TPtr>(in.get());
			Py_XINCREF(ptr);
			return Out(ptr);
		}

		/** meta function to detect if a type is a PyObject-derived type
		*   @ingroup Python
		*/
		template <typename T> struct IsPyObject: meta::IsDerived<T, PyObject> {};

		LASS_DLL TPyObjPtr LASS_CALL getPyObjectByName(const std::string& iName);

		namespace impl
		{
			LASS_DLL void LASS_CALL dealloc(PyObject* obj);
			LASS_DLL PyObject* LASS_CALL repr(PyObject* obj);
			LASS_DLL PyObject* LASS_CALL str(PyObject* obj);

			class LASS_DLL ModuleDefinition: util::NonCopyable
			{
			public:
				ModuleDefinition(const char* name, const char* doc = 0);
				const char* name() const { return name_.get(); }
				void setName(const char* name);
				const char* doc() const { return doc_.get(); }
				void setDoc(const char* doc);
				PyObject* module() const { return module_; }
				void addFunctionDispatcher(PyCFunction dispatcher, const char* name, const char* doc, PyCFunction& overloadChain);
				template <typename CppClass> void injectClass(const char* doc);
				PyObject* inject();
			private:
				typedef std::vector<PyMethodDef> TMethods;
				struct NamedObject
				{
					char* name;
					PyObject* object;
				};
				typedef std::vector<NamedObject> TObjects;
				typedef util::ScopedPtr< char, util::ArrayStorage > TScopedCString;
				TMethods methods_;
				TObjects objects_;
				TScopedCString name_;
				TScopedCString doc_;
				PyObject* module_;
#if PY_MAJOR_VERSION >= 3
				PyModuleDef def_;
#endif
				bool isInjected_;
			};

			/**	@ingroup Python
			 *	@internal
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
				
				void setSsizeArgfunc(ssizeargfunc iOverload);
				void setSsizeSsizeArgfunc(ssizessizeargfunc iOverload);
				void setLenfunc(lenfunc iOverload);
				void setSsizeObjArgProcfunc(ssizeobjargproc iOverload);
				void setSsizeSsizeObjArgProcfunc(ssizessizeobjargproc iOverload);
				void setObjObjProcfunc(objobjproc iOverload);
				void setObjObjArgProcfunc(objobjargproc iOverload);

				void setIterfunc(getiterfunc iOverload);
				void setIterNextfunc(iternextfunc iOverload);

				bool operator()(PyObject* iSelf, PyObject* iArgs, 
					PyObject*& result) const;
			private:
				PyCFunction pyCFunction_;
				unaryfunc unaryfunc_;
				binaryfunc binaryfunc_;
				ternaryfunc ternaryfunc_;

				ssizeargfunc ssizeargfunc_;
				ssizessizeargfunc ssizessizeargfunc_;
				lenfunc lenfunc_;
				ssizeobjargproc ssizeobjargproc_;
				ssizessizeobjargproc ssizessizeobjargproc_;
				objobjproc objobjproc_;
				objobjargproc objobjargproc_;

				getiterfunc getiterfunc_;
				iternextfunc iternextfunc_;
			};

			template <typename T, PyCFunction dispatcher> struct FunctionTypeDispatcher;

			/**	@ingroup
			 *	@internal
			 */
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

			/**	@ingroup
			 *	@internal
			 */
			struct LASS_DLL CompareFunc
			{
				PyCFunction dispatcher;
				int op;
				CompareFunc(PyCFunction dispatcher, int op): dispatcher(dispatcher), op(op) {}
			};

			/**	@ingroup
			 *	@internal
			 */
			typedef std::vector<CompareFunc> TCompareFuncs;

			/**	@ingroup
			 *	@internal
			 */
			template <typename CppClass>
			struct RichCompare
			{
				static PyObject* call(PyObject* self, PyObject* other, int op)
				{
					if (other==Py_None)
					{
						// we need to treat the None type differently because the pyGet/BuildSimpleObject are able to cast
						// from None but if you give that thing to a reference, then you are in big trouble
						switch (op)
						{
						case Py_EQ:
							{
								if (self==other)
									Py_RETURN_TRUE;
								else
									Py_RETURN_FALSE;
							}
						case Py_NE:
							{
								if (self!=other)
									Py_RETURN_TRUE;
								else
									Py_RETURN_FALSE;
							}
						// don't define any order relation on None
						default:
							Py_RETURN_FALSE;
						};
					}

					TPyObjPtr args(Py_BuildValue("(O)", other));
					const TCompareFuncs::const_iterator end = CppClass::_lassPyCompareFuncs.end();
					for (TCompareFuncs::const_iterator i = CppClass::_lassPyCompareFuncs.begin(); i != end; ++i)
					{
						if (i->op == op)
						{
							PyObject* result = (i->dispatcher)(self, args.get());\
							if (result || PyErr_Occurred() && !PyErr_ExceptionMatches(PyExc_TypeError))
							{
								return result;
							}
						}
					}

					return RichCompare<typename CppClass::_lassPyParentType>::call(self, other, op);
				}
			};

			/**	@ingroup
			 *	@internal
			 */
			template <>
			struct RichCompare<PyObjectPlus>
			{
				static PyObject* call(PyObject* /*self*/, PyObject* /*other*/, int op)
				{
					static char* symbols[] = { "<", "<=", "==", "!=", ">", ">=" };
					LASS_ASSERT(op >= 0 && op <= Py_GE);
					std::ostringstream buffer;
					buffer << "Comparison operator " << symbols[op] << " not implemented for this type";
					PyErr_SetString(PyExc_NotImplementedError, buffer.str().c_str());
					return 0;				
				}
			};

			class StaticMemberHelper
			{
			public:
				virtual ~StaticMemberHelper() {}
				virtual PyObject* build() const = 0;
			};
			typedef util::SharedPtr<StaticMemberHelper> TStaticMemberHelperPtr;

			template <typename T>
			class StaticMemberHelperObject: public StaticMemberHelper
			{
			public:
				StaticMemberHelperObject(const T& obj): obj_(obj) {}
				PyObject* build() const { return pyBuildSimpleObject(obj_); }
			private:
				T obj_;
			};
			template <typename T, size_t N>
			class StaticMemberHelperObject<T[N]>: public StaticMemberHelper
			{
			public:
				StaticMemberHelperObject(T obj[N]): obj_(obj) {}
				PyObject* build() const { return pyBuildSimpleObject(obj_); }
			private:
				T* obj_;
			};
			template <>
			class StaticMemberHelperObject<PyObject*>: public StaticMemberHelper
			{
			public:
				StaticMemberHelperObject(PyObject* obj): obj_(obj) {}
 				PyObject* build() const { return obj_; }
			private:
				PyObject* obj_;
			};
			template <typename T>
			inline TStaticMemberHelperPtr staticMemberHelperObject(const T& obj)
			{
				return TStaticMemberHelperPtr(new StaticMemberHelperObject<T>(obj));
			}

			class StaticMemberHelperType: public StaticMemberHelper
			{
			public:
				StaticMemberHelperType(PyTypeObject* type): type_(type) {}
				PyObject* build() const { return reinterpret_cast<PyObject*>(type_); }
			private:
				PyTypeObject* type_;
			};
			inline TStaticMemberHelperPtr staticMemberHelperType(PyTypeObject* type)
			{
				return TStaticMemberHelperPtr(new StaticMemberHelperType(type));
			}

			/**	@ingroup
			 *	@internal
			 */
			struct StaticMember
			{
				TStaticMemberHelperPtr member;
				PyTypeObject* parentType;
				std::vector<PyMethodDef>* methods;
				std::vector<PyGetSetDef>* getSetters;
				const std::vector<StaticMember>* statics;
				const char* name;
				const char* doc;
			};
			typedef std::vector<StaticMember> TStaticMembers;

			/**	@ingroup
			 *	@internal
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

			/**	@ingroup
			 *	@internal
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
				const char* iName, const char * iDocumentation, const TStaticMemberHelperPtr& iObject, 
				PyTypeObject* iParentType = 0, std::vector<PyMethodDef>* iMethods = 0, 
				std::vector<PyGetSetDef>* iGetSetters = 0, 
				const TStaticMembers* iStatics = 0);
			LASS_DLL PyMethodDef LASS_CALL createPyMethodDef(
				const char *ml_name, PyCFunction ml_meth, int ml_flags, 
				const char *ml_doc);
			LASS_DLL PyGetSetDef LASS_CALL createPyGetSetDef(
				const char* name, getter get, setter set, const char* doc, void* closure);

			LASS_DLL void LASS_CALL injectStaticMembers(
				PyTypeObject& iPyType, const TStaticMembers& iStatics);
			LASS_DLL void LASS_CALL finalizePyType(
				PyTypeObject& iPyType, PyTypeObject& iPyParentType, 
				std::vector<PyMethodDef>& iMethods, 
				std::vector<PyGetSetDef>& iGetSetters, 
				const TStaticMembers& iStatics, 
				const char* iModuleName, const char* iDocumentation);

			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& pyType, std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs, 
				const char* methodName, const char* documentation, 
				PyCFunction i_dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& pyType, std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
				const lass::python::impl::ComparatorSlot& methodName, const char* documentation, 
				PyCFunction dispatcher, OverloadLink& overloadChain); 

			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& pyType, std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
				const lass::python::impl::UnarySlot& methodName, const char* documentation, 
				unaryfunc dispatcher, OverloadLink& overloadChain); 

			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& pyType, std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
				const lass::python::impl::BinarySlot& methodName, const char* documentation, 
				binaryfunc dispatcher, OverloadLink& overloadChain); 

			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& pyType, std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
				const lass::python::impl::TernarySlot& methodName, const char* documentation, 
				ternaryfunc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& pyType, std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
				const lass::python::impl::LenSlot&, const char* documentation, 
				lenfunc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& pyType, std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
				const lass::python::impl::SsizeArgSlot&, const char* documentation, 
				ssizeargfunc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& pyType, std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
				const lass::python::impl::SsizeSsizeArgSlot&, const char* documentation, 
				ssizessizeargfunc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& pyType, std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
				const lass::python::impl::SsizeObjArgSlot&, const char* documentation, 
				ssizeobjargproc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& pyType, std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
				const lass::python::impl::SsizeSsizeObjArgSlot&, const char* documentation, 
				ssizessizeobjargproc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& pyType, std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
				const lass::python::impl::ObjObjArgSlot&, const char* documentation, 
				objobjargproc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& pyType, std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
				const lass::python::impl::IterSlot&, const char* documentation, 
				getiterfunc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				PyTypeObject& pyType, std::vector<PyMethodDef>& classMethods, TCompareFuncs& compareFuncs,
				const lass::python::impl::IterNextSlot&, const char* documentation, 
				iternextfunc dispatcher, OverloadLink& overloadChain);

			template <typename CppClass> void addClassStaticMethod(
				const char* iMethodName, const char* iDocumentation,
				PyCFunction iMethodDispatcher, PyCFunction& oOverloadChain);
			template <typename CppClass, typename T> void addClassStaticConst(
				const char* iName, const T& iValue);
			template <typename InnerCppClass> void addClassInnerClass(
				TStaticMembers& oOuterStatics,
				const char* iInnerClassName, const char* iDocumentation);

			template <typename In, typename Out> int pyNumericCast(In iIn, Out& oV);
			template <typename Integer> int pyGetSignedObject(
				PyObject* iValue, Integer& oV);
			template <typename Integer> int pyGetUnsignedObject(
				PyObject* iValue, Integer& oV);
			template <typename Float> int pyGetFloatObject(
				PyObject* iValue, Float& oV);

			LASS_DLL void LASS_CALL addMessageHeader(const std::string& iHeader);
			LASS_DLL bool LASS_CALL checkSequenceSize(PyObject* iValue, Py_ssize_t iExpectedSize);
			LASS_DLL TPyObjPtr LASS_CALL checkedFastSequence(PyObject* iValue, Py_ssize_t iExpectedSize);
		}
	}
}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#include "pyobject_plus.inl"

#endif
