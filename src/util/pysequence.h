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
#include "../stde/extended_algorithm.h"

namespace lass
{
namespace python
{
namespace impl
{

	template<typename C>
	struct ContainerTraits
	{
		static const typename C::value_type&		element_at(const C& iC, int i) { return iC[i]; };
		static typename C::value_type&				element_at(C& iC, int i) { return iC[i]; };
		static typename C::const_iterator			const_iterator_at(const C& iC, int i) { return iC.begin()+i; };
		static typename C::iterator					iterator_at(C& iC, int i) { return iC.begin()+i; };
	};

	template<typename C, typename A>
	struct ContainerTraits<std::list<C, A> >
	{
		static const C& element_at(const std::list<C,A>& iC, int i) { return *const_iterator_at(iC,i); };
		static C& element_at(std::list<C,A>& iC, int i) { return *iterator_at(iC,i); };
		static typename std::list<C, A>::const_iterator	const_iterator_at(const std::list<C,A>& iC, int i) 
		{ 
			std::list<C,A>::const_iterator it = iC.begin();
			for (int j=0;j<i;++j)
				++it;
			return it;
		};
		static typename std::list<C, A>::iterator	iterator_at(std::list<C,A>& iC, int i) 
		{ 
			std::list<C,A>::iterator it = iC.begin();
			for (int j=0;j<i;++j)
				++it;
			return it;
		};
	};



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
		virtual PyObject* pop(int i) = 0;
		virtual bool pointsToSameContainer(void* iO) = 0;

	};

	template<typename Container>
	struct ContainerNotOwned
	{
		typedef Container* ContainerPtr;
		static	void dispose(ContainerPtr ioC) {};
	};
	template<typename Container>
	struct ContainerOwned
	{
		typedef Container* ContainerPtr;
		static	void dispose(ContainerPtr ioC) {delete ioC;};
	};

	template<typename Container, typename ContainerOwnerShipPolicy = ContainerNotOwned<Container> > 
	class PySequenceContainer : public PySequenceImplBase
	{
	public:
		PySequenceContainer(typename ContainerOwnerShipPolicy::ContainerPtr iC, bool iReadOnly = false) : cont_(iC), readOnly_(iReadOnly) {}
		virtual ~PySequenceContainer() { ContainerOwnerShipPolicy::dispose(cont_); }
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
		virtual PyObject* pop(int i);
		virtual bool pointsToSameContainer(void* iO) 
		{ 
			return (iO == (void*)cont_);
		}

	private:
		typename ContainerOwnerShipPolicy::ContainerPtr cont_;
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
		template<typename Container> PySequence( Container& iCont ) : PyObjectPlus(&Type)
		{
			pimpl_ = new PySequenceContainer<Container>(&iCont);
			initialize();
		}
		template<typename Container> PySequence( const Container& iCont ) : PyObjectPlus(&Type)
		{
			pimpl_ = new PySequenceContainer<Container>(const_cast<Container*>(&iCont),true);
			initialize();
		}
		//PySequence( PyObject* iP );
		virtual ~PySequence();
		virtual void append(PyObject* i)	{ pimpl_->append(i); }
		virtual PyObject* pop(int i)		{ return pimpl_->pop(i); }


		static int PySequence_Clear(PyObject *iPO);
		//static PyObject* PySequence_ListIter(PyObject* iPO);

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

		template<typename Container> bool pointsToSameContainer(Container& iO) 
		{ 
			return pimpl_->pointsToSameContainer(&iO);
		}
	private:
		PySequence();
		PySequenceImplBase*	pimpl_;
		static void initialize();
	};

	template<typename Container, typename ContainerOwnerShipPolicy>
	int PySequenceContainer<Container, ContainerOwnerShipPolicy>::PySequence_Clear()
	{
		cont_->clear();
		return 0;
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	int PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Length()
	{
		return cont_->size();
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	PyObject* PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Concat(PyObject *bb)
	{
		Container toConcat;
		int r = pyGetSimpleObject(bb,toConcat);
		if (r)
		{
			PyErr_SetString(PyExc_TypeError, "Cannot convert to concatenation type");
			return NULL;
		}
		Container result(*cont_);
		result.insert(result.end(), toConcat.begin(), toConcat.end());
		return pyBuildList<Container>(result.begin(),result.end());
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	PyObject* PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Repeat(int n)
	{
		Container result = stde::repeat(*cont_,n);
		return pyBuildList<Container>(result.begin(),result.end());
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	PyObject* PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Item(int i)
	{
		if (i<0 || i>cont_->size())
		{
			PyErr_SetString(PyExc_IndexError, "Index out of bounds");
			return NULL;
		}
		return pyBuildSimpleObject( ContainerTraits<Container>::element_at(*cont_,i));
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	PyObject* PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Slice(int ilow, int ihigh)
	{
		int i, len;
		if (ilow < 0)
			ilow = 0;
		else if (ilow > cont_->size())
			ilow = cont_->size();
		if (ihigh < ilow)
			ihigh = ilow;
		else if (ihigh > cont_->size())
			ihigh = cont_->size();
		len = ihigh - ilow;
		return pyBuildList<Container>(	ContainerTraits<Container>::const_iterator_at(*cont_,ilow),
										ContainerTraits<Container>::const_iterator_at(*cont_,ilow+len) );
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	int PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_AssItem(int i, PyObject *v)
	{
		if (readOnly_)
		{
			PyErr_SetString(PyExc_TypeError, "Sequence is read-only");
			return NULL;
		}
		PyObject *old_value;
		if (i < 0 || i >= PySequence_Length()) 
		{
			PyErr_SetString(PyExc_IndexError,"list assignment index out of range");
			return -1;
		}
		if (v == NULL)
			return PySequence_AssSlice(i, i+1, v);
		return pyGetSimpleObject(v,ContainerTraits<Container>::element_at(*cont_,i));
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	int PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_AssSlice(int ilow, int ihigh, PyObject *v)
	{
		if (readOnly_)
		{
			PyErr_SetString(PyExc_TypeError, "Sequence is read-only");
			return -1;
		}
		Container temp;
		int r = pyGetSimpleObject(v,temp);
		if (r)
		{
			PyErr_SetString(PyExc_TypeError, "Cannot convert to type for slice assignment");
			return -1;
		}
		cont_->erase(	ContainerTraits<Container>::iterator_at(*cont_,ilow),
						ContainerTraits<Container>::iterator_at(*cont_,ihigh) );
		cont_->insert(	ContainerTraits<Container>::iterator_at(*cont_,ilow+1),
						temp.begin(),temp.end());
		return 0;
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	int PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Contains(PyObject *el)
	{
		Container::value_type temp;
		int r = pyGetSimpleObject(el,temp);
		if (r)
		{
			// type is not convertible and hence is not found
			// this corresponds to the Python behavior
			return 0;
		}
		if (std::find(cont_->begin(),cont_->end(),temp)!=cont_->end())
			return 1;
		return 0;
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	int PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_InplaceConcat(PyObject *other)
	{
		if (readOnly_)
		{
			PyErr_SetString(PyExc_TypeError, "Sequence is read-only");
			return -1;
		}
		Container toConcat;
		int r = pyGetSimpleObject(other,toConcat);
		if (r)
		{
			PyErr_SetString(PyExc_TypeError, "Cannot convert to concatenation type");
			return 1;
		}
		cont_->insert(cont_->end(), toConcat.begin(), toConcat.end());
		return 0;
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	int PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_InplaceRepeat(int n)
	{
		if (readOnly_)
		{
			PyErr_SetString(PyExc_TypeError, "Sequence is read-only");
			return -1;
		}
		stde::inplace_repeat(*cont_,n);
		return 0;
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	void PySequenceContainer<Container,ContainerOwnerShipPolicy>::append(PyObject* i)
	{
		if (readOnly_)
		{
			PyErr_SetString(PyExc_TypeError, "Sequence is read-only");
			return;
		}
		Container::value_type temp;
		int r = pyGetSimpleObject(i,temp);
		if (r)
		{
			PyErr_SetString(PyExc_TypeError, "Cannot convert to append type");
			return;
		}
		cont_->push_back( temp );
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	PyObject* PySequenceContainer<Container,ContainerOwnerShipPolicy>::pop(int i)
	{
		if (readOnly_)
		{
			PyErr_SetString(PyExc_TypeError, "Sequence is read-only");
			return NULL;
		}
		Container::value_type temp = ContainerTraits<Container>::element_at(*cont_,i);
		cont_->erase(ContainerTraits<Container>::iterator_at(*cont_,i));
		return pyBuildSimpleObject(temp);
	}



}

}

}

#endif