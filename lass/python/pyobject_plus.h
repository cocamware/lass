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
		virtual ::lass::python::impl::ClassDefinition* _lassPyGetClassDef() const { return &_lassPyClassDef; } \
	private:

#include "pyobject_casters.h"
#include "pyobject_special_methods.h"
#include "export_traits.h"

namespace lass
{
	namespace python
	{
		namespace impl
		{
			class NamePredicate
			{
			public:
				NamePredicate(const char* name): name_(name) {}
				template <typename T> bool operator()(const T& x) const { return cmp(x.name()); }
				bool operator()(const PyMethodDef& x) const { return cmp(x.ml_name); }
				template <typename T> bool operator()(T* x) const { return (*this)(*x); }
			private:
				bool cmp(const char* name) const { return name && strcmp(name, name_) == 0; }
				const char* name_;
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

			class StaticMember
			{
			public:
				StaticMember(const char* name, const TStaticMemberHelperPtr member): member_(member), name_(name) {}
				const TStaticMemberHelperPtr& member() const { return member_; }
				const char* name() const { return name_; }
			private:
				TStaticMemberHelperPtr member_;
				const char* name_;
			};

			struct LASS_DLL CompareFunc
			{
				PyCFunction dispatcher;
				int op;
				CompareFunc(PyCFunction dispatcher, int op): dispatcher(dispatcher), op(op) {}
			};

			template <typename CppClass> PyObject* richCompareDispatcher(PyObject* self, PyObject* other, int op)
			{
				return CppClass::_lassPyClassDef.callRichCompare(self, other, op);
			}

			class OverloadLink;

			class LASS_DLL ClassDefinition
			{
			public:
				typedef void(*TClassRegisterHook)();

				ClassDefinition(const char* name, const char* doc, Py_ssize_t typeSize, 
					richcmpfunc richcmp, ClassDefinition* parent, TClassRegisterHook registerHook);
				PyTypeObject* type() { return &type_; }
				const PyTypeObject* type() const { return &type_; }
				const char* name() const { return type_.tp_name; }
				const char* doc() const { return type_.tp_doc; }
				void setDoc(const char* doc) { type_.tp_doc = const_cast<char*>(doc); } ///< @a doc must be valid until another one is set

				void addMethod(const char* name, const char* doc, PyCFunction dispatcher, OverloadLink& overloadChain);
				void addMethod(const ComparatorSlot& slot, const char* doc, PyCFunction dispatcher, OverloadLink& overloadChain);
				void addMethod(const UnarySlot& slot, const char* doc, unaryfunc dispatcher, OverloadLink& overloadChain); 
				void addMethod(const BinarySlot& slot, const char* doc, binaryfunc dispatcher, OverloadLink& overloadChain); 
				void addMethod(const TernarySlot& slot, const char* doc, ternaryfunc dispatcher, OverloadLink& overloadChain);
				void addMethod(const LenSlot& slot, const char* doc, lenfunc dispatcher, OverloadLink& overloadChain);
				void addMethod(const SsizeArgSlot& slot, const char* doc, ssizeargfunc dispatcher, OverloadLink& overloadChain);
				void addMethod(const SsizeSsizeArgSlot& slot, const char* doc, ssizessizeargfunc dispatcher, OverloadLink& overloadChain);
				void addMethod(const SsizeObjArgSlot& slot, const char* doc, ssizeobjargproc dispatcher, OverloadLink& overloadChain);
				void addMethod(const SsizeSsizeObjArgSlot& slot, const char* doc, ssizessizeobjargproc dispatcher, OverloadLink& overloadChain);
				void addMethod(const ObjObjSlot& slot, const char* doc, objobjproc dispatcher, OverloadLink& overloadChain);
				void addMethod(const ObjObjArgSlot& slot, const char* doc, objobjargproc dispatcher, OverloadLink& overloadChain);
				void addMethod(const IterSlot& slot, const char* doc, getiterfunc dispatcher, OverloadLink& overloadChain);
				void addMethod(const IterNextSlot& slot, const char* doc, iternextfunc dispatcher, OverloadLink& overloadChain);
				void addGetSetter(const char* name, const char* doc, getter get, setter set);
				void addStaticMethod(const char* name, const char* doc, PyCFunction dispatcher, PyCFunction& overloadChain);
				template <typename T> void addStaticConst(const char* name, const T& value)
				{
					LASS_ASSERT(std::count_if(statics_.begin(), statics_.end(), NamePredicate(name)) == 0);
					statics_.push_back(StaticMember(name, staticMemberHelperObject(value)));
				}
				void addInnerClass(ClassDefinition& innerClass);
				
				void freezeDefinition(const char* scopeName = 0);

				PyObject* callRichCompare(PyObject* self, PyObject* other, int op);

			private:
				typedef std::vector<PyMethodDef> TMethods;
				typedef std::vector<PyGetSetDef> TGetSetters;
				typedef std::vector<CompareFunc> TCompareFuncs;
				typedef std::vector<StaticMember> TStaticMembers;
				typedef std::vector<ClassDefinition*> TClassDefs;

				PyTypeObject type_;
				TMethods methods_;
				TGetSetters getSetters_;
				TCompareFuncs compareFuncs_;
				TStaticMembers statics_;
				TClassDefs innerClasses_;
				TClassDefs subClasses_;
				ClassDefinition* parent_;
				TClassRegisterHook classRegisterHook_;
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
			static void _lassPyClassRegisterHook();
			virtual impl::ClassDefinition* _lassPyGetClassDef() const { return &_lassPyClassDef; }
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

			//template <typename T, PyCFunction dispatcher> struct FunctionTypeDispatcher;


			/**	@ingroup
			 *	@internal
			 */


			LASS_DLL PyMethodDef LASS_CALL createPyMethodDef(
				const char *ml_name, PyCFunction ml_meth, int ml_flags, 
				const char *ml_doc);
			LASS_DLL PyGetSetDef LASS_CALL createPyGetSetDef(
				const char* name, getter get, setter set, const char* doc, void* closure);


			LASS_DLL bool LASS_CALL checkSequenceSize(PyObject* iValue, Py_ssize_t iExpectedSize);
			LASS_DLL TPyObjPtr LASS_CALL checkedFastSequence(PyObject* obj);
			LASS_DLL TPyObjPtr LASS_CALL checkedFastSequence(PyObject* obj, Py_ssize_t expectedSize);
			LASS_DLL TPyObjPtr LASS_CALL checkedFastSequence(PyObject* obj, Py_ssize_t minimumSize, Py_ssize_t maximumSize);
			LASS_DLL PyObject* LASS_CALL establishMagicalBackLinks(PyObject* result, PyObject* self);
		}
	}
}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#include "pyobject_plus.inl"

#endif
