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
 *	Copyright (C) 2004-2024 the Initial Developer.
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
	static PyObject* fun(PyObject* iSelf, PyObject* iOther, PyObject* iThird)
	{
		TPyObjPtr args(Py_BuildValue("(O,O)", iOther, iThird));
		PyObject* result = DispatcherAddress(iSelf, args.get());
		if (!result && PyErr_ExceptionMatches(PyExc_TypeError) && iThird == Py_None)
		{
			// try as a binary function
			PyErr_Clear();
			TPyObjPtr args2(Py_BuildValue("(O)", iOther));
			return DispatcherAddress(iSelf, args2.get());
		}
		return result;	
	}
};
/** @internal
*/
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::SsizeArgSlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf, Py_ssize_t iSize) 
	{
		TPyObjPtr args(Py_BuildValue("(n)", iSize));
		return DispatcherAddress(iSelf,args.get());
	}
};
/** @internal
*/
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::SsizeSsizeArgSlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf, Py_ssize_t iSize, Py_ssize_t iSize2)
	{
		TPyObjPtr args(Py_BuildValue("(n,n)", iSize, iSize2));
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
		TPyObjPtr args(Py_BuildValue("(n,O)",iSize,iOther));
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
		TPyObjPtr args(Py_BuildValue("(n,n,O)",iSize,iSize2,iOther));
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
		if (!temp)
			return -1;
		int result;
		if (pyGetSimpleObject(temp.get(), result) != 0)
			return -1;
		return result;
	}
};

/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::ObjObjArgSlot, DispatcherAddress>
{
	static int fun(PyObject* self, PyObject* key, PyObject* value)
	{
		// value can be NULL in case of __delitem__: del dct[x] is in fact
		// implemented by calling mp_ass_subscript with NULL as value.
		// However, we can't translate NULL back into a valid PyObject*, so
		// we'll just omit the second argument.

		TPyObjPtr args(value
			? Py_BuildValue("(O,O)", key, value)
			: Py_BuildValue("(O)", key));
		TPyObjPtr temp(DispatcherAddress(self, args.get()));
		if (temp)
		{
			LASS_ASSERT(!PyErr_Occurred());
			return 0;
		}
		if (!PyErr_Occurred())
		{
			PyErr_SetString(PyExc_AssertionError, "ObjObjArgSlot: expected return value or exception.");
		}
		return -1;
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
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::ArgKwSlot ,DispatcherAddress>
{
	static PyObject* fun(PyObject* iSelf, PyObject* iArgs, PyObject* LASS_UNUSED(iKw))
	{
		if (!_PyArg_NoKeywords("function", iKw))
		{
			return 0;
		}
		return DispatcherAddress(iSelf, iArgs);
	}
};
/** @internal
 */
template <PyCFunction DispatcherAddress> struct FunctionTypeDispatcher<lass::python::impl::InquirySlot ,DispatcherAddress>
{
	static int fun(PyObject* iSelf)
	{
		TPyObjPtr args(Py_BuildValue("()"));
		TPyObjPtr temp(DispatcherAddress(iSelf,args.get()));
		if (!temp)
			return -1;
		int result;
		if (pyGetSimpleObject(temp.get(), result) != 0)
			return -1;
		return result;
	}
};

/////////////////////////////////////////////

}
}
}

// EOF
