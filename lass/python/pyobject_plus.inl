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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

namespace lass
{
namespace python
{

// --- impl ----------------------------------------------------------------------------------------

namespace impl
{

/** @internal
 */
template <typename T, PyCFunction dispatcher> struct FunctionTypeDispatcher
{
	static PyObject* fun(PyObject* self, PyObject* args) { return dispatcher(self,args); }
	typedef PyCFunction OverloadType;
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::UnarySlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf)
	{
		TPyObjPtr args(PyTuple_New(0));
		return DispatcherAddress(iSelf, args.get());
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::BinarySlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf, PyObject* iOther)
	{
		TPyObjPtr args(Py_BuildValue("(O)", iOther));
		return DispatcherAddress(iSelf, args.get());
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::TernarySlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf, PyObject* iArgs, PyObject* iKw)
	{
		if (iKw)
		{
			PyErr_SetString(PyExc_TypeError, "keyword arguments are not supported");
			return 0;
		}
		return DispatcherAddress(iSelf, iArgs);
	}
};
/** @internal
*/
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::SsizeArgSlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf, Py_ssize_t iSize) 
	{
		TPyObjPtr args(Py_BuildValue("(" LASS_PY_SSIZE_FORMAT ")", iSize));
		return DispatcherAddress(iSelf,args.get());
	}
};
/** @internal
*/
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::SsizeSsizeArgSlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf, Py_ssize_t iSize, Py_ssize_t iSize2)
	{
		TPyObjPtr args(Py_BuildValue("(" LASS_PY_SSIZE_FORMAT "," LASS_PY_SSIZE_FORMAT ")", iSize, iSize2));
		return DispatcherAddress(iSelf,args.get());
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::LenSlot ,DispatcherAddress>
{
	static Py_ssize_t fun(PyObject* iSelf) 
	{
		TPyObjPtr args(Py_BuildValue("()"));
		TPyObjPtr temp(DispatcherAddress(iSelf,args.get()));
		if (!temp)
			return -1;
		Py_ssize_t result;
		if (pyGetSimpleObject(temp.get(), result) != 0)
			return -1;
		return result;
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::SsizeObjArgSlot ,DispatcherAddress>
{
	static int fun(PyObject * iSelf, Py_ssize_t iSize, PyObject * iOther)
	{
		TPyObjPtr args(Py_BuildValue("(" LASS_PY_SSIZE_FORMAT ",O)",iSize,iOther));
		TPyObjPtr temp(DispatcherAddress(iSelf,args.get()));
		return temp ? 0 : -1;
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::SsizeSsizeObjArgSlot ,DispatcherAddress>
{
	static int fun(PyObject * iSelf, Py_ssize_t iSize, Py_ssize_t iSize2, PyObject * iOther)
	{
		TPyObjPtr args(Py_BuildValue("(" LASS_PY_SSIZE_FORMAT "," LASS_PY_SSIZE_FORMAT ",O)",iSize,iSize2,iOther));
		TPyObjPtr temp(DispatcherAddress(iSelf,args.get()));
		return temp ? 0 : -1;
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::ObjObjSlot ,DispatcherAddress>
{
	static int fun(PyObject * iSelf, PyObject * iOther)
	{
		TPyObjPtr args(Py_BuildValue("(O)",iOther));
		TPyObjPtr temp(DispatcherAddress(iSelf,args.get()));
		return temp ? 0 : -1;
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::ObjObjArgSlot ,DispatcherAddress>
{
	static int fun(PyObject * iSelf, PyObject * iOther, PyObject* iOther2)
	{
		TPyObjPtr args(Py_BuildValue("(O,O)",iOther,iOther2));
		TPyObjPtr temp(DispatcherAddress(iSelf,args.get()));
		return temp ? 0 : -1;
	}
};
/** @internal
*/
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::IterSlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf)
	{
		TPyObjPtr args(PyTuple_New(0));
		return DispatcherAddress(iSelf, args.get());
	}
};
/** @internal
*/
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::IterNextSlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf)
	{
		TPyObjPtr args(PyTuple_New(0));
		return DispatcherAddress(iSelf, args.get());
	}
};
/////////////////////////////////////////////



/** @intenal
 */
template <typename CppClass>
void addClassStaticMethod(
		const char* iMethodName, const char* iDocumentation,
		PyCFunction iMethodDispatcher, PyCFunction& oOverloadChain)
{
#if PY_VERSION_HEX >= 0x02030000 // >= 2.3
	::std::vector<PyMethodDef>::iterator i = ::std::find_if(
		CppClass::_lassPyClassDef.methods_.begin(), CppClass::_lassPyClassDef.methods_.end(), PyMethodEqual(iMethodName));
	if (i == CppClass::_lassPyClassDef.methods_.end())
	{
		CppClass::_lassPyClassDef.methods_.insert(CppClass::_lassPyClassDef.methods_.begin(), createPyMethodDef(
			iMethodName, iMethodDispatcher, METH_VARARGS | METH_STATIC, iDocumentation));
		oOverloadChain = 0;
	}
	else
	{
		LASS_ASSERT(i->ml_flags == (METH_VARARGS | METH_CLASS));
		oOverloadChain = i->ml_meth;
		i->ml_meth = iMethodDispatcher;
		if (i->ml_doc == 0)
		{
			i->ml_doc = const_cast<char*>(iDocumentation);
		}
	}
#else
	TStaticMembers::iterator i = ::std::find_if(
		CppClass::_lassPyClassDef.statics_.begin(), CppClass::_lassPyClassDef.statics_.end(), StaticMemberEqual(iMethodName));
	if (i == CppClass::_lassPyClassDef.statics_.end())
	{
		PyMethodDef* methodDef(new PyMethodDef(createPyMethodDef(
			iMethodName, iMethodDispatcher, METH_VARARGS, iDocumentation)));
		PyObject* cFunction = PyCFunction_New(methodDef, 0);
		PyObject* descr = PyStaticMethod_New(cFunction);
		CppClass::_lassPyClassDef.statics_.push_back(createStaticMember(
			iMethodName, staticMemberHelperObject(descr)));
		oOverloadChain = 0;
	}
	else
	{
		PyObject* descr = i->object;
		LASS_ASSERT(descr && PyObject_IsInstance(
			descr, reinterpret_cast<PyObject*>(&PyStaticMethod_Type)));		
		PyObject* cFunction = PyStaticMethod_Type.tp_descr_get(descr, 0, 0);
		LASS_ASSERT(cFunction && PyObject_IsInstance(
			cFunction, reinterpret_cast<PyObject*>(&PyCFunction_Type)));
		PyMethodDef* methodDef = reinterpret_cast<PyCFunctionObject*>(cFunction)->m_ml;
		LASS_ASSERT(methodDef && methodDef->ml_flags == METH_VARARGS);
		oOverloadChain = methodDef->ml_meth;
		methodDef->ml_meth = iMethodDispatcher;
		if (methodDef->ml_doc == 0)
		{
			methodDef->ml_doc = const_cast<char*>(iDocumentation);
		}
		i->doc = methodDef->ml_doc;
	}	
#endif
}	


/** @internal
 */
template <typename CppClass, typename T>
void addClassStaticConst(const char* iName, const T& iValue)
{
	LASS_ASSERT(std::count_if(
		CppClass::_lassPyClassDef.statics_.begin(), CppClass::_lassPyClassDef.statics_.end(), StaticMemberEqual(iName)) == 0);
	CppClass::_lassPyClassDef.statics_.push_back(createStaticMember(iName, staticMemberHelperObject(iValue)));
}

}
}
}

// EOF
