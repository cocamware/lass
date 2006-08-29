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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYITERATORRANGE_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYITERATORRANGE_H

#include "util_common.h"
#include "pyobject_plus.h"
#include "string_cast.h"

namespace lass
{
namespace python
{
namespace impl
{
	/// begin of iterators

	class PyIteratorRangeImplBase
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

class PyIteratorRange : public lass::python::PyObjectPlus
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
	virtual std::string pyStr(void) { return pimpl_->pyStr(); }
	virtual std::string pyRepr(void) { return pimpl_->pyRepr(); }

	static PyObject* PyIteratorRange_IterNext( PyObject* iPO) { return static_cast<PyIteratorRange*>(iPO)->pimpl_->PyIteratorRange_IterNext(); }
	
private:
	impl::PyIteratorRangeImplBase* pimpl_;
	static void initialize();
	static bool isInitialized;
};


}

}

#endif
