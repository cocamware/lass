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

/** @defgroup Python
 *  @brief interface library to Python
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYITERATORRANGE_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYITERATORRANGE_H

#include "python_common.h"
#include "pyobject_plus.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4244)
#endif

namespace lass
{
namespace python
{
namespace impl
{
	/// begin of iterators

	class LASS_DLL PyIteratorRangeImplBase
	{
	public:
		PyIteratorRangeImplBase() {};
		virtual ~PyIteratorRangeImplBase() {};
		virtual int PyIteratorRange_Length() const = 0;
		virtual std::string pyStr(void) const = 0;
		virtual std::string pyRepr(void) const = 0;
		virtual PyObject* iterNext() = 0;
	};

	template<typename M> 
	class PyIteratorRangeImpl : public PyIteratorRangeImplBase
	{
	public:
		PyIteratorRangeImpl(M iBegin, M iEnd) : beginIt_(iBegin), endIt_(iEnd), curIt_(iBegin)  {}
		virtual ~PyIteratorRangeImpl() {}
		virtual int PyIteratorRange_Length() const;
		virtual std::string pyStr(void) const;
		virtual std::string pyRepr(void) const;
		virtual PyObject* iterNext();
	private:
		M beginIt_;
		M endIt_;
		M curIt_;
	};

	template<typename M>
	int PyIteratorRangeImpl<M>::PyIteratorRange_Length() const
	{
		return std::distance(beginIt_,endIt_);
	}
	template<typename M>
	std::string PyIteratorRangeImpl<M>::pyStr() const
	{
		return "PyIteratorRangeImpl<M>::str";
	}
	template<typename M>
	std::string PyIteratorRangeImpl<M>::pyRepr() const
	{
		return "PyIteratorRangeImpl<M>::repr";
	}
	template<typename M>
	PyObject* PyIteratorRangeImpl<M>::iterNext()
	{
		if (curIt_!=endIt_)
			return pyBuildSimpleObject(*curIt_++);
		// according to python specs this is allowed and is equivalent of setting the 
		// stopiteration exception
		return NULL;	
	}
}

class LASS_DLL PyIteratorRange : public lass::python::PyObjectPlus
{
	PY_HEADER(PyObjectPlus);
public:
	template<typename CI> PyIteratorRange( CI iBegin, CI iEnd ) 
	{
		initialize();
		impl::fixObjectType(this);
		pimpl_ = new impl::PyIteratorRangeImpl<CI>(iBegin,iEnd);
	}

	virtual ~PyIteratorRange() {}
	virtual std::string doPyStr(void) { return pimpl_->pyStr(); }
	virtual std::string doPyRepr(void) { return pimpl_->pyRepr(); }

	static PyObject* iter( PyObject* iPo) { Py_XINCREF(iPo); return iPo; }
	static PyObject* iterNext( PyObject* iPO) { return static_cast<PyIteratorRange*>(iPO)->pimpl_->iterNext(); }
	
private:
	static void initialize();

	impl::PyIteratorRangeImplBase* pimpl_;
	static bool isInitialized;
};

template<>
struct PyExportTraits<PyIteratorRange*>
{
	/** we take ownership! */
	static PyObject* build( PyIteratorRange* iV )
	{
		return iV;
	}
};


}

}

// --- iterators -------------------------------------------------------------------------------------

/** @ingroup Python
 *  Exports a set of C++ iterators to Python
 *
 *  @param t_cppClass
 *      the C++ class you're exporting an iterator of
 *  @param i_cppBegin
 *      the name of the method in C++ that provides the beginning of the exported iterator range 
 *  @param i_cppEnd
 *      the name of the method in C++ that provides the beginning of the exported iterator range 
 *  @param s_doc
 *      documentation of method as shown in Python (zero terminated C string)
 *  @param i_dispatcher
 *      A unique name of the static C++ dispatcher function to be generated.  This name will be
 *      used for the names of automatic generated variables and functions and should be unique
 *      per exported C++ class/method pair.
 *
 *  Invoke this macro to export an iterator range to python.  The returned object will support the 
 *	iterator protocol by default.  The class generating the export will also support the iterator
 *	generator protocol.
 *
 *  @note
 *      the documentation of the Python method will be the @a s_doc of the first exported
 *      overload.
 *  @note
 *      the iterator stability of C++ is inherited into Python.  It is the responsibility of the user
 *		of these macro's to ensure or document the stability of the iterators.
 *  @note
 *      Although you can overload the __iter__ slot it is probably of little use.  The last assigned function
 *		will return the iterator.  It could be useful if some of the overloaded function throw, in that case
 *		the first non-throwing (if any) function will be chosen.
 *
 *  @code
 *  // foo.h
 *  class Foo
 *  {
 *      PY_HEADER(python::PyObjectPlus)
 *  public:
 *      void barA(int a);
 *      std::vector<int>::const_iterator begin() const { return vector_.begin(); }
 *      std::vector<int>::const_iterator end() const { return vector_.end(); }
 *  private:
 *		std::vector<int> vector_;
 *  };
 *
 *  std::vector<int> temp;
 *  std::vector<int>::const_iterator freeBegin(Foo& iObj) { return temp.begin(); }
 *  std::vector<int>::const_iterator freeEnd(Foo& iObj) { return temp.end(); }
 *
 *  // foo.cpp
 *  PY_DECLARE_CLASS(Foo)
 *  PY_CLASS_ITERFUNC_EX(Foo, begin, end, 0, foo_bar_a)
 *  PY_CLASS_FREE_ITERFUNC_EX(Foo, begin, end, 0, foo_bar_a)
 *  @endcode
 */
#define PY_CLASS_ITERFUNC_EX( t_cppClass, i_cppBegin, i_cppEnd, s_doc, i_dispatcher )\
	lass::python::PyIteratorRange* LASS_CONCATENATE_3( lassPyImpl_method_, i_dispatcher, itDispatch1 ) (::lass::python::impl::ShadowTraits< t_cppClass >::TCppClass& iObj) { \
	return new lass::python::PyIteratorRange(iObj.i_cppBegin (), iObj.i_cppEnd ()); } \
	PY_CLASS_FREE_METHOD_NAME_DOC( t_cppClass, LASS_CONCATENATE_3( lassPyImpl_method_, i_dispatcher, itDispatch1 ), lass::python::methods::_iter_, s_doc)
/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_ITERFUNC_EX with
 *  @a i_dispatcher = lassPyImpl_method_ ## @a i_cppClass ## __LINE__.
 */
#define PY_CLASS_ITERFUNC_DOC( i_cppClass, i_cppBegin, icppEnd, s_doc )\
		PY_CLASS_ITERFUNC_EX(\
			i_cppClass, i_cppBegin, icppEnd, s_doc,\
			LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_ITERFUNC_DOC with @a s_doc = 0.
 */
#define PY_CLASS_ITERFUNC( i_cppClass, i_cppBegin, icppEnd )\
		PY_CLASS_ITERFUNC_DOC( i_cppClass, i_cppBegin, icppEnd, 0 )

#define PY_CLASS_FREE_ITERFUNC_EX( t_cppClass, i_cppBegin, i_cppEnd, s_doc, i_dispatcher )\
lass::python::PyIteratorRange* LASS_CONCATENATE_3( lassPyImpl_method_, i_dispatcher, itDispatch2 ) (::lass::python::impl::ShadowTraits< t_cppClass >::TCppClass& iObj) { \
return new lass::python::PyIteratorRange(i_cppBegin(iObj), i_cppEnd(iObj)); } \
	PY_CLASS_FREE_METHOD_NAME_DOC( t_cppClass, LASS_CONCATENATE_3( lassPyImpl_method_, i_dispatcher, itDispatch2 ), lass::python::methods::_iter_, s_doc)
/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_ITERFUNC_EX with
 *  @a i_dispatcher = lassPyImpl_method_ ## @a i_cppClass ## __LINE__.
 */
#define PY_CLASS_FREE_ITERFUNC_DOC( i_cppClass, i_cppBegin, icppEnd, s_doc )\
		PY_CLASS_FREE_ITERFUNC_EX(\
			i_cppClass, i_cppBegin, icppEnd, s_doc,\
			LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
/** @ingroup Python
 *  convenience macro, wraps PY_CLASS_ITERFUNC_DOC with @a s_doc = 0.
 */
#define PY_CLASS_FREE_ITERFUNC( i_cppClass, i_cppBegin, icppEnd )\
		PY_CLASS_FREE_ITERFUNC_DOC( i_cppClass, i_cppBegin, icppEnd, 0 )

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif
