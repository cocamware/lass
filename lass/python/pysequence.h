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
#include "pyshadow_object.h"
#include "shadowee_traits.h"
#include "exception.h"
#include "../util/string_cast.h"
#include "../stde/extended_algorithm.h"

#include <vector>
#include <list>
#include <deque>
#include "../stde/static_vector.h"
#include <typeinfo>

namespace lass
{
namespace python
{

namespace impl
{

	template <typename Container>
	struct ContainerTraitsBase
	{
		typedef Container container_type;
		typedef typename Container::value_type value_type;
		typedef typename Container::const_iterator const_iterator;
		typedef typename Container::iterator iterator;
		static size_t size(const container_type& c)
		{
			return c.size();
		}
		static const const_iterator begin(const container_type& c) 
		{ 
			return c.begin();
		}
		static const iterator begin(container_type& c) 
		{ 
			return c.begin();
		}
		template <typename It> 
		static const It next(It it, Py_ssize_t i = 1) 
		{ 
			return it + i; 
		}
		template <typename InputIt> 
		static void insert(container_type& c, iterator i, InputIt first, InputIt last)
		{
			c.insert(i, first, last);
		}
		static void erase(container_type& c, iterator first, iterator last)
		{
			c.erase(first, last);
		}
		static void clear(container_type& c)
		{
			c.clear();
		}
		static void reserve(container_type& /*c*/, size_t /*n*/) 
		{
		}
		static const util::SharedPtr<container_type> copy(const container_type& c) 
		{ 
			return util::SharedPtr<container_type>(new container_type(c)); 
		}
		static void inplace_repeat(container_type& c, Py_ssize_t n)
		{
			stde::inplace_repeat_c(c, n);
		}
		static std::string repr(const container_type& c)
		{
			return util::stringCast<std::string>(c);
		}
	};

	template <typename Container>
	struct ContainerTraits: ContainerTraitsBase<Container>
	{
	};

	template <typename T, typename A>
	struct ContainerTraits< std::vector<T, A> >: ContainerTraitsBase< std::vector<T, A> >
	{
		typedef typename ContainerTraitsBase< std::vector<T, A> >::container_type container_type;
		static void reserve( container_type& c, size_t n ) 
		{ 
			c.reserve(n); 
		}	
	};

	template <typename T, typename A>
	struct ContainerTraits< std::list<T, A> >: ContainerTraitsBase< std::list<T, A> >
	{
		template <typename It>
		static const It next( It it, Py_ssize_t i ) 
		{ 
			std::advance(it, i); 
			return it; 
		}
	};

	class LASS_DLL PySequenceImplBase
	{
	public:
		virtual ~PySequenceImplBase() {};

		virtual std::auto_ptr<PySequenceImplBase> copy() const = 0; 
		virtual const bool clear() = 0;
		virtual const bool reserve(size_t n) = 0;
		virtual const bool append(const TPyObjPtr& i) = 0;
		virtual const bool pop(Py_ssize_t i) = 0;
		virtual const std::string repr() const = 0;

		virtual const Py_ssize_t length() const = 0;
		virtual PyObject* const item(Py_ssize_t i) const = 0;
		virtual PyObject* const slice(Py_ssize_t low, Py_ssize_t high) const = 0;
		virtual const int assItem(Py_ssize_t i, PyObject* obj) = 0;
		virtual const int assSlice(Py_ssize_t low, Py_ssize_t high, PyObject* obj) = 0;
		virtual const int contains(PyObject* obj) const = 0;
		virtual const bool inplaceConcat(PyObject* obj) = 0;
		virtual const bool inplaceRepeat(Py_ssize_t n) = 0;

		virtual const type_info& type() const = 0;
		virtual void* const raw(bool writable) = 0;
	};

	template<typename Container> 
	class PySequenceContainer : public PySequenceImplBase
	{
	public:
		typedef util::SharedPtr<Container> TContainerPtr;
		typedef util::SharedPtr<const Container> TConstContainerPtr;
		typedef ContainerTraits<Container> TContainerTraits;

		PySequenceContainer(const TContainerPtr& c, bool readOnly = false): 
			cont_(c), 
			readOnly_(readOnly)
		{
			LASS_ASSERT(cont_);
		}
		~PySequenceContainer() 
		{
		}

		std::auto_ptr<PySequenceImplBase> copy() const
		{
			TContainerPtr cont = TContainerTraits::copy(*cont_);
			return std::auto_ptr<PySequenceImplBase>(new PySequenceContainer(cont));
		}
		const bool clear()
		{
			if (!checkWritable())
			{
				return false;
			}
			TContainerTraits::clear(*cont_);
			return true;
		}
		const bool reserve(size_t n)
		{
			if (!checkWritable())
			{
				return false;
			}
			TContainerTraits::reserve(*cont_, n);
			return true;
		}
		const bool append(const TPyObjPtr& obj)
		{
			if (!checkWritable())
			{
				return false;
			}
			typename TContainerTraits::value_type value;
			if (pyGetSimpleObject(obj.get(), value) != 0)
			{
				return false;
			}
			TContainerTraits::iterator end = next(begin(), length());
			TContainerTraits::insert(*cont_, end, &value, &value + 1);
			return true;
		}
		const bool pop(Py_ssize_t i)
		{
			if (!checkWritable())
			{
				return false;
			}
			i = normalizeIndex(i);
			if (!checkIndex(i))
			{
				return false;
			}
			TContainerTraits::erase(*cont_, next(begin(), i), next(begin(), i + 1));
			return true;
		}
		const std::string repr() const
		{
			return TContainerTraits::repr(*cont_);
		}

		const Py_ssize_t length() const
		{
			const Py_ssize_t size = static_cast<Py_ssize_t>(TContainerTraits::size(*cont_));
			LASS_ASSERT(size >= 0);
			return size;
		}
		PyObject* const item(Py_ssize_t i) const
		{
			i = normalizeIndex(i);
			if (!checkIndex(i))
			{
				return 0;
			}
			return pyBuildSimpleObject(*next(begin(), i));
		}
		PyObject* const slice(Py_ssize_t low, Py_ssize_t high) const
		{
			const Py_ssize_t size = length();
			low = num::clamp(normalizeIndex(low), 0, size);
			high = num::clamp(normalizeIndex(high), low, size);
			return fromSharedPtrToNakedCast(pyBuildList(next(begin(), low), next(begin(), high)));
		}
		const int assItem(Py_ssize_t i, PyObject* obj)
		{
			i = normalizeIndex(i);
			if (!checkWritable() || !checkIndex(i))
			{
				return -1;
			}
			if (obj == 0)
			{
				return assSlice(i, i + 1, 0);
			}
			if (pyGetSimpleObject(obj, *next(begin(), i)) != 0)
			{
				return -1;
			}
			return 0;
		}
		const int assSlice(Py_ssize_t low, Py_ssize_t high, PyObject* other)
		{
			if (!checkWritable())
			{
				return -1;
			}
			const Py_ssize_t size = length();
			low = num::clamp(normalizeIndex(low), 0, size);
			high = num::clamp(normalizeIndex(high), low, size);
			TConstContainerPtr b;
			if (other)
			{
				if (pyGetSimpleObject(other, b) != 0)
				{
					return -1;
				}
				if (cont_ == b)
				{
					b = TContainerTraits::copy(*b);
				}
			}
			TContainerTraits::erase(*cont_, next(begin(), low), next(begin(), high));
			if (b)
			{
				TContainerTraits::const_iterator first = TContainerTraits::begin(*b);
				TContainerTraits::const_iterator last = next(first, TContainerTraits::size(*b));
				TContainerTraits::insert(*cont_, next(begin(), low), first, last);
			}
			return 0;
		}
		const int contains(PyObject* obj) const
		{
			typename Container::value_type value;
			if (pyGetSimpleObject(obj, value) != 0)
			{
				// type is not convertible and hence is not found
				// this corresponds to the Python behavior
				return 0;
			}
			TContainerTraits::const_iterator end = next(begin(), length());
			if (std::find(begin(), end, value) != end)
			{
				return 1;
			}
			return 0;
		}
		const bool inplaceConcat(PyObject* other)
		{
			if (!checkWritable())
			{
				return false;
			}
			TConstContainerPtr b;
			if (pyGetSimpleObject(other, b) != 0)
			{
				return false;
			}
			if (cont_ == b)
			{
				b = TContainerTraits::copy(*b);
			}
			const TContainerTraits::iterator end = next(begin(), length());
			const TContainerTraits::const_iterator first = TContainerTraits::begin(*b);
			const TContainerTraits::const_iterator last = TContainerTraits::next(first, TContainerTraits::size(*b));
			TContainerTraits::insert(*cont_, end, first, last);
			return true;
		}
		const bool inplaceRepeat(Py_ssize_t n)
		{
			if (!checkWritable())
			{
				return false;
			}
			TContainerTraits::inplace_repeat(*cont_,n);
			return true;
		}

		const type_info& type() const 
		{ 
			return typeid(TContainerPtr); 
		}
		void* const raw(bool writable)
		{ 
			if (writable && readOnly_)
			{
				return 0;
			}
			return &cont_; 
		}
	private:
		const bool checkWritable() const
		{
			if (readOnly_)
			{
				PyErr_SetString(PyExc_TypeError, "Sequence is read-only");
				return false;
			}
			return true;
		}
		const bool checkIndex(Py_ssize_t& i) const
		{
			const Py_ssize_t size = length();
			if (i < 0 || i >= size) 
			{
				PyErr_SetString(PyExc_IndexError,"list assignment index out of range");
				return false;
			}
			return true;
		}
		const Py_ssize_t normalizeIndex(Py_ssize_t i) const
		{
			return i;
			if (i < 0)
			{
				return i + length();
			}		
			return i;
		}
		const typename TContainerTraits::const_iterator begin() const
		{
			return TContainerTraits::begin(*cont_);
		}
		const typename TContainerTraits::iterator begin()
		{
			return TContainerTraits::begin(*cont_);
		}
		template <typename It> static const It next(It it, Py_ssize_t i)
		{
			return TContainerTraits::next(it, i);
		}
		TContainerPtr cont_;
		bool readOnly_;
	};

	class Sequence;
	typedef PyObjectPtr<Sequence>::Type TSequencePtr;

	/** Object for interfacing sequence-like objects with Python 
	*/
	class LASS_DLL Sequence : public PyObjectPlus, util::NonCopyable
	{
		PY_HEADER(PyObjectPlus);
		static PySequenceMethods pySequenceMethods;
		static bool isInitialized;

	public:
		template<typename Container> Sequence( const util::SharedPtr<Container>& container )
		{
			initialize();
			impl::fixObjectType(this);
			pimpl_.reset(new PySequenceContainer<Container>(LASS_ENFORCE_POINTER(container)));
		}
		template<typename Container> Sequence( const util::SharedPtr<const Container>& container ) 
		{
			initialize();
			impl::fixObjectType(this);
			pimpl_.reset(new PySequenceContainer<Container>(LASS_ENFORCE_POINTER(container).constCast<Container>(), true));
		}
		template<typename Container> Sequence( const Container& container )
		{
			initialize();
			impl::fixObjectType(this);
			util::SharedPtr<Container> p(ContainerTraits<Container>::copy(container));
			pimpl_.reset(new PySequenceContainer<Container>(p, true));
		}

		const TSequencePtr copy() const;
		void clear();
		void reserve(size_t n);
		void append(const TPyObjPtr& obj);
		const TPyObjPtr pop(Py_ssize_t i);
		const TPyObjPtr pop_back();

		std::string doPyStr();
		std::string doPyRepr();

		static Py_ssize_t length( PyObject* self);
		static PyObject* concat(PyObject* self, PyObject* other);
		static PyObject* repeat(PyObject* self, Py_ssize_t n);
		static PyObject* item(PyObject* self, Py_ssize_t i);
		static PyObject* slice(PyObject* self, Py_ssize_t low, Py_ssize_t high);
		static int assItem(PyObject* self, Py_ssize_t i, PyObject* obj);
		static int assSlice(PyObject* self, Py_ssize_t low, Py_ssize_t high, PyObject* slice);
		static int contains(PyObject* a, PyObject* obj);
		static PyObject* inplaceConcat(PyObject* self, PyObject* other);
		static PyObject* inplaceRepeat(PyObject* self, Py_ssize_t n);

		const type_info& type() const;
		void* const raw(bool writable) const;

	private:
		Sequence(std::auto_ptr<PySequenceImplBase> pimpl);
		util::ScopedPtr<PySequenceImplBase> pimpl_;
		static void initialize();
	};

	template <>
	struct ShadowTraits<Sequence>
	{
		enum { 	isShadow = true };
		typedef PyObjectPtr<Sequence>::Type TPyClassPtr;
		typedef Sequence TCppClass;
		typedef TPyClassPtr TCppClassPtr;
		typedef PyObjectPtr<const Sequence>::Type TConstCppClassPtr;

		template <typename Container> static int getObject(PyObject* obj, util::SharedPtr<Container>& value)
		{
			return getObject(obj, value, true);
		}
		template <typename Container> static int getObject(PyObject* obj, util::SharedPtr<const Container>& value)
		{
			util::SharedPtr<Container> temp;
			if (getObject(obj, temp, false) != 0)
			{
				return 1;
			}
			value = temp.constCast<const Container>();
			return 0;
		}
		static int getObject(PyObject* obj, TCppClassPtr& sequence)
		{
			sequence = fromNakedToSharedPtrCast<Sequence>(obj);
			return 0;
		}
		static int getObject(PyObject* obj, TConstCppClassPtr& sequence)
		{
			sequence = fromNakedToSharedPtrCast<const Sequence>(obj);
			return 0;
		}
		template <typename T> static TPyClassPtr buildObject(const T& value)
		{
			return TPyClassPtr(new Sequence(value));
		}
		static TPyClassPtr buildObject(const TPyClassPtr& sequence)
		{
			return sequence;
		}

	private:

		template <typename Container> static int getObject(PyObject* obj, util::SharedPtr<Container>& value, bool writable)
		{
			if (!PySequence_Check(obj))
			{
				PyErr_SetString(PyExc_TypeError, "not a sequence");
				return 1;
			}

			// check if we have our own Sequence object, then take a shortcut
			if (isOfType(obj, Sequence::_lassPyClassDef.type()))
			{
				const Sequence* const sequence = static_cast<Sequence*>(obj);
				void* const raw = sequence->raw(writable);
				if (raw && typeid(value) == sequence->type())
				{
					value = *static_cast< util::SharedPtr<Container>* >(raw);
					return 0;
				}
			}

			const util::SharedPtr<Container> result(new Container);
			const Py_ssize_t size = PySequence_Length(obj);
			ContainerTraits<Container>::reserve(*result, size);
			for (Py_ssize_t i = 0; i < size; ++i)
			{
				typename Container::value_type temp;
				if (PyExportTraits<typename Container::value_type>::get( PySequence_ITEM(obj, i) , temp ) != 0)
				{
					impl::addMessageHeader(
						std::string("sequence element ") + util::stringCast<std::string>(i));
					return 1;
				}
				result->push_back( temp );
			}
			value = result;
			return 0;
		}
	};
}

template <typename ContainerType>
struct ShadoweeTraitsSequence: meta::True
{
	typedef impl::Sequence TShadow;
	typedef impl::ShadowTraits<impl::Sequence> TShadowTraits;
	typedef DefaultShadoweePointerTraits<ContainerType> TPointerTraits;
};

/**	@ingroup Python
 *	@internal
 */
template< typename C, typename A>
struct ShadoweeTraits< std::vector< C, A > >:
	public ShadoweeTraitsSequence< std::vector< C, A > >
{
};

/**	@ingroup Python
 *	@internal
 */
template< typename C, typename A>
struct ShadoweeTraits< std::list< C, A > >:
	public ShadoweeTraitsSequence< std::list< C, A > >
{
};

/**	@ingroup Python
 *	@internal
 */
template< typename C, typename A>
struct ShadoweeTraits< std::deque< C, A > >:
	public ShadoweeTraitsSequence< std::deque< C, A > >
{
};

/**	@ingroup Python
 *	@internal
 */
template< typename C, size_t maxsize>
struct ShadoweeTraits< stde::static_vector< C, maxsize > >:
	public ShadoweeTraitsSequence< stde::static_vector< C, maxsize > >
{
};


}

}

#endif
