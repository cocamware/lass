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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSEQUENCE_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSEQUENCE_H

#include "util_common.h"
#include "pyobject_plus.h"

namespace lass
{
namespace python
{
namespace impl
{

	class PySequenceImplBase
	{
	public:
		PySequenceImplBase() {};
		virtual ~PySequenceImplBase() {};
		virtual int PySequence_Clear() = 0;

		virtual int PySequence_Length() = 0;
		virtual PyObject* PySequence_Concat(PyObject *bb) = 0;
		virtual PyObject* PySequence_Repeat(int n) = 0;
		virtual PyObject* PySequence_Item(int i) = 0;
		virtual PyObject* PySequence_Slice(int ilow, int ihigh) = 0;
		virtual int PySequence_AssItem(int i, PyObject *v) = 0;
		virtual int PySequence_AssSlice(int ilow, int ihigh, PyObject *v) = 0;
		virtual int PySequence_Contains(PyObject *el) = 0;
		virtual int PySequence_InplaceConcat(PyObject *other) = 0;
		virtual int PySequence_InplaceRepeat(int n) = 0;
		virtual void append(PyObject* i) = 0;
	};


	template<typename V,typename A> 
	class PySequenceVec : public PySequenceImplBase
	{
	public:
		PySequenceVec(std::vector<V,A>& iVec, bool iReadOnly = false) : vector_(iVec), readOnly_(iReadOnly) {}
		virtual ~PySequenceVec() {}

		virtual int PySequence_Clear();

		virtual int PySequence_Length();
		virtual PyObject* PySequence_Concat(PyObject *bb);
		virtual PyObject* PySequence_Repeat(int n);
		virtual PyObject* PySequence_Item(int i);
		virtual PyObject* PySequence_Slice(int ilow, int ihigh);
		virtual int PySequence_AssItem(int i, PyObject *v);
		virtual int PySequence_AssSlice(int ilow, int ihigh, PyObject *v);
		virtual int PySequence_Contains(PyObject *el);
		virtual int PySequence_InplaceConcat(PyObject *other);
		virtual int PySequence_InplaceRepeat(int n);
		virtual void append(PyObject* i);

	private:
		std::vector<V,A>& vector_;
		bool readOnly_;
	};

	/** PySequence.  Object for interfacing sequence-like objects with Python 
	*/
	class LASS_DLL PySequence : public PyObjectPlus
	{
		PY_HEADER(PyObjectPlus);
		static PySequenceMethods pySequenceMethods;
		static bool isInitialized;

	public:
		template<typename V, typename A> PySequence( std::vector<V, A>& iStdVec ) : PyObjectPlus(&Type)
		{
			pimpl_ = new PySequenceVec<V,A>(iStdVec);
			this->ob_type->tp_as_sequence= &pySequenceMethods;
			initialize();
		}
		template<typename V, typename A> PySequence( const std::vector<V, A>& iStdVec ) : PyObjectPlus(&Type)
		{
			pimpl_ = new PySequenceVec<V,A>(iStdVec, true);
			this->ob_type->tp_as_sequence= &pySequenceMethods;
			initialize();
		}


		virtual ~PySequence();
		virtual void append(PyObject* i) { pimpl_->append(i); }

		static int PySequence_Clear(PyObject *iPO);
		static PyObject* PySequence_ListIter(PyObject* iPO);

		static int PySequence_Length( PyObject* iPO);
		static PyObject* PySequence_Concat(PyObject *a, PyObject *bb);
		static PyObject* PySequence_Repeat(PyObject *a, int n);
		static PyObject* PySequence_Item(PyObject*a, int i);
		static PyObject* PySequence_Slice(PyObject *a, int ilow, int ihigh);
		static int PySequence_AssItem(PyObject *a, int i, PyObject *v);
		static int PySequence_AssSlice(PyObject *a, int ilow, int ihigh, PyObject *v);
		static int PySequence_Contains(PyObject *a, PyObject *el);
		static PyObject * PySequence_InplaceConcat(PyObject *self, PyObject *other);
		static PyObject * PySequence_InplaceRepeat(PyObject *self, int n);

	private:
		PySequence();
		PySequenceImplBase*	pimpl_;
		static void initialize();
	};

	template<typename V,typename A>
	int PySequenceVec<V,A>::PySequence_Clear()
	{
		vector_.clear();
		return 0;
	}
	template<typename V,typename A>
	int PySequenceVec<V,A>::PySequence_Length()
	{
		return vector_.size();
	}
	template<typename V,typename A>
	PyObject* PySequenceVec<V,A>::PySequence_Concat(PyObject *bb)
	{
		std::vector<V> toConcat;
		int r = pyGetSimpleObject(bb,toConcat);
		if (r)
		{
			PyErr_SetString(PyExc_TypeError, "Cannot convert to concatenation type");
			return NULL;
		}
		std::vector<V> result(vector_);
		result.insert(result.end(), toConcat.begin(), toConcat.end());
		return pyBuildList<std::vector<V,A> >(result.begin(),result.end());
	}
	template<typename V,typename A>
	PyObject* PySequenceVec<V,A>::PySequence_Repeat(int n)
	{
		std::vector<V> result;
		for (int i=0;i<n;++i)
			result.insert(result.end(), vector_.begin(), vector_.end());
		return pyBuildList<std::vector<V,A> >(result.begin(),result.end());
	}
	template<typename V,typename A>
	PyObject* PySequenceVec<V,A>::PySequence_Item(int i)
	{
		if (i<0 || i>vector_.size())
		{
			PyErr_SetString(PyExc_IndexError, "Index out of bounds");
			return NULL;
		}
		return pyBuildSimpleObject(vector_[i]);
	}
	template<typename V,typename A>
	PyObject* PySequenceVec<V,A>::PySequence_Slice(int ilow, int ihigh)
	{
		int i, len;
		if (ilow < 0)
			ilow = 0;
		else if (ilow > vector_.size())
			ilow = vector_.size();
		if (ihigh < ilow)
			ihigh = ilow;
		else if (ihigh > vector_.size())
			ihigh = vector_.size();
		len = ihigh - ilow;
		return static_cast<PyObject*>(pyBuildList<std::vector<V,A> >(vector_.begin()+ilow,vector_.end()+ilow+len));
	}
	template<typename V,typename A>
	int PySequenceVec<V,A>::PySequence_AssItem(int i, PyObject *v)
	{
		PyObject *old_value;
		if (i < 0 || i >= PySequence_Length()) 
		{
			PyErr_SetString(PyExc_IndexError,"list assignment index out of range");
			return -1;
		}
		if (v == NULL)
			return PySequence_AssSlice(i, i+1, v);
		return pyGetSimpleObject(v,vector_[i]);
	}
	template<typename V,typename A>
	int PySequenceVec<V,A>::PySequence_AssSlice(int ilow, int ihigh, PyObject *v)
	{
		std::vector<V> temp;
		int r = pyGetSimpleObject(v,temp);
		if (r)
		{
			PyErr_SetString(PyExc_TypeError, "Cannot convert to type for slice assignment");
			return -1;
		}
		vector_.erase(vector_.begin()+ilow,vector_.begin()+ihigh);
		vector_.insert(vector_.begin()+ilow+1,temp.begin(),temp.end());
		return 0;
	}
	template<typename V,typename A>
	int PySequenceVec<V,A>::PySequence_Contains(PyObject *el)
	{

		V temp;
		int r = pyGetSimpleObject(el,temp);
		if (r)
		{
			// type is not convertible and hence is not found
			// this corresponds to the Python behavior
			return 0;
		}
		for (int i = 0; i < vector_.size(); ++i)
		{
			if ( vector_[i]==temp )
				return 1;
		}
		return 0;
	}
	template<typename V,typename A>
	int PySequenceVec<V,A>::PySequence_InplaceConcat(PyObject *other)
	{
		std::vector<V> toConcat;
		int r = pyGetSimpleObject(other,toConcat);
		if (r)
		{
			PyErr_SetString(PyExc_TypeError, "Cannot convert to concatenation type");
			return 1;
		}
		vector_.insert(vector_.end(), toConcat.begin(), toConcat.end());
		return 0;
	}
	template<typename V,typename A>
	int PySequenceVec<V,A>::PySequence_InplaceRepeat(int n)
	{
		int l = vector_.size();
		for (int i=0;i<(n-1);++i)
			for (int j=0;j<l;++j)
                vector_.push_back(vector_[j]);
		return 0;
	}
	template<typename V,typename A>
	void PySequenceVec<V,A>::append(PyObject* i)
	{
		V temp;
		int r = pyGetSimpleObject(i,temp);
		if (r)
		{
			PyErr_SetString(PyExc_TypeError, "Cannot convert to append type");
			return;
		}
		vector_.push_back( temp );
	}



}

}

}

#endif