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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSEQUENCE_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSEQUENCE_H

#include "util_common.h"
#include "pyobject_plus.h"
#include "pyobject_util.h"
#include "../stde/extended_algorithm.h"
#include "string_cast.h"

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
		static void reserve(C& iC, int iAmount)		{};
	};

	template<typename C, typename A>
	struct ContainerTraits<std::vector<C, A> > 
	{
		static const typename std::vector<C, A>::value_type&		element_at(const std::vector<C, A>& iC, int i) { return iC[i]; };
		static typename std::vector<C, A>::value_type&				element_at(std::vector<C, A>& iC, int i) { return iC[i]; };
		static typename std::vector<C, A>::const_iterator			const_iterator_at(const std::vector<C, A>& iC, int i) { return iC.begin()+i; };
		static typename std::vector<C, A>::iterator					iterator_at(std::vector<C, A>& iC, int i) { return iC.begin()+i; };
		static void reserve(std::vector<C, A>& iC, int iAmount)		{iC.reserve(iAmount);};
	};


	template<typename C, typename A>
	struct ContainerTraits<std::list<C, A> >
	{
		static const C& element_at(const std::list<C,A>& iC, int i) { return *const_iterator_at(iC,i); };
		static C& element_at(std::list<C,A>& iC, int i) { return *iterator_at(iC,i); };
		static typename std::list<C, A>::const_iterator	const_iterator_at(const std::list<C,A>& iC, int i) 
		{ 
			typename std::list<C,A>::const_iterator it = iC.begin();
			for (int j=0;j<i;++j)
				++it;
			return it;
		};
		static typename std::list<C, A>::iterator	iterator_at(std::list<C,A>& iC, int i) 
		{ 
			typename std::list<C,A>::iterator it = iC.begin();
			for (int j=0;j<i;++j)
				++it;
			return it;
		};
		static void reserve(std::list<C, A>& iC, int iAmount)		{};
	};



	class PySequenceImplBase
	{
	public:
		PySequenceImplBase() {};
		virtual ~PySequenceImplBase() {};
		virtual void clear() = 0;
		virtual void reserve(int iAmount) = 0;


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
		virtual std::string pyStr(void) = 0;
		virtual std::string pyRepr(void) = 0;
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
		virtual void clear();
		virtual void reserve(int iAmount);

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
		virtual std::string pyStr(void);
		virtual std::string pyRepr(void);
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
		virtual void clear()				{ pimpl_->clear(); }
		virtual void reserve(int iAmount)	{ pimpl_->reserve(iAmount); }
		virtual PyObject* pop(int i)		{ return pimpl_->pop(i); }
		virtual std::string pyStr(void)		{ return pimpl_->pyStr(); }
		virtual std::string pyRepr(void)	{ return pimpl_->pyRepr(); }

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
	void PySequenceContainer<Container, ContainerOwnerShipPolicy>::clear()
	{
		cont_->clear();
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	void PySequenceContainer<Container, ContainerOwnerShipPolicy>::reserve(int iAmount)
	{
		ContainerTraits<Container>::reserve(*cont_,iAmount);
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	int PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Length()
	{
		const int size = static_cast<int>(cont_->size());
		LASS_ASSERT(size >= 0);
		return size;
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
		return pyBuildList(result.begin(),result.end());
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	PyObject* PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Repeat(int n)
	{
		Container result = stde::repeat_c(*cont_,n);
		return pyBuildList(result.begin(),result.end());
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	PyObject* PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Item(int i)
	{
		const int size = static_cast<int>(cont_->size());
		LASS_ASSERT(size >= 0);

		if (i<0 || i>=size)
		{
			PyErr_SetString(PyExc_IndexError, "Index out of bounds");
			return NULL;
		}
		return pyBuildSimpleObject( ContainerTraits<Container>::element_at(*cont_,i));
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	PyObject* PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Slice(int ilow, int ihigh)
	{
		const int size = static_cast<int>(cont_->size());
		LASS_ASSERT(size >= 0);

		int len;
		if (ilow < 0)
			ilow = 0;
		else if (ilow > size)
			ilow = size;
		if (ihigh < ilow)
			ihigh = ilow;
		else if (ihigh > size)
			ihigh = size;
		len = ihigh - ilow;
		return pyBuildList(	ContainerTraits<Container>::const_iterator_at(*cont_,ilow),
							ContainerTraits<Container>::const_iterator_at(*cont_,ilow+len) );
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	int PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_AssItem(int i, PyObject *v)
	{
		if (readOnly_)
		{
			PyErr_SetString(PyExc_TypeError, "Sequence is read-only");
			return -1;
		}
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
		typename Container::value_type temp;
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
		stde::inplace_repeat_c(*cont_,n);
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
		typename Container::value_type temp;
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
		typename Container::value_type temp = ContainerTraits<Container>::element_at(*cont_,i);
		cont_->erase(ContainerTraits<Container>::iterator_at(*cont_,i));
		return pyBuildSimpleObject(temp);
	}

	template<typename Container, typename ContainerOwnerShipPolicy>
	std::string PySequenceContainer<Container,ContainerOwnerShipPolicy>::pyStr( void)
	{
		return pyRepr();
	}

	template<typename Container, typename ContainerOwnerShipPolicy>
	std::string PySequenceContainer<Container,ContainerOwnerShipPolicy>::pyRepr( void)
	{
		return util::stringCast<std::string>(*cont_);
	}


	template <typename Sequence>
	int pyGetSequenceObject( PyObject* iValue, Sequence& oV )
	{
		if (!PySequence_Check(iValue))
		{
			PyErr_SetString(PyExc_TypeError, "not a python sequence");
			return 1;
		}
		// check if we have our own PySequence object, then take a shortcut
		if (isOfType( iValue, &PySequence::Type ) && ((PySequence*)iValue)->pointsToSameContainer(oV))
		{
			return 0;
		}
		else
		{
			Sequence result;
			const int size = PySequence_Length(iValue);
			for (int i = 0; i < size; ++i)
			{
				typename Sequence::value_type temp;
				if (pyGetSimpleObject( PySequence_ITEM(iValue, i) , temp ) != 0)
				{
					impl::addMessageHeader(
						std::string("sequence element ") + util::stringCast<std::string>(i));
					return 1;
				}
				result.push_back( temp );
			}
			oV.swap(result);
		}
		return 0;
	}


}

/** @ingroup Python
	*  build a copy of a std::vector as a Python list
	*  @note you build a reference to the std::vector, but the list is read-only
	*/
template< class V, typename A>
PyObject* pyBuildSimpleObject( const std::vector<V, A>& iV )
{
	return impl::pyBuildList(iV.begin(),iV.end() );
}

/** @ingroup Python
	*  build a copy of a std::vector as a Python ;ost
	*  @note you build a reference to the std::vector, any changes done in Python
	*  will be reflected in the original object, as far as the typesystem allows it of course
	*/
template<class V, typename A>
PyObject* pyBuildSimpleObject( std::vector<V, A>& iV )
{
	return new impl::PySequence( iV );
}

/** @ingroup Python
	*  build a copy of a std::list as a Python list
	*  @note you build a reference to the std::list, but the list is read-only
	*/
template< class V, typename A>
PyObject* pyBuildSimpleObject( const std::list<V, A>& iV )
{
	return impl::pyBuildList(iV.begin(),iV.end() );
}
/** @ingroup Python
	*  build a copy of a std::list as a Python ;ost
	*  @note you build a reference to the std::list, any changes done in Python
	*  will be reflected in the original object, as far as the typesystem allows it of course
	*/
template<class V, typename A>
PyObject* pyBuildSimpleObject( std::list<V, A>& iV )
{
	return new impl::PySequence( iV );
}

/** @ingroup Python
	*  build a copy of a std::queue as a Python list
	*  @note you build a reference to the std::queue, but the list is read-only
	*/
/*
template< class V, typename A>
PyObject* pyBuildSimpleObject( const std::queue<V, A>& iV )
{
	return new impl::PySequence( iV, true );
}
*/
/** @ingroup Python
	*  build a copy of a std::queue as a Python ;ost
	*  @note you build a reference to the std::queue, any changes done in Python
	*  will be reflected in the original object, as far as the typesystem allows it of course
	*/
/*
template<class V, class A>
PyObject* pyBuildSimpleObject( std::queue<V, A>& iV )
{
	return new impl::PySequence( iV );
}
*/
/** @ingroup Python
	*  build a copy of a std::deque as a Python list
	*  @note you build a reference to the std::deque, but the list is read-only
	*/
template< class V, class A>
PyObject* pyBuildSimpleObject( const std::deque<V,A>& iV )
{
	return impl::pyBuildList(iV.begin(),iV.end() );
}
/** @ingroup Python
	*  build a copy of a std::deque as a Python ;ost
	*  @note you build a reference to the std::deque, any changes done in Python
	*  will be reflected in the original object, as far as the typesystem allows it of course
	*/
template<typename V,typename A>
PyObject* pyBuildSimpleObject( std::deque<V,A>& iV )
{
	return new impl::PySequence( iV );
}


/** @ingroup Python
	*  get a copy of a Python sequence as a std::vector.
	*  @note you get a COPY of the sequence, not the original sequence itself!
	*/
template<class C, typename A>
int pyGetSimpleObject( PyObject* iValue, std::vector<C, A>& oV )
{
	return impl::pyGetSequenceObject( iValue, oV );
}

/** @ingroup Python
	*  get a copy of a Python sequence as a std::list.
	*  @note you get a COPY of the sequence, not the original sequence itself!
	*/
template<class C, typename A>
int pyGetSimpleObject( PyObject* iValue, std::list<C, A>& oV )
{
	return impl::pyGetSequenceObject( iValue, oV );
}

/** @ingroup Python
	*  get a copy of a Python sequence as a std::queue.
	*  @note you get a COPY of the sequence, not the original sequence itself!
	*/
/*
template<class C, typename A>
int pyGetSimpleObject( PyObject* iValue, std::queue<C, A>& oV )
{
	return impl::pyGetSequenceObject( iValue, oV );
}
*/

/** @ingroup Python
	*  get a copy of a Python sequence as a std::deque.
	*  @note you get a COPY of the sequence, not the original sequence itself!
	*/
template<class C, typename A>
int pyGetSimpleObject( PyObject* iValue, std::deque<C, A>& oV )
{
	return impl::pyGetSequenceObject( iValue, oV );
}


}

}

#endif
