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
 *	Copyright (C) 2004-2026 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSEQUENCE_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYSEQUENCE_H

#include "python_common.h"
#include "pyshadow_object.h"
#include "shadowee_traits.h"
#include "exception.h"
#include "container.h"
#include "argument_traits.h"
#include "subscript.h"
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
	class LASS_PYTHON_DLL PySequenceImplBase: public ContainerImplBase
	{
	public:
		typedef std::unique_ptr<PySequenceImplBase> TPimpl;
		using ContainerImplBase::ContainerImplBase;
		virtual TPimpl copy() const = 0;
		virtual bool reserve(Py_ssize_t n) = 0;
		virtual bool append(const TPyObjPtr& i) = 0;
		virtual bool pop(Py_ssize_t i) = 0;
		virtual PyObject* item(Py_ssize_t i) const = 0;
		virtual PyObject* slice(Slice slice) const = 0;
		virtual int assItem(Py_ssize_t i, PyObject* obj) = 0;
		virtual int assSlice(Slice slice, PyObject* obj) = 0;
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
		typedef ArgumentTraits<typename TContainerTraits::value_type> TArgTraits;
		typedef PySequenceImplBase::TPimpl TPimpl;

		PySequenceContainer(const TContainerPtr& container, bool readOnly = false): 
			TBase(container, readOnly)
		{
		}
		~PySequenceContainer() 
		{
		}
		const TPyObjPtr asNative() const override
		{
			return pyBuildList(this->begin(), this->next(this->begin(), this->length()));
		}
		TPimpl copy() const override
		{
			TContainerPtr copy = TContainerTraits::copy(this->container());
			return TPimpl(new PySequenceContainer(copy));
		}
		bool reserve(Py_ssize_t n) override
		{
			if (!this->checkWritable())
			{
				return false;
			}
			TContainerTraits::reserve(this->container(), n);
			return true;
		}
		bool append(const TPyObjPtr& obj) override
		{
			if (!this->checkWritable())
			{
				return false;
			}
			typename TArgTraits::TStorage value;
			if (pyGetSimpleObject(obj.get(), value) != 0)
			{
				return false;
			}
			TIterator end = this->next(this->begin(), this->length());
			TContainerTraits::insert(this->container(), end, TArgTraits::arg(value));
			return true;
		}
		bool pop(Py_ssize_t i) override
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
		PyObject* item(Py_ssize_t i) const override
		{
			if (!this->checkIndex(i))
			{
				return 0;
			}
			return pyBuildSimpleObject(*this->next(this->begin(), i));
		}
		PyObject* slice(Slice slice) const override
		{
			Py_ssize_t sliceLength = slice.adjustIndices(this->length());
			TPyObjPtr s(PyList_New(sliceLength));
			if (!s)
			{
				return nullptr;
			}
			TConstIterator first = this->next(this->begin(), slice.start);
			for (Py_ssize_t i = 0; i < sliceLength; ++i)
			{
				if (i > 0)
				{
					// this odd place of advancing is to avoid stepping beyond end
					first = this->next(first, slice.step);
				}
				PyObject* item = pyBuildSimpleObject(*first);
				if (!item)
				{
					return nullptr;
				}
				PyList_SET_ITEM(s.get(), i, item);
			}
			return fromSharedPtrToNakedCast(s);
		}
		int assItem(Py_ssize_t i, PyObject* obj) override
		{
			if (!this->checkWritable() || !this->checkIndex(i))
			{
				return -1;
			}
			if (!obj)
			{
				Slice slice { i, i + 1, 1 };
				return assSlice(slice, nullptr);
			}
			if (pyGetSimpleObject(obj, *this->next(this->begin(), i)) != 0)
			{
				return -1;
			}
			return 0;
		}
		int assSlice(Slice slice, PyObject* other) override
		{
			if (!this->checkWritable())
			{
				return -1;
			}

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

			Py_ssize_t sliceLength = slice.adjustIndices(this->length());
			if (slice.step == 1)
			{
				LASS_ASSERT(slice.start + sliceLength <= this->length());
				const TIterator first = this->next(this->begin(), slice.start);
				const TIterator last = this->next(first, sliceLength);
				TContainerTraits::erase(this->container(), first, last);
				if (b)
				{
					const TIterator pos = this->next(this->begin(), slice.start);
					const TConstIterator bFirst = TContainerTraits::begin(*b);
					const TConstIterator bLast = this->next(bFirst, TContainerTraits::size(*b));
					TContainerTraits::insert(this->container(), pos, bFirst, bLast);
				}
			}
			else
			{
				if (b)
				{
					const Py_ssize_t bLength = TContainerTraits::size(*b);
					if (sliceLength != bLength)
					{
						std::ostringstream buffer;
						buffer << "cannot assign sequence of length " << bLength << " to an extended slice of length " << sliceLength;
						PyErr_SetString(PyExc_ValueError, buffer.str().c_str());
						return -1;
					}
					TIterator left = this->next(this->begin(), slice.start);
					TConstIterator right = TContainerTraits::begin(*b);
					for (Py_ssize_t i = 0; i < sliceLength; ++i)
					{
						if (i > 0)
						{
							// this odd place of advancing is to avoid stepping beyond end
							left = this->next(left, slice.step);
						}
						*left = *right++;
					}
				}
				else
				{
					Py_ssize_t index = slice.start;
					Py_ssize_t step = slice.step;
					if (step > 0 && sliceLength > 0)
					{
						// always traverse back to front ...
						index += (sliceLength - 1) * step;
						step = -step;
					}
					for (Py_ssize_t i = 0; i < sliceLength; ++i, index += step)
					{
						// always recompute iterator, as erase on std::deque invalidates _all_ iterators.
						// slow for std::list, but that's a rare case ...
						TContainerTraits::erase(this->container(), this->next(this->begin(), index));
					}
				}
			}
			return 0;
		}
		int contains(PyObject* obj) const override
		{
			typename TArgTraits::TStorage value;
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
			if (std::find(this->begin(), end, TArgTraits::arg(value)) != end)
			{
				return 1;
			}
			return 0;
		}
		bool inplaceConcat(PyObject* other) override
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
		bool inplaceRepeat(Py_ssize_t n) override
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
	class LASS_PYTHON_DLL Sequence : public PyObjectPlus, util::NonCopyable
	{
		PY_HEADER(PyObjectPlus)

	public:
		template<typename Container> Sequence( const util::SharedPtr<Container>& container )
		{
			TPimpl pimpl(new PySequenceContainer<Container>(
				LASS_ENFORCE_POINTER(container)));
			init(std::move(pimpl));
		}
		template<typename Container> Sequence( const util::SharedPtr<const Container>& container ) 
		{
			TPimpl pimpl(new PySequenceContainer<Container>(
				(LASS_ENFORCE_POINTER(container)).template constCast<Container>(), true));
			init(std::move(pimpl));
		}
		template<typename Container> Sequence( const Container& container )
		{
			util::SharedPtr<Container> p(ContainerTraits<Container>::copy(container));
			TPimpl pimpl(new PySequenceContainer<Container>(p, true));
			init(std::move(pimpl));
		}

		const TSequencePtr copy() const;
		void clear();
		void reserve(Py_ssize_t n);
		void append(const TPyObjPtr& obj);
		const TPyObjPtr pop(Py_ssize_t i);
		const TPyObjPtr pop_back();
		const TPyObjPtr asList() const;
		const TPyObjPtr iter() const;
		std::string repr() const;

		const std::type_info& type() const;
		void* raw(bool writable) const;

		static Py_ssize_t length(PyObject* self);
		static PyObject* concat(PyObject* self, PyObject* other);
		static PyObject* repeat(PyObject* self, Py_ssize_t n);
		static PyObject* item(PyObject* self, Py_ssize_t i);
		static int assItem(PyObject* self, Py_ssize_t i, PyObject* obj);
		static int contains(PyObject* a, PyObject* obj);
		static PyObject* inplaceConcat(PyObject* self, PyObject* other);
		static PyObject* inplaceRepeat(PyObject* self, Py_ssize_t n);
		static PyObject* subscript(PyObject* self, PyObject* key);
		static int assSubscript(PyObject* self, PyObject* key, PyObject* value);

	private:
		typedef PySequenceImplBase::TPimpl TPimpl;

		Sequence(TPimpl&& pimpl);
		void init(TPimpl&& pimpl);

		TPimpl pimpl_;
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
			typedef ArgumentTraits<typename Container::value_type> TArgTraits;
			for (Py_ssize_t i = 0; i < size; ++i)
			{
				typename TArgTraits::TStorage temp;
				TPyObjPtr item( PySequence_ITEM(obj, i) );
				if (pyGetSimpleObject( item.get() , temp ) != 0)
				{
					std::ostringstream buffer;
					buffer << "sequence element " << i;
					impl::addMessageHeader(buffer.str().c_str());
					return 1;
				}
				result->emplace_back(TArgTraits::arg(temp));
			}
			value = std::move(result);
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
