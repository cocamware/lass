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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#include "util_common.h"
#include "pyobject_plus.h"
#include "string_cast.h"

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
		virtual PyObject* PyIteratorRange_IterNext() = 0;
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
		virtual PyObject* PyIteratorRange_IterNext();
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
	PyObject* PyIteratorRangeImpl<M>::PyIteratorRange_IterNext()
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

	static PyObject* PyIteratorRange_IterNext( PyObject* iPO) { return static_cast<PyIteratorRange*>(iPO)->pimpl_->PyIteratorRange_IterNext(); }
	
private:
	static void initialize();

	impl::PyIteratorRangeImplBase* pimpl_;
	static bool isInitialized;
};


}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif
