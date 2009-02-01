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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSEQUENCE_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSEQUENCE_H

#include "python_common.h"
#include "pyobject_plus.h"
#include "pyobject_util.h"
#include "../util/string_cast.h"
#include "../stde/extended_algorithm.h"

#include <vector>
#include <list>
#include <deque>
#include "../stde/static_vector.h"

namespace lass
{
namespace python
{

namespace impl
{

	template<typename C>
	struct ContainerTraits
	{
		static const typename C::value_type&		element_at(const C& iC, Py_ssize_t i) { return iC[i]; };
		static typename C::value_type&				element_at(C& iC, Py_ssize_t i) { return iC[i]; };
		static typename C::const_iterator			const_iterator_at(const C& iC, Py_ssize_t i) { return iC.begin()+i; };
		static typename C::iterator					iterator_at(C& iC, Py_ssize_t i) { return iC.begin()+i; };
		static void reserve(C& /*iC*/, int /*iAmount*/)		{};
	};

	template<typename C, typename A>
	struct ContainerTraits<std::vector<C, A> > 
	{
		static const typename std::vector<C, A>::value_type&		element_at(const std::vector<C, A>& iC, Py_ssize_t i) { return iC[i]; };
		static typename std::vector<C, A>::value_type&				element_at(std::vector<C, A>& iC, Py_ssize_t i) { return iC[i]; };
		static typename std::vector<C, A>::const_iterator			const_iterator_at(const std::vector<C, A>& iC, Py_ssize_t i) { return iC.begin()+i; };
		static typename std::vector<C, A>::iterator					iterator_at(std::vector<C, A>& iC, Py_ssize_t i) { return iC.begin()+i; };
		static void reserve(std::vector<C, A>& iC, int iAmount)		{iC.reserve(iAmount);};
	};


	template<typename C, typename A>
	struct ContainerTraits<std::list<C, A> >
	{
		static const C& element_at(const std::list<C,A>& iC, Py_ssize_t i) { return *const_iterator_at(iC,i); };
		static C& element_at(std::list<C,A>& iC, Py_ssize_t i) { return *iterator_at(iC,i); };
		static typename std::list<C, A>::const_iterator	const_iterator_at(const std::list<C,A>& iC, Py_ssize_t i) 
		{ 
			typename std::list<C,A>::const_iterator it = iC.begin();
			for (Py_ssize_t j=0;j<i;++j)
				++it;
			return it;
		};
		static typename std::list<C, A>::iterator	iterator_at(std::list<C,A>& iC, Py_ssize_t i) 
		{ 
			typename std::list<C,A>::iterator it = iC.begin();
			for (Py_ssize_t j=0;j<i;++j)
				++it;
			return it;
		};
		static void reserve(std::list<C, A>& /*iC*/, int /*iAmount*/)		{};
	};



	class LASS_DLL PySequenceImplBase
	{
	public:
		PySequenceImplBase() {};
		virtual ~PySequenceImplBase() {};
		virtual void clear() = 0;
		virtual void reserve(int iAmount) = 0;


		virtual Py_ssize_t PySequence_Length() = 0;
		virtual PyObject* PySequence_Concat(PyObject *bb) = 0;
		virtual PyObject* PySequence_Repeat(Py_ssize_t n) = 0;
		virtual PyObject* PySequence_Item(Py_ssize_t i) = 0;
		virtual PyObject* PySequence_Slice(Py_ssize_t ilow, Py_ssize_t ihigh) = 0;
		virtual int PySequence_AssItem(Py_ssize_t i, PyObject *v) = 0;
		virtual int PySequence_AssSlice(Py_ssize_t ilow, Py_ssize_t ihigh, PyObject *v) = 0;
		virtual int PySequence_Contains(PyObject *el) = 0;
		virtual int PySequence_InplaceConcat(PyObject *other) = 0;
		virtual int PySequence_InplaceRepeat(Py_ssize_t n) = 0;
		virtual void append(const TPyObjPtr& i) = 0;
		virtual TPyObjPtr pop(int i) = 0;
		virtual bool pointsToSameContainer(void* iO) = 0;
		virtual std::string pyStr(void) = 0;
		virtual std::string pyRepr(void) = 0;
	};

	template<typename Container>
	struct ContainerNotOwned
	{
		typedef Container* ContainerPtr;
		static	void dispose(ContainerPtr /*ioC*/) {};
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

		virtual Py_ssize_t PySequence_Length();
		virtual PyObject* PySequence_Concat(PyObject *bb);
		virtual PyObject* PySequence_Repeat(Py_ssize_t n);
		virtual PyObject* PySequence_Item(Py_ssize_t i);
		virtual PyObject* PySequence_Slice(Py_ssize_t ilow, Py_ssize_t ihigh);
		virtual int PySequence_AssItem(Py_ssize_t i, PyObject *v);
		virtual int PySequence_AssSlice(Py_ssize_t ilow, Py_ssize_t ihigh, PyObject *v);
		virtual int PySequence_Contains(PyObject *el);
		virtual int PySequence_InplaceConcat(PyObject *other);
		virtual int PySequence_InplaceRepeat(Py_ssize_t n);
		virtual void append(const TPyObjPtr& i);
		virtual TPyObjPtr pop(int i);
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
		template<typename Container> PySequence( Container& iCont )
		{
			initialize();
			impl::fixObjectType(this);
			pimpl_ = new PySequenceContainer<Container>(&iCont);
		}
		template<typename Container> PySequence( const Container& iCont ) 
		{
			initialize();
			impl::fixObjectType(this);
			pimpl_ = new PySequenceContainer<Container>(const_cast<Container*>(&iCont),true);
		}
		//PySequence( PyObject* iP );
		~PySequence();
		std::string doPyStr(void)		{ return pimpl_->pyStr(); }
		std::string doPyRepr(void)	{ return pimpl_->pyRepr(); }
		void append(const TPyObjPtr& i)	{ pimpl_->append(i); }
		void clear()				{ pimpl_->clear(); }
		void reserve(int iAmount)	{ pimpl_->reserve(iAmount); }
		TPyObjPtr pop(int i)		{ return pimpl_->pop(i); }
		TPyObjPtr popwo()		{ return pimpl_->pop(-1); }

		//static PyObject* PySequence_ListIter(PyObject* iPO);

		static Py_ssize_t PySequence_Length( PyObject* iPO);
		static PyObject* PySequence_Concat(PyObject *a, PyObject *bb);
		static PyObject* PySequence_Repeat(PyObject *a, Py_ssize_t n);
		static PyObject* PySequence_Item(PyObject*a, Py_ssize_t i);
		static PyObject* PySequence_Slice(PyObject *a, Py_ssize_t ilow, Py_ssize_t ihigh);
		static int PySequence_AssItem(PyObject *a, Py_ssize_t i, PyObject *v);
		static int PySequence_AssSlice(PyObject *a, Py_ssize_t ilow, Py_ssize_t ihigh, PyObject *v);
		static int PySequence_Contains(PyObject *a, PyObject *el);
		static PyObject * PySequence_InplaceConcat(PyObject *self, PyObject *other);
		static PyObject * PySequence_InplaceRepeat(PyObject *self, Py_ssize_t n);

		template<typename Container> bool pointsToSameContainer(Container& iO) 
		{ 
			return pimpl_->pointsToSameContainer(&iO);
		}
	private:
		PySequence();
		PySequenceImplBase*	pimpl_;
		static void initialize();
	};

	template <typename Sequence>
	int pyGetSequenceObject( PyObject* iValue, Sequence& oV )
	{
		if (!PySequence_Check(iValue))
		{
			PyErr_SetString(PyExc_TypeError, "not a python sequence");
			return 1;
		}
		// check if we have our own PySequence object, then take a shortcut
		if (isOfType(iValue, PySequence::_lassPyGetStaticType()) && ((PySequence*)iValue)->pointsToSameContainer(oV))
		{
			return 0;
		}
		else
		{
			Sequence result;
			const Py_ssize_t size = PySequence_Length(iValue);
			for (Py_ssize_t i = 0; i < size; ++i)
			{
				typename Sequence::value_type temp;
				if (PyExportTraits<typename Sequence::value_type>::get( PySequence_ITEM(iValue, i) , temp ) != 0)
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
	Py_ssize_t PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Length()
	{
		const Py_ssize_t size = static_cast<Py_ssize_t>(cont_->size());
		LASS_ASSERT(size >= 0);
		return size;
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	PyObject* PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Concat(PyObject *bb)
	{
		Container toConcat;
		int r = pyGetSequenceObject(bb,toConcat);
		if (r)
		{
			PyErr_SetString(PyExc_TypeError, "Cannot convert to concatenation type");
			return NULL;
		}
		Container result(*cont_);
		result.insert(result.end(), toConcat.begin(), toConcat.end());
		return fromSharedPtrToNakedCast(pyBuildList(result.begin(),result.end()));
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	PyObject* PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Repeat(Py_ssize_t n)
	{
		Container result = stde::repeat_c(*cont_,n);
		return fromSharedPtrToNakedCast(pyBuildList(result.begin(),result.end()));
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	PyObject* PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Item(Py_ssize_t i)
	{
		const Py_ssize_t size = static_cast<Py_ssize_t>(cont_->size());
		LASS_ASSERT(size >= 0);

		if (i>=size)
		{
			PyErr_SetString(PyExc_IndexError, "Index out of bounds");
			return NULL;
		}
		if (i<0)
			i = (size-(-i % size)) % size;
		return lass::python::PyExportTraits< typename Container::value_type >::build( ContainerTraits<Container>::element_at(*cont_,i));
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	PyObject* PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_Slice(Py_ssize_t ilow, Py_ssize_t ihigh)
	{
		const Py_ssize_t size = static_cast<Py_ssize_t>(cont_->size());
		LASS_ASSERT(size >= 0);

		if (ilow<0)
			ilow = (size-(-ilow % size)) % size;
		if (ihigh<0)
			ihigh = (size-(-ihigh % size)) % size;

		Py_ssize_t len;
		if (ilow < 0)
			ilow = 0;
		else if (ilow > size)
			ilow = size;
		if (ihigh < ilow)
			ihigh = ilow;
		else if (ihigh > size)
			ihigh = size;
		len = ihigh - ilow;
		return fromSharedPtrToNakedCast(pyBuildList(
			ContainerTraits<Container>::const_iterator_at(*cont_,ilow),
			ContainerTraits<Container>::const_iterator_at(*cont_,ilow+len) ));
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	int PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_AssItem(Py_ssize_t i, PyObject *v)
	{
		const Py_ssize_t size = static_cast<Py_ssize_t>(cont_->size());
		if (readOnly_)
		{
			PyErr_SetString(PyExc_TypeError, "Sequence is read-only");
			return -1;
		}
		if (i >= PySequence_Length()) 
		{
			PyErr_SetString(PyExc_IndexError,"list assignment index out of range");
			return -1;
		}
		if (i<0)
			i = (size-(-i % size)) % size;
		if (v == NULL)
			return PySequence_AssSlice(i, i+1, v);
		return PyExportTraits<typename Container::value_type>::get(v,ContainerTraits<Container>::element_at(*cont_,i));
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	int PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_AssSlice(Py_ssize_t ilow, Py_ssize_t ihigh, PyObject *v)
	{
		if (readOnly_)
		{
			PyErr_SetString(PyExc_TypeError, "Sequence is read-only");
			return -1;
		}
		Container temp;
		int r = pyGetSequenceObject(v,temp);
		if (r)
		{
			PyErr_SetString(PyExc_TypeError, "Cannot convert to type for slice assignment");
			return -1;
		}
		const Py_ssize_t size = static_cast<Py_ssize_t>(cont_->size());
		if (ilow<0)
			ilow = (size-(-ilow % size)) % size;
		if (ihigh<0)
			ihigh = (size-(-ihigh % size)) % size;

		if (ilow < 0)
			ilow = 0;
		else if (ilow > size)
			ilow = size;
		if (ihigh < ilow)
			ihigh = ilow;
		else if (ihigh > size)
			ihigh = size;

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
		int r = PyExportTraits<typename Container::value_type>::get(el,temp);
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
		//int r = pyGetSimpleObject(other,toConcat);
		int r = pyGetSequenceObject(other,toConcat);
		if (r)
		{
			PyErr_SetString(PyExc_TypeError, "Cannot convert to concatenation type");
			return 1;
		}
		cont_->insert(cont_->end(), toConcat.begin(), toConcat.end());
		return 0;
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	int PySequenceContainer<Container,ContainerOwnerShipPolicy>::PySequence_InplaceRepeat(Py_ssize_t n)
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
	void PySequenceContainer<Container,ContainerOwnerShipPolicy>::append(const TPyObjPtr& i)
	{
		if (readOnly_)
		{
			PyErr_SetString(PyExc_TypeError, "Sequence is read-only");
			return;
		}
		typename Container::value_type temp;
		if (pyGetSimpleObject(i.get(), temp) != 0)
		{
			fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}
		cont_->push_back( temp );
	}
	template<typename Container, typename ContainerOwnerShipPolicy>
	TPyObjPtr PySequenceContainer<Container,ContainerOwnerShipPolicy>::pop(int i)
	{
		if (readOnly_)
		{
			PyErr_SetString(PyExc_TypeError, "Sequence is read-only");
			return TPyObjPtr();
		}
		const Py_ssize_t size = static_cast<Py_ssize_t>(cont_->size());
		if (i<0)
			i = (size-(-i % size)) % size;
		typename Container::value_type temp = ContainerTraits<Container>::element_at(*cont_,i);
		cont_->erase(ContainerTraits<Container>::iterator_at(*cont_,i));
		return fromNakedToSharedPtrCast<PyObject>(lass::python::PyExportTraits<typename Container::value_type>::build(temp));
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
}



/**	@ingroup Python
 *	@internal
 */
template <typename ContainerType>
struct PyExportTraitsSequence
{
	typedef ContainerType TContainer;

	/*	build a copy of a container as a Python tuple
	 *	@note you get a read-only COPY of the container
	 */
	static PyObject* build( const TContainer& iV) 
	{ 
		return fromSharedPtrToNakedCast(impl::pyBuildTuple(iV.begin(),iV.end()));
	}

	/**	expose a container to python as a reference.
	 *	@note you build a reference to the container, any changes done in Python
	 *	will be reflected in the original object, as far as the typesystem allows it of course
	 */
	static PyObject* build(TContainer& iV) 
	{ 
		return new impl::PySequence(iV);
	}

	/**	get a copy of a Python sequence as a container.
	 *	@note you get a COPY of the sequence, not the original sequence itself!
	 */
	static int get( PyObject* iV, TContainer& oV) 
	{ 
		return impl::pyGetSequenceObject(iV, oV);
	}
};

/**	@ingroup Python
 *	@internal
 */
template< typename C, typename A>
struct PyExportTraits< std::vector< C, A > >:
	public PyExportTraitsSequence< std::vector< C, A > >
{
};

/**	@ingroup Python
 *	@internal
 */
template< typename C, typename A>
struct PyExportTraits< std::list< C, A > >:
	public PyExportTraitsSequence< std::list< C, A > >
{
};

/**	@ingroup Python
 *	@internal
 */
template< typename C, typename A>
struct PyExportTraits< std::deque< C, A > >:
	public PyExportTraitsSequence< std::deque< C, A > >
{
};

/**	@ingroup Python
 *	@internal
 */
template< typename C, size_t maxsize>
struct PyExportTraits< stde::static_vector< C, maxsize > >:
	public PyExportTraitsSequence< stde::static_vector< C, maxsize > >
{
};


}

}

#endif
