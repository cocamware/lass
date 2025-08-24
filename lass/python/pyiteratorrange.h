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
 *	Copyright (C) 2004-2025 the Initial Developer.
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


#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYITERATORRANGE_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYITERATORRANGE_H

#include "python_common.h"
#include "pyobject_plus.h"
#include "_lass_module.h"
#include "pyshadow_object.h"


#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4244)
#endif

namespace lass
{
namespace python
{


/** @defgroup PythonIterators Python Iterators
 *  @brief Exposing iterators to Python
 * 
 *  Lass provides three sets of building blocks to add iterator support to your
 *  Python exports:
 *
 *  1. `PyIteratorRange`: the central C++ type that implements the Python Iterator
 *     protocol. It's used for all exported Python iterators from the bindings.
 *     It's Python type is `_lass.PyIteratorRange` and will be type-hinted as
 *     `Iterator[Any]`.
 * 
 *     `PyIteratorRange` uses the pimpl idiom to hold an object that implements the
 *     `impl::PyIteratorRangeImplBase` interface to perform the actual C++ iteration.
 *
 *     There's two common implementations of this interface:
 *     - `impl::PyIteratorRangeImpl`: for iterating over a C++ iterator pair
 *     - `impl::PyIndexIteratorRangeImpl`: for iterating over a sequence by index.
 *
 *     You can directly return a `PyIteratorRange` from your bindings, or you can
 *     use one of the convenience methods below.
 *
 *  2. Two groups of macros:
 *
 *     - `PY_CLASS_ITERFUNC()`, `PY_CLASS_ITERFUNC_DOC()` and `PY_CLASS_ITERFUNC_EX()`
 *     - `PY_CLASS_FREE_ITERFUNC()`, `PY_CLASS_FREE_ITERFUNC_DOC()` and `PY_CLASS_ITERFUNC_EX()`
 * 
 *     These older macros help you to define the `__iter__` method for your class by
 *     directly returning a `PyIteratorRange` object.
 *
 *  3. Range views:
 * 
 *     - `ContainerRangeView` with `makeContainerRangeView()`
 *     - `MemberRangeView` with `makeMemberRangeView()` and `makeMemberRangeViewFactory()`
 *     - `FreeRangeView` with `makeFreeRangeView()` and `makeFreeRangeViewFactory()`
 *     - `IndexedRangeView` with `makeIndexedRangeView()` and `makeIndexedRangeViewFactory()`
 *     - `FreeRangeView` with `makeFreeRangeView()` and `makeFreeRangeViewFactory()`
 *
 *     The benefit of using these is that they'll use the actual value-type in the
 *     type-hints instead of `Any`.
 *
 *  @ingroup Python
 */
namespace impl
{
	/// begin of iterators

	/** Interface to C++ iteration, used by PyIteratorRange
	 *  @ingroup PythonIterators
	 */
	class LASS_PYTHON_DLL PyIteratorRangeImplBase
	{
	public:
		PyIteratorRangeImplBase() = default;
		virtual ~PyIteratorRangeImplBase() = default;
		virtual PyObject* iterNext() = 0;
	};

	/** Implementation of PyIteratorRangeImplBase that iterates over a C++ iterator pair.
	 *  @ingroup PythonIterators
	 */
	template<typename Iterator> 
	class PyIteratorRangeImpl : public PyIteratorRangeImplBase
	{
	public:
		PyIteratorRangeImpl(Iterator first, Iterator last): first_(first), last_(last), current_(first) {}
		~PyIteratorRangeImpl() = default;
		PyObject* iterNext() override
		{
			if (current_ != last_)
			{
				return pyBuildSimpleObject(*current_++);
			}
			// according to python specs this is allowed and is equivalent of setting the 
			// stopiteration exception
			return nullptr;
		}
	private:
		Iterator first_;
		Iterator last_;
		Iterator current_;
	};

	/** Implementation of PyIteratorRangeImplBase to iterate over a sequence by index.
	 *  @ingroup PythonIterators
	 */
	template<typename ValueType, typename SizeType>
	class PyIndexIteratorRangeImpl : public PyIteratorRangeImplBase
	{
	public:
		using AtFunc = std::function<ValueType(SizeType)>;
		PyIndexIteratorRangeImpl(AtFunc atFunc, SizeType size) : atFunc_(atFunc), size_(size), index_(0) {}
		~PyIndexIteratorRangeImpl() = default;
		PyObject* iterNext() override
		{
			if (index_ < size_)
			{
				return pyBuildSimpleObject(atFunc_(index_++));
			}
			return nullptr; // equivalent to StopIteration in Python
		}
	private:
		AtFunc atFunc_;
		SizeType size_;
		SizeType index_;
	};
}


/** Python iterator object
 * 
 *  This is the class that implements the Python iterator interface and that is exposed
 *  to Python. It can be created directly with two iterators, or with a custom
 *  implementation deriving from `PyIteratorRangeImplBase`.
 * 
 *  @ingroup PythonIterators
 */
class LASS_PYTHON_DLL PyIteratorRange : public lass::python::PyObjectPlus
{
	PY_HEADER(PyObjectPlus)
public:
	using TPimpl = std::unique_ptr<impl::PyIteratorRangeImplBase>;

	/** Construct PyIteratorRange with custom implementation */
	PyIteratorRange(TPimpl pimpl);

	/** Construct PyIteratorRange with default implemenation iterating between two iterators */
	template<typename Iterator> PyIteratorRange(Iterator first, Iterator last):
		PyIteratorRange(std::make_unique<impl::PyIteratorRangeImpl<Iterator>>(first, last))
	{
	}

	/** Owner object of the iterators
	 * 
	 *  This is the object that owns the iterators that PyIteratorRange is iterating over
	 *  and that must stay alive while the PyIteratorRange is in use. If the owning object
	 *  is destroyed while the PyIteratorRange is still in use, the iterators will become
	 *  invalid and any further attempts to use the PyIteratorRange will result in undefined
	 *  behavior.
	 * 
	 *  Setting the owner on the PyIteratorRange will prevent that it is destroyed while
	 *  the PyIteratorRange is still in use.
	 * 
	 *  If a method or free method returns a PyIteratorRange, the owner will automatically
	 *  be set to `self`, the instance the method was called on, if the owner was not yet
	 *  set. If that doesn't work for your use case, you must set the owner manually.
	 */
	///@{
	const TPyObjPtr& owner() const;
	void setOwner(const TPyObjPtr& owner);
	///@}

	static PyObject* iter( PyObject* iPo);
	static PyObject* iterNext( PyObject* iPO);
	
private:
	TPimpl pimpl_;
	TPyObjPtr owner_;
};



/** View that adapts a C++ Range (begin()/end()) to a PyIteratorRange.
 * 
 *  ContainerRangeView owns a ShadoweePtr to the container and produces a PyIteratorRange
 *  via `iter()`, setting the owner to the underlying container. The PyIteratorRange will
 *  iterate over `self->begin()` to `self->end()`.
 * 
 *  The main benefit of returning a ContainerRangeView over a plain PyIteratorRange is
 *  that it correctly types the iterator's value type in the generated Python stubs as
 *  `ValueType`, in contrast to the generic `Any` type used by PyIteratorRange.
 *
 *  @tparam SelfType C++ container type
 *  @tparam ValueType Element type returned by the container's iterators
 *
 *  @ingroup PythonIterators
 */
template <typename SelfType, typename ValueType>
class ContainerRangeView
{
public:
	using TSelf = SelfType;
	using TValue = ValueType;
	using TSelfPtr = ShadoweePtr<TSelf>;
	using TShadow = typename ShadoweeTraits<TSelf>::TShadow;
	using TShadowTraits = impl::ShadowTraits<TShadow>;
	using TShadowPtr = typename TShadowTraits::TPyClassPtr;

	/** Constructor creating a range view over a container */
	ContainerRangeView(TSelfPtr self):
		self_(std::move(self))
	{
	}

	std::unique_ptr<PyIteratorRange> iter() const
	{
		auto it = std::make_unique<PyIteratorRange>(self_->begin(), self_->end());
		TShadowPtr owner = TShadowTraits::buildObject(self_);
		[[maybe_unused]] TSelfPtr tmp;
		LASS_ASSERT(TShadowTraits::getObject(owner.get(), tmp) == 0 && tmp == self_);
		it->setOwner(owner);
		return it;
	}
private:
	TSelfPtr self_;
};



/** Returns a `ContainerRangeView` iterating over `self->begin()` to `self->end()`
 *
 * It automatically deduces `ValueType` from the container's iterators.
 *
 * This function can be used as a free method:
 * ```
 * PY_CLASS_FREE_METHOD_NAME(MyContainer, makeContainerRangeView<MyContainer>, methods::_iter_);
 * ```
 *
 * @ingroup PythonIterators
 * @relates ContainerRangeView
 */
template <typename T>
auto makeContainerRangeView(const ShadoweePtr<T>& self)
{
	using TValue1 = decltype(*std::declval<T>().begin());
	using TValue2 = decltype(*std::declval<T>().end());
	static_assert(std::is_same_v<TValue1, TValue2>, "Begin and end iterators must return the same value type");
	return ContainerRangeView<T, TValue1>(self);
}



/** View that adapts member iterator accessors to a PyIteratorRange.
 *
 *  MemberRangeView holds a ShadoweePtr to the container, and obtains the iterator pair by
 *  calling the supplied member accessors `(self->*begin)()` and `(self->*end)()` when 
 *  `iter()` is invoked.
 * 
 *  `iter()` produces a `PyIteratorRange` and sets `self` as its owner, keeping the container
 *  alive for the duration of the iteration.
 *
 *  The main benefit of returning a MemberRangeView over a plain PyIteratorRange is
 *  that it correctly types the iterator's value type in the generated Python stubs as
 *  `ValueType`, in contrast to the generic `Any` type used by PyIteratorRange.
 * 
 *  @tparam SelfType C++ container type
 *  @tparam ValueType Element type returned by the container's iterators
 *  @tparam GetIterator Function type of the member iterator accessors.
 * 
 *  @ingroup PythonIterators
 */
template <typename SelfType, typename ValueType, typename GetIterator>
class MemberRangeView
{
public:
	using TSelf = SelfType;
	using TValue = ValueType;
	using TGetIterator = GetIterator;
	using TSelfPtr = ShadoweePtr<TSelf>;
	using TShadow = typename ShadoweeTraits<TSelf>::TShadow;
	using TShadowTraits = impl::ShadowTraits<TShadow>;
	using TShadowPtr = typename TShadowTraits::TPyClassPtr;

	MemberRangeView(TSelfPtr self, TGetIterator begin, TGetIterator end):
		self_(std::move(self)),
		begin_(begin),
		end_(end)
	{
	}

	std::unique_ptr<PyIteratorRange> iter() const
	{
		auto it = std::make_unique<PyIteratorRange>(((*self_).*begin_)(), ((*self_).*end_)());
		TShadowPtr owner = TShadowTraits::buildObject(self_);
		[[maybe_unused]] TSelfPtr tmp;
		LASS_ASSERT(TShadowTraits::getObject(owner.get(), tmp) == 0 && tmp == self_);
		it->setOwner(owner);
		return it;
	}
private:
	TSelfPtr self_;
	TGetIterator begin_;
	TGetIterator end_;
};



/** Returns a `MemberRangeView` iterating over `(self->*begin)()` and `(self->*end)()`
 *
 * @ingroup PythonIterators
 * @relates MemberRangeView
 */
template <typename T, typename GetIterator>
auto makeMemberRangeView(const ShadoweePtr<T>& self, GetIterator begin, GetIterator end)
{
	using TGetIterator = std::decay_t<GetIterator>;
	using TValue1 = decltype(*(std::declval<T>().*begin)());
	using TValue2 = decltype(*(std::declval<T>().*end)());
	static_assert(std::is_same_v<TValue1, TValue2>, "Begin and end iterators must return the same value type");
	return MemberRangeView<T, TValue1, TGetIterator>(self, begin, end);
}



/** Returns a callable creating a `MemberRangeView` iterating over `(self->*begin)()` to `(self->*end)()`
 *
 * Can be used to create a free method:
 * ```
 * PY_CLASS_FREE_METHOD_NAME(MyContainer, makeMemberRangeViewFactory<MyContainer>(&MyContainer::begin, &MyContainer::end), methods::_iter_);
 * ```
 * 
 * @ingroup PythonIterators
 * @relates FreeRangeView
 */
template <typename T, typename GetIterator>
auto makeMemberRangeViewFactory(GetIterator begin, GetIterator end)
{
	using TSelfPtr = ShadoweePtr<T>;
	using TGetIterator = std::decay_t<GetIterator>;
	using TRangeView = decltype(makeMemberRangeView<T>(std::declval<TSelfPtr>(), std::declval<TGetIterator>(), std::declval<TGetIterator>()));
	using TFactory = std::function<TRangeView(TSelfPtr)>;

	TGetIterator start = begin;
	TGetIterator stop = end;
	TFactory makeRangeView = [start, stop](TSelfPtr self) {
		return makeMemberRangeView<T>(self, start, stop);
	};
	return makeRangeView;
}



/** View that adapts free iterator accessors to a PyIteratorRange.
 * 
 *  FreeRangeView holds a ShadoweePtr to the container, and obtains the iterator pair by
 *  calling the supplied free-callable accessors `begin(*self)` and `end(*self)` when 
 *  `iter()` is invoked.
 * 
 *  `iter()` produces a `PyIteratorRange` and sets `self` as its owner, keeping the container
 *  alive for the duration of the iteration.
 * 
 *  The main benefit of returning a FreeRangeView over a plain PyIteratorRange is
 *  that it correctly types the iterator's value type in the generated Python stubs as
 *  `ValueType`, in contrast to the generic `Any` type used by PyIteratorRange.
 *
 *  @tparam SelfType C++ container type
 *  @tparam ValueType Element type returned by the container's iterators
 *  @tparam GetIterator Function type of the free-callable iterator accessors; expected to
 *          accept a reference to the container as sole argument and return an iterator.
 * 
 *  @ingroup PythonIterators
 */
template <typename SelfType, typename ValueType, typename GetIterator>
class FreeRangeView
{
public:
	using TSelf = SelfType;
	using TValue = ValueType;
	using TGetIterator = GetIterator;
	using TSelfPtr = ShadoweePtr<TSelf>;
	using TShadow = typename ShadoweeTraits<TSelf>::TShadow;
	using TShadowTraits = impl::ShadowTraits<TShadow>;
	using TShadowPtr = typename TShadowTraits::TPyClassPtr;

	FreeRangeView(TSelfPtr self, GetIterator begin, GetIterator end):
		self_(std::move(self)),
		begin_(begin),
		end_(end)
	{
	}

	std::unique_ptr<PyIteratorRange> iter() const
	{
		auto it = std::make_unique<PyIteratorRange>(begin_(*self_), end_(*self_));
		TShadowPtr owner = TShadowTraits::buildObject(self_);
		[[maybe_unused]] TSelfPtr tmp;
		LASS_ASSERT(TShadowTraits::getObject(owner.get(), tmp) == 0 && tmp == self_);
		it->setOwner(owner);
		return it;
	}
private:
	TSelfPtr self_;
	TGetIterator begin_;
	TGetIterator end_;
};



/** Returns a `FreeRangeView` iterating over `begin(*self)` and `end(*self)`
 *
 * @ingroup PythonIterators
 * @relates FreeRangeView
 */
template <typename T, typename GetIterator>
auto makeFreeRangeView(const ShadoweePtr<T>& self, GetIterator begin, GetIterator end)
{
	using TGetIterator = std::decay_t<GetIterator>;
	using TValue1 = decltype(*begin(std::declval<T&>()));
	using TValue2 = decltype(*end(std::declval<T&>()));
	static_assert(std::is_same_v<TValue1, TValue2>, "Begin and end iterators must return the same value type");
	return FreeRangeView<T, TValue1, TGetIterator>(self, begin, end);
}



/** Returns a callable creating a `FreeRangeView` iterating over `begin(*self)` and `end(*self)`
 * 
 * Can be used to create a free method:
 * ```
 * PY_CLASS_FREE_METHOD_NAME(MyContainer, makeFreeRangeViewFactory<MyContainer>(containerBegin, containerEnd), methods::_iter_);
 * ```
 * 
 * @ingroup PythonIterators
 * @relates FreeRangeView
 */
template <typename T, typename GetIterator>
auto makeFreeRangeViewFactory(GetIterator begin, GetIterator end)
{
	using TSelfPtr = ShadoweePtr<T>;
	using TGetIterator = std::decay_t<GetIterator>;
	using TRangeView = decltype(makeFreeRangeView<T>(std::declval<TSelfPtr>(), std::declval<TGetIterator>(), std::declval<TGetIterator>()));
	using TFactory = std::function<TRangeView(TSelfPtr)>;

	TGetIterator start = begin;
	TGetIterator stop = end;
	TFactory makeRangeView = [start, stop](TSelfPtr self) {
		return makeFreeRangeView<T>(self, start, stop);
	};
	return makeRangeView;
}



/** View that adapts index-based element access to a PyIteratorRange.
 *
 *  IndexedRangeView holds a ShadoweePtr to the container, and iterates ove the elements
 *  by index. It uses the supplied member functions `atMethod` and `sizeMethod` to access
 *  the elements and the size of the container.
 * 
 *  It retrieves the size of the container by calling `(self->*sizeMethod)()` once when the
 *  iteration starts, and accesses the elements by calling `(self->*atMethod)(index)` for
 *  each index from `0` to `size-1`.
 *
 *  `iter()` produces a `PyIteratorRange` and sets `self` as its owner, keeping the container
 *  alive for the duration of the iteration.
 *
 *  @tparam SelfType C++ container type
 *  @tparam ValueType Element type returned by the container's iterators
 *  @tparam SizeType Argument type of AtMethod and return type of SizeMethod
 *  @tparam AtMethod Member function type of the element accessor;
 *                   expected to accept a SizeType index and return a ValueType
 *  @tparam SizeMethod Member function type of the size accessor; expected to return a SizeType
 *
 *  @ingroup PythonIterators
 */
template <typename SelfType, typename ValueType, typename SizeType, typename AtMethod, typename SizeMethod>
class IndexedRangeView
{
public:
	using TSelf = SelfType;
	using TShadowTraits = impl::ShadowTraits<typename ShadoweeTraits<TSelf>::TShadow>;
	using TSelfPtr = ShadoweePtr<TSelf>;
	using TShadowPtr = typename TShadowTraits::TPyClassPtr;

	using TValue = ValueType;
	using TSize = SizeType;
	using TImpl = impl::PyIndexIteratorRangeImpl<TValue, TSize>;

	IndexedRangeView(TSelfPtr self, AtMethod atMethod, SizeMethod sizeMethod):
		self_(std::move(self)),
		atMethod_(atMethod),
		sizeMethod_(sizeMethod)
	{
	}

	std::unique_ptr<PyIteratorRange> iter() const
	{
		TSelfPtr self = self_;
		AtMethod at = atMethod_;
		TSize size = ((*self).*sizeMethod_)();
		auto pimpl = std::make_unique<TImpl>(
			[self, at](TSize index) -> TValue { return ((*self).*at)(index); },
			size
		);
		auto it = std::make_unique<PyIteratorRange>(std::move(pimpl));
		TShadowPtr owner = TShadowTraits::buildObject(self);
		[[maybe_unused]] TSelfPtr tmp;
		LASS_ASSERT(TShadowTraits::getObject(owner.get(), tmp) == 0 && tmp == self);
		it->setOwner(owner);
		return it;
	}
private:
	TSelfPtr self_;
	AtMethod atMethod_;
	SizeMethod sizeMethod_;
};


/** Returns a `IndexedRangeView` iterating over `(self->*atMethod)(index)` for index in 0 to `(self->*sizeMethod)() - 1`
 *
 * @ingroup PythonIterators
 * @relates IndexedRangeView
 */
template <typename T, typename AtMethod, typename SizeMethod>
auto makeIndexedRangeView(const ShadoweePtr<T>& self, AtMethod atMethod, SizeMethod sizeMethod)
{
	using TAtMethod = std::decay_t<AtMethod>;
	using TSizeMethod = std::decay_t<SizeMethod>;
	using TSize = decltype((std::declval<T>().*sizeMethod)());
	using TValue = decltype((std::declval<T>().*atMethod)(std::declval<TSize>()));
	return IndexedRangeView<T, TValue, TSize, TAtMethod, TSizeMethod>(std::move(self), atMethod, sizeMethod);
}


/** Returns callable that creates a `IndexedRangeView` iterating over `(self->*atMethod)(index)` for index in 0 to `(self->*sizeMethod)() - 1`
 *
 * Can be used to create a free method:
 * ```
 * PY_CLASS_FREE_METHOD_NAME(MyContainer, makeIndexedRangeViewFactory<MyContainer>(&MyContainer::operator[], &MyContainer::size), methods::_iter_);
 * ```
 * 
 * @ingroup PythonIterators
 * @relates IndexedRangeView
 */
template <typename T, typename AtMethod, typename SizeMethod>
auto makeIndexedRangeViewFactory(AtMethod atMethod, SizeMethod sizeMethod)
{
	using TSelfPtr = ShadoweePtr<T>;
	using TAtMethod = std::decay_t<AtMethod>;
	using TSizeMethod = std::decay_t<SizeMethod>;
	using TRangeView = decltype(makeIndexedRangeView<T>(std::declval<TSelfPtr>(), std::declval<TAtMethod>(), std::declval<TSizeMethod>()));
	using TFactory = std::function<TRangeView(TSelfPtr)>;

	TAtMethod at = atMethod;
	TSizeMethod size = sizeMethod;
	TFactory makeRangeView = [at, size](TSelfPtr self) { 
		return makeIndexedRangeView<T>(self, at, size);
	};
	return makeRangeView;
}



/** View that adapts index-based element access to a PyIteratorRange.
 *
 *  IndexedRangeView holds a ShadoweePtr to the container, and iterates ove the elements
 *  by index. It uses the supplied member functions `atMethod` and `sizeMethod` to access
 *  the elements and the size of the container.
 *
 *  It retrieves the size of the container by calling `(self->*sizeMethod)()` once when the
 *  iteration starts, and accesses the elements by calling `(self->*atMethod)(index)` for
 *  each index from `0` to `size-1`.
 *
 *  `iter()` produces a `PyIteratorRange` and sets `self` as its owner, keeping the container
 *  alive for the duration of the iteration.
 *
 *  @tparam SelfType C++ container type
 *  @tparam ValueType Element type returned by the container's iterators
 *  @tparam SizeType Argument type of AtMethod and return type of SizeMethod
 *  @tparam AtMethod Member function type of the element accessor;
 *                   expected to accept a SizeType index and return a ValueType
 *  @tparam SizeMethod Member function type of the size accessor; expected to return a SizeType
 *
 *  @ingroup PythonIterators
 */
template <typename SelfType, typename ValueType, typename SizeType, typename AtFunc, typename SizeFunc>
class FreeIndexedRangeView
{
public:
	using TSelf = SelfType;
	using TShadowTraits = impl::ShadowTraits<typename ShadoweeTraits<TSelf>::TShadow>;
	using TSelfPtr = ShadoweePtr<TSelf>;
	using TShadowPtr = typename TShadowTraits::TPyClassPtr;

	using TValue = ValueType;
	using TSize = SizeType;
	using TImpl = impl::PyIndexIteratorRangeImpl<TValue, TSize>;

	FreeIndexedRangeView(TSelfPtr self, AtFunc atFunc, SizeFunc sizeFunc) :
		self_(std::move(self)),
		atFunc_(atFunc),
		sizeFunc_(sizeFunc)
	{
	}

	std::unique_ptr<PyIteratorRange> iter() const
	{
		TSelfPtr self = self_;
		AtFunc at = atFunc_;
		TSize size = sizeFunc_(*self);
		auto pimpl = std::make_unique<TImpl>(
			[self, at](TSize index) -> TValue { return at(*self, index); },
			size
		);
		auto it = std::make_unique<PyIteratorRange>(std::move(pimpl));
		TShadowPtr owner = TShadowTraits::buildObject(self);
		[[maybe_unused]] TSelfPtr tmp;
		LASS_ASSERT(TShadowTraits::getObject(owner.get(), tmp) == 0 && tmp == self);
		it->setOwner(owner);
		return it;
	}
private:
	TSelfPtr self_;
	AtFunc atFunc_;
	SizeFunc sizeFunc_;
};


/** Returns a `FreeIndexedRangeView` iterating over `atFunc(*self, index)` for index in 0 to `sizeFunc(*self) - 1`
 *
 * @ingroup PythonIterators
 * @relates FreeIndexedRangeView
 */
template <typename T, typename AtFunc, typename SizeFunc>
auto makeFreeIndexedRangeView(const ShadoweePtr<T>& self, AtFunc atFunc, SizeFunc sizeFunc)
{
	using TSize = decltype(sizeFunc(std::declval<T&>()));
	using TValue = decltype(atFunc(std::declval<T&>(), std::declval<TSize>()));
	return FreeIndexedRangeView<T, TValue, TSize, AtFunc, SizeFunc>(std::move(self), atFunc, sizeFunc);
}


/** Returns a callable that creates `FreeIndexedRangeView` iterating over `atFunc(*self, index)` for index in 0 to `sizeFunc(*self) - 1`
 *
 * Can be used to create a free method:
 * ```
 * PY_CLASS_FREE_METHOD_NAME(MyContainer, makeFreeIndexedRangeViewFactory<MyContainer>(myContainerAt, myContainerSize), methods::_iter_);
 * ```
 *
 * @ingroup PythonIterators
 * @relates FreeIndexedRangeView
 */
template <typename T, typename AtFunc, typename SizeFunc>
auto makeFreeIndexedRangeViewFactory(AtFunc atFunc, SizeFunc sizeFunc)
{
	using TSelfPtr = ShadoweePtr<T>;
	using TAtFunc = std::decay_t<AtFunc>;
	using TSizeFunc = std::decay_t<SizeFunc>;
	using TRangeView = decltype(makeFreeIndexedRangeView<T>(std::declval<TSelfPtr>(), std::declval<TAtFunc>(), std::declval<TSizeFunc>()));
	using TFactory = std::function<TRangeView(TSelfPtr)>;

	TAtFunc at = atFunc;
	TSizeFunc size = sizeFunc;
	TFactory makeRangeView = [at, size](TSelfPtr self) {
		return makeFreeIndexedRangeView<T>(self, at, size);
	};
	return makeRangeView;
}




/** Passes a PyIteratorRange to Python
 *
 *  @note This steals a reference to the PyIteratorRange object.
 *
 *  @ingroup PythonIterators PyExportTraits
 */
template<>
struct PyExportTraits<PyIteratorRange*>
{
	constexpr static const char* py_typing = "Iterator[Any]";
	constexpr static const char* py_typing_preamble = "from collections.abc import Iterator";

	/** we take ownership! */
	static PyObject* build( PyIteratorRange* iV )
	{
		return iV;
	}
};


template <typename RangeViewType, typename ValueType>
struct PyExportTraitsRangeView
{
	constexpr static const char* py_typing = "Iterator[ValueType]";
	constexpr static const char* py_typing_preamble = "from collections.abc import Iterator";

	static PyObject* build(const RangeViewType& value)
	{
		return value.iter().release();
	}
};


/** Builds a PyIteratorRange from a ContainerRangeView
 *
 * @ingroup PythonIterators PyExportTraits
 * @relates ContainerRangeView
 */
template <typename SelfType, typename ValueType>
struct PyExportTraits<ContainerRangeView<SelfType, ValueType>>:
	PyExportTraitsRangeView<ContainerRangeView<SelfType, ValueType>, ValueType>
{
};


/** Builds a PyIteratorRange from a MemberRangeView
 *
 * @ingroup PythonIterators PyExportTraits
 * @relates MemberRangeView
 */
template <typename SelfType, typename ValueType, typename GetIterator>
struct PyExportTraits<MemberRangeView<SelfType, ValueType, GetIterator>>:
	PyExportTraitsRangeView<MemberRangeView<SelfType, ValueType, GetIterator>, ValueType>
{
};


/** Builds a PyIteratorRange from a FreeRangeView
 *
 * @ingroup PythonIterators PyExportTraits
 * @relates FreeRangeView
 */
template <typename SelfType, typename ValueType, typename GetIterator>
struct PyExportTraits< FreeRangeView<SelfType, ValueType, GetIterator> >:
	PyExportTraitsRangeView<FreeRangeView<SelfType, ValueType, GetIterator>, ValueType>
{
};


/** Builds a PyIteratorRange from a IndexedRangeView
 *
 * @ingroup PythonIterators PyExportTraits
 * @relates IndexedRangeView
 */
template <typename SelfType, typename ValueType, typename SizeType, typename AtMethod, typename SizeMethod>
struct PyExportTraits< IndexedRangeView<SelfType, ValueType, SizeType, AtMethod, SizeMethod> >:
	PyExportTraitsRangeView<IndexedRangeView<SelfType, ValueType, SizeType, AtMethod, SizeMethod>, ValueType>
{
};


/** Builds a PyIteratorRange from a FreeIndexedRangeView
 *
 * @ingroup PythonIterators PyExportTraits
 * @relates FreeIndexedRangeView
 */
template <typename SelfType, typename ValueType, typename SizeType, typename AtFunc, typename SizeFunc>
struct PyExportTraits< FreeIndexedRangeView<SelfType, ValueType, SizeType, AtFunc, SizeFunc> > :
	PyExportTraitsRangeView<FreeIndexedRangeView<SelfType, ValueType, SizeType, AtFunc, SizeFunc>, ValueType>
{
};


}

}

// --- iterators -------------------------------------------------------------------------------------

/** @ingroup PythonIterators
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
	lass::python::PyIteratorRange* LASS_CONCATENATE_3( lassPyImpl_method_, i_dispatcher, itDispatch1 ) (const ::lass::python::impl::ShadowTraits< t_cppClass >::TCppClassPtr& iObj) { \
	return new lass::python::PyIteratorRange(iObj->i_cppBegin (), iObj->i_cppEnd ()); } \
	PY_CLASS_FREE_METHOD_NAME_DOC( t_cppClass, LASS_CONCATENATE_3( lassPyImpl_method_, i_dispatcher, itDispatch1 ), lass::python::methods::_iter_, s_doc)
/** @ingroup PythonIterators
 *  convenience macro, wraps PY_CLASS_ITERFUNC_EX with
 *  @a i_dispatcher = lassPyImpl_method_ ## @a i_cppClass ## __LINE__.
 */
#define PY_CLASS_ITERFUNC_DOC( i_cppClass, i_cppBegin, icppEnd, s_doc )\
		PY_CLASS_ITERFUNC_EX(\
			i_cppClass, i_cppBegin, icppEnd, s_doc,\
			LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
/** @ingroup PythonIterators
 *  convenience macro, wraps PY_CLASS_ITERFUNC_DOC with @a s_doc = 0.
 */
#define PY_CLASS_ITERFUNC( i_cppClass, i_cppBegin, icppEnd )\
		PY_CLASS_ITERFUNC_DOC( i_cppClass, i_cppBegin, icppEnd, 0 )

#define PY_CLASS_FREE_ITERFUNC_EX( t_cppClass, i_cppBegin, i_cppEnd, s_doc, i_dispatcher )\
lass::python::PyIteratorRange* LASS_CONCATENATE_3( lassPyImpl_method_, i_dispatcher, itDispatch2 ) (const ::lass::python::impl::ShadowTraits< t_cppClass >::TCppClassPtr& iObj) { \
return new lass::python::PyIteratorRange(i_cppBegin(iObj), i_cppEnd(iObj)); } \
	PY_CLASS_FREE_METHOD_NAME_DOC( t_cppClass, LASS_CONCATENATE_3( lassPyImpl_method_, i_dispatcher, itDispatch2 ), lass::python::methods::_iter_, s_doc)
/** @ingroup PythonIterators
 *  convenience macro, wraps PY_CLASS_ITERFUNC_EX with
 *  @a i_dispatcher = lassPyImpl_method_ ## @a i_cppClass ## __LINE__.
 */
#define PY_CLASS_FREE_ITERFUNC_DOC( i_cppClass, i_cppBegin, icppEnd, s_doc )\
		PY_CLASS_FREE_ITERFUNC_EX(\
			i_cppClass, i_cppBegin, icppEnd, s_doc,\
			LASS_UNIQUENAME(LASS_CONCATENATE(lassPyImpl_method_, i_cppClass)))
/** @ingroup PythonIterators
 *  convenience macro, wraps PY_CLASS_ITERFUNC_DOC with @a s_doc = 0.
 */
#define PY_CLASS_FREE_ITERFUNC( i_cppClass, i_cppBegin, icppEnd )\
		PY_CLASS_FREE_ITERFUNC_DOC( i_cppClass, i_cppBegin, icppEnd, 0 )

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif
