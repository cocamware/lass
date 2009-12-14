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
#include "container.h"
#include "../util/string_cast.h"
#include "../stde/extended_algorithm.h"

#include <vector>
#include <list>
#include <deque>

namespace lass
{
namespace python
{
namespace impl
{
	class LASS_DLL PySequenceImplBase: public ContainerImplBase
	{
	public:
		virtual std::auto_ptr<PySequenceImplBase> copy() const = 0;
		virtual bool reserve(size_t n) = 0;
		virtual bool append(const TPyObjPtr& i) = 0;
		virtual bool pop(Py_ssize_t i) = 0;
		virtual PyObject* item(Py_ssize_t i) const = 0;
		virtual PyObject* slice(Py_ssize_t low, Py_ssize_t high, Py_ssize_t step) const = 0;
		virtual int assItem(Py_ssize_t i, PyObject* obj) = 0;
		virtual int assSlice(Py_ssize_t low, Py_ssize_t high, Py_ssize_t step, PyObject* obj) = 0;
		virtual int contains(PyObject* obj) const = 0;
		virtual bool inplaceConcat(PyObject* obj) = 0;
		virtual bool inplaceRepeat(Py_ssize_t n) = 0;
	};

	template<typename Container> 
	class PySequenceContainer: public ContainerImpl<Container, PySequenceImplBase>
	{
	public:
		typedef ContainerImpl<Container, PySequenceImplBase> TBase;
		typedef typename TBase::TContainerPtr TContainerPtr;
		typedef typename TBase::TConstContainerPtr TConstContainerPtr;
		typedef typename TBase::TContainerTraits TContainerTraits;
		typedef typename TContainerTraits::iterator TIterator;
		typedef typename TContainerTraits::const_iterator TConstIterator;

		PySequenceContainer(const TContainerPtr& container, bool readOnly = false): 
			TBase(container, readOnly)
		{
		}
		~PySequenceContainer() 
		{
		}
		const TPyObjPtr asNative() const
		{
			return pyBuildList(this->begin(), this->next(this->begin(), this->length()));
		}
		std::auto_ptr<PySequenceImplBase> copy() const
		{
			TContainerPtr copy = TContainerTraits::copy(this->container());
			return std::auto_ptr<PySequenceImplBase>(new PySequenceContainer(copy));
		}
		bool reserve(size_t n)
		{
			if (!this->checkWritable())
			{
				return false;
			}
			TContainerTraits::reserve(this->container(), n);
			return true;
		}
		bool append(const TPyObjPtr& obj)
		{
			if (!this->checkWritable())
			{
				return false;
			}
			typename TContainerTraits::value_type value;
			if (pyGetSimpleObject(obj.get(), value) != 0)
			{
				return false;
			}
			TIterator end = this->next(this->begin(), this->length());
			TContainerTraits::insert(this->container(), end, &value, &value + 1);
			return true;
		}
		bool pop(Py_ssize_t i)
		{
			if (!this->checkWritable())
			{
				return false;
			}
			if (!this->checkIndex(i))
			{
				return false;
			}
			TContainerTraits::erase(this->container(), this->next(this->begin(), i), this->next(this->begin(), i + 1));
			return true;
		}
		PyObject* item(Py_ssize_t i) const
		{
			if (!this->checkIndex(i))
			{
				return 0;
			}
			return pyBuildSimpleObject(*this->next(this->begin(), i));
		}
		PyObject* slice(Py_ssize_t low, Py_ssize_t high, Py_ssize_t step) const
		{
			const Py_ssize_t size = this->length();
			low = num::clamp(low, Py_ssize_t(0), size);
			high = num::clamp(high, low, size);
			step = std::max(step, Py_ssize_t(1));
			const Py_ssize_t n = (high - low + step - 1) / step;
			TPyObjPtr s(PyList_New(n));
			TConstIterator first = this->next(this->begin(), low);
			for (Py_ssize_t i = 0; i < n; ++i)
			{
				if (i > 0)
				{
					// this odd place of advancing is to avoid stepping beyond end
					std::advance(first, step);
				}
				PyList_SET_ITEM(s.get(), i, pyBuildSimpleObject(*first));
			}
			return fromSharedPtrToNakedCast(s);
		}
		int assItem(Py_ssize_t i, PyObject* obj)
		{
			if (!this->checkWritable() || !this->checkIndex(i))
			{
				return -1;
			}
			if (obj == 0)
			{
				return assSlice(i, i + 1, 1, 0);
			}
			if (pyGetSimpleObject(obj, *this->next(this->begin(), i)) != 0)
			{
				return -1;
			}
			return 0;
		}
		int assSlice(Py_ssize_t low, Py_ssize_t high, Py_ssize_t step, PyObject* other)
		{
			if (!this->checkWritable())
			{
				return -1;
			}
			const Py_ssize_t size = this->length();
			low = num::clamp(low, Py_ssize_t(0), size);
			high = num::clamp(high, low, size);
			step = std::max(step, Py_ssize_t(1));
			TConstContainerPtr b;
			if (other)
			{
				if (pyGetSimpleObject(other, b) != 0)
				{
					return -1;
				}
				if (&this->container() == b.get())
				{
					b = TContainerTraits::copy(*b);
				}
			}
			if (step == 1)
			{
				const TIterator first = this->next(this->begin(), low);
				const TIterator last = this->next(first, high - low);
				TContainerTraits::erase(this->container(), first, last);
				if (b)
				{
					const TConstIterator bFirst = TContainerTraits::begin(*b);
					const TConstIterator bLast = this->next(bFirst, TContainerTraits::size(*b));
					TContainerTraits::insert(this->container(), this->next(this->begin(), low), bFirst, bLast);
				}
			}
			else
			{
				if (b)
				{
					const Py_ssize_t sliceLength = (high - low + step - 1) / step;
					const Py_ssize_t bLength = TContainerTraits::size(*b);
					if (sliceLength != bLength)
					{
						std::ostringstream buffer;
						buffer << "cannot assign sequence of length " << bLength << " to an extended slice of length " << sliceLength;
						PyErr_SetString(PyExc_ValueError, buffer.str().c_str());
						return -1;
					}
					TIterator left = this->next(this->begin(), low);
					TConstIterator right = TContainerTraits::begin(*b);
					while (low < high)
					{
						*left = *right++;
						low += step;
						if (low < high) // make sure you don't step beyond end
						{
							left = this->next(left, step);
						}
					}
				}
				else
				{
					while (low < high)
					{
						TContainerTraits::erase(this->container(), this->next(this->begin(), low));
						low += step - 1;
						--high;
					}
				}
			}
			return 0;
		}
		int contains(PyObject* obj) const
		{
			typename Container::value_type value;
			if (pyGetSimpleObject(obj, value) != 0)
			{
				if (PyErr_ExceptionMatches(PyExc_TypeError))
				{
					// type is not convertible and hence is not found
					// this corresponds to the Python behavior
					PyErr_Clear();
					return 0;
				}
				return -1;
			}
			TConstIterator end = this->next(this->begin(), this->length());
			if (std::find(this->begin(), end, value) != end)
			{
				return 1;
			}
			return 0;
		}
		bool inplaceConcat(PyObject* other)
		{
			if (!this->checkWritable())
			{
				return false;
			}
			TConstContainerPtr b;
			if (pyGetSimpleObject(other, b) != 0)
			{
				return false;
			}
			if (&this->container() == b.get())
			{
				b = TContainerTraits::copy(*b);
			}
			const TIterator end = this->next(this->begin(), this->length());
			const TConstIterator first = TContainerTraits::begin(*b);
			const TConstIterator last = this->next(first, TContainerTraits::size(*b));
			TContainerTraits::insert(this->container(), end, first, last);
			return true;
		}
		bool inplaceRepeat(Py_ssize_t n)
		{
			if (!this->checkWritable())
			{
				return false;
			}
			TContainerTraits::inplace_repeat(this->container(),n);
			return true;
		}
	private:
		bool checkIndex(Py_ssize_t& i) const
		{
			const Py_ssize_t size = this->length();
			if (i < 0 || i >= size) 
			{
				PyErr_SetString(PyExc_IndexError,"list assignment index out of range");
				return false;
			}
			return true;
		}
	};

	class Sequence;
	typedef PyObjectPtr<Sequence>::Type TSequencePtr;

	/** Object for interfacing sequence-like objects with Python 
	*/
	class LASS_DLL Sequence : public PyObjectPlus, util::NonCopyable
	{
		PY_HEADER(PyObjectPlus);
		static PySequenceMethods pySequenceMethods;
		static PyMappingMethods pyMappingMethods;
		static bool isInitialized;

	public:
		template<typename Container> Sequence( const util::SharedPtr<Container>& container )
		{
			std::auto_ptr<PySequenceImplBase> pimpl(
				new PySequenceContainer<Container>(LASS_ENFORCE_POINTER(container)));
			init(pimpl);
		}
		template<typename Container> Sequence( const util::SharedPtr<const Container>& container ) 
		{
			std::auto_ptr<PySequenceImplBase> pimpl(
				new PySequenceContainer<Container>(LASS_ENFORCE_POINTER(container).template constCast<Container>(), true));
			init(pimpl);
		}
		template<typename Container> Sequence( const Container& container )
		{
			util::SharedPtr<Container> p(ContainerTraits<Container>::copy(container));
			std::auto_ptr<PySequenceImplBase> pimpl(new PySequenceContainer<Container>(p, true));
			init(pimpl);
		}

		const TSequencePtr copy() const;
		void clear();
		void reserve(size_t n);
		void append(const TPyObjPtr& obj);
		const TPyObjPtr pop(Py_ssize_t i);
		const TPyObjPtr pop_back();
		const TPyObjPtr asList() const;
		const TPyObjPtr iter() const;

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
		static PyObject* subscript(PyObject* self, PyObject* key);
		static int assSubscript(PyObject* self, PyObject* key, PyObject* value);

		const std::type_info& type() const;
		void* raw(bool writable) const;

	private:
		Sequence(std::auto_ptr<PySequenceImplBase> pimpl);
		void init(std::auto_ptr<PySequenceImplBase> pimpl);
		static void initializeType();

		util::ScopedPtr<PySequenceImplBase> pimpl_;
	};

	template <>
	struct ShadowTraits<Sequence>: public ShadowTraitsContainer< Sequence, ShadowTraits<Sequence> >
	{
		template <typename Container> static int getObjectImpl(PyObject* obj, util::SharedPtr<Container>& value, bool writable)
		{
			if (!PySequence_Check(obj))
			{
				PyErr_SetString(PyExc_TypeError, "not a sequence");
				return 1;
			}

			// check if we have our own Sequence object, then take a shortcut
			if (obj->ob_type == Sequence::_lassPyClassDef.type())
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
					std::ostringstream buffer;
					buffer << "sequence element " << i;
					impl::addMessageHeader(buffer.str().c_str());
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
	typedef SharedPointerTraits<ContainerType> TPointerTraits;
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

}
}

#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_STDE_STATIC_VECTOR_H

namespace lass
{
namespace python
{
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

// EOF
