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

/** @defgroup Python
 *  @brief interface library to Python
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H

#include "python_common.h"
#include "pyobject_ptr.h"
#include "../meta/is_derived.h"
#include "../meta/select.h"
#include "../util/string_cast.h"
#include <cstdlib>

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4267) // conversion from 'size_t' to 'unsigned int', possible loss of data
#endif

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
		virtual ::lass::python::impl::ClassDefinition* const _lassPyGetClassDef() const { return &_lassPyClassDef; } \
	private:

#include "pyobject_casters.h"
#include "pyobject_special_methods.h"
#include "pyobject_export_deprecated.h"

namespace lass
{
	namespace python
	{
		namespace impl
		{
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

			class ClassDefinition;

			/**	@ingroup
			 *	@internal
			 */
			struct StaticMember
			{
				TStaticMemberHelperPtr member;
				ClassDefinition* classDef;
				const char* name;
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

			class LASS_DLL ClassDefinition
			{
			public:
				ClassDefinition(const char* name, const char* doc, Py_ssize_t typeSize, richcmpfunc richcmp, ClassDefinition* parent);
				PyTypeObject* const type() { return &type_; }
				const PyTypeObject* const type() const { return &type_; }
				const char* name() const { return type_.tp_name; }
				const char* doc() const { return type_.tp_doc; }
				void setDoc(const char* doc) { type_.tp_doc = doc; } ///< @a doc must be valid until another one is set

				void freezeDefinition(const char* scopeName = 0);

				// currently, these are public to easy the transformation, but they will become private one day ...
				typedef std::vector<PyMethodDef> TMethods;
				typedef std::vector<PyGetSetDef> TGetSetters;
				PyTypeObject type_;
				TMethods methods_;
				TGetSetters getSetters_;
				TStaticMembers statics_;
				TCompareFuncs compareFuncs_;
				ClassDefinition* parent_;
				bool isFrozen_;
			};
		}

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
		public:
			static impl::ClassDefinition _lassPyClassDef;
			virtual impl::ClassDefinition* const _lassPyGetClassDef() const { return &_lassPyClassDef; }
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

			/**	@ingroup Python
			 *	@internal
			 */
			class LASS_DLL OverloadLink
			{
			public:
				enum Signature
				{
					sNull,
					sPyCFunction,
					sBinary,
					sTernary,
					sSsizeArg,
					sSsizeSsizeArg,
					sSsizeObjArg,
					sSsizeSsizeObjArg,
					sObjObj,
					sObjObjArg,
				};
				OverloadLink();
				void setNull();
				void setPyCFunction(PyCFunction iOverload);
				void setBinaryfunc(binaryfunc iOverload);
				void setTernaryfunc(ternaryfunc iOverload);
				
				void setSsizeArgfunc(ssizeargfunc iOverload);
				void setSsizeSsizeArgfunc(ssizessizeargfunc iOverload);
				void setSsizeObjArgProcfunc(ssizeobjargproc iOverload);
				void setSsizeSsizeObjArgProcfunc(ssizessizeobjargproc iOverload);
				void setObjObjProcfunc(objobjproc iOverload);
				void setObjObjArgProcfunc(objobjargproc iOverload);

				bool operator()(PyObject* iSelf, PyObject* iArgs, 
					PyObject*& result) const;
			private:
				PyObject* call(PyObject* iSelf, PyObject* iArgs) const;
				union
				{
					PyCFunction pyCFunction_;
					binaryfunc binaryfunc_;
					ternaryfunc ternaryfunc_;

					ssizeargfunc ssizeargfunc_;
					ssizessizeargfunc ssizessizeargfunc_;
					ssizeobjargproc ssizeobjargproc_;
					ssizessizeobjargproc ssizessizeobjargproc_;
					objobjproc objobjproc_;
					objobjargproc objobjargproc_;

					getiterfunc getiterfunc_;
					iternextfunc iternextfunc_;
				};
				Signature signature_;
			};

			template <typename T, PyCFunction dispatcher> struct FunctionTypeDispatcher;

			/**	@ingroup
			 *	@internal
			 */
			/*
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
			*/

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
					const TCompareFuncs::const_iterator end = CppClass::_lassPyClassDef.compareFuncs_.end();
					for (TCompareFuncs::const_iterator i = CppClass::_lassPyClassDef.compareFuncs_.begin(); i != end; ++i)
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
					static const char* symbols[] = { "<", "<=", "==", "!=", ">", ">=" };
					LASS_ASSERT(op >= 0 && op <= Py_GE);
					std::ostringstream buffer;
					buffer << "Comparison operator " << symbols[op] << " not implemented for this type";
					PyErr_SetString(PyExc_NotImplementedError, buffer.str().c_str());
					return 0;				
				}
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
				const char* iName, const TStaticMemberHelperPtr& iObject, ClassDefinition* iClassDef = 0);
			LASS_DLL PyMethodDef LASS_CALL createPyMethodDef(
				const char *ml_name, PyCFunction ml_meth, int ml_flags, 
				const char *ml_doc);
			LASS_DLL PyGetSetDef LASS_CALL createPyGetSetDef(
				const char* name, getter get, setter set, const char* doc, void* closure);

			LASS_DLL void LASS_CALL injectStaticMembers(ClassDefinition& classDef);

			LASS_DLL void LASS_CALL addClassMethod(
				ClassDefinition& classDef,
				const char* methodName, const char* documentation, 
				PyCFunction i_dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				ClassDefinition& classDef,
				const lass::python::impl::ComparatorSlot& methodName, const char* documentation, 
				PyCFunction dispatcher, OverloadLink& overloadChain); 

			LASS_DLL void LASS_CALL addClassMethod(
				ClassDefinition& classDef,
				const lass::python::impl::UnarySlot& methodName, const char* documentation, 
				unaryfunc dispatcher, OverloadLink& overloadChain); 

			LASS_DLL void LASS_CALL addClassMethod(
				ClassDefinition& classDef,
				const lass::python::impl::BinarySlot& methodName, const char* documentation, 
				binaryfunc dispatcher, OverloadLink& overloadChain); 

			LASS_DLL void LASS_CALL addClassMethod(
				ClassDefinition& classDef,
				const lass::python::impl::TernarySlot& methodName, const char* documentation, 
				ternaryfunc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				ClassDefinition& classDef,
				const lass::python::impl::LenSlot&, const char* documentation, 
				lenfunc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				ClassDefinition& classDef,
				const lass::python::impl::SsizeArgSlot&, const char* documentation, 
				ssizeargfunc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				ClassDefinition& classDef,
				const lass::python::impl::SsizeSsizeArgSlot&, const char* documentation, 
				ssizessizeargfunc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				ClassDefinition& classDef,
				const lass::python::impl::SsizeObjArgSlot&, const char* documentation, 
				ssizeobjargproc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				ClassDefinition& classDef,
				const lass::python::impl::SsizeSsizeObjArgSlot&, const char* documentation, 
				ssizessizeobjargproc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				ClassDefinition& classDef,
				const lass::python::impl::ObjObjArgSlot&, const char* documentation, 
				objobjargproc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				ClassDefinition& classDef,
				const lass::python::impl::IterSlot&, const char* documentation, 
				getiterfunc dispatcher, OverloadLink& overloadChain);

			LASS_DLL void LASS_CALL addClassMethod(
				ClassDefinition& classDef,
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
