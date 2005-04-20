/** @class lass::util::ScopedPtr
 *  @ingroup SmartPtr
 *  @brief Smart pointer with simple sole ownership of single objects, non-copyable.
 *  @author Bram de Greve
 *  @date 2003
 *
 *  To be honest, this is nothing but a copy of boost::scoped_ptr, see http://www.boost.org.
 *  So, let's quote boost for they have the best explanation of what ScopedPtr is:
 *
 *  <i>"The scoped_ptr class template stores a pointer to a dynamically allocated object.
 *  (Dynamically allocated objects are allocated with the C++ new expression.) The object
 *  pointed to is guaranteed to be deleted, either on destruction of the scoped_ptr, or via
 *  an explicit reset. See the example.
 *
 *  The scoped_ptr template is a simple solution for simple needs. It supplies a basic
 *  "resource acquisition is initialization" facility, without shared-ownership or
 *  transfer-of-ownership semantics. Both its name and enforcement of semantics (by being
 *  noncopyable) signal its intent to retain ownership solely within the current scope.
 *  Because it is noncopyable, it is safer than shared_ptr or std::auto_ptr for pointers
 *  which should not be copied.
 *
 *  Because scoped_ptr is simple, in its usual implementation every operation is as fast as
 *  for a built-in pointer and it has no more space overhead that a built-in pointer.
 *
 *  scoped_ptr cannot be used in C++ Standard Library containers. Use shared_ptr if you need
 *  a smart pointer that can.
 *
 *  scoped_ptr cannot correctly hold a pointer to a dynamically allocated array. See
 *  scoped_array for that usage.
 *
 *  The class template is parameterized on T, the type of the object pointed to. T must meet
 *  the smart pointer common requirements."</i>
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_SCOPED_PTR_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_SCOPED_PTR_H



#include "util_common.h"
#include "smart_ptr_policies.h"
#include "non_copyable.h"
#include "../num/safe_bool.h"


namespace lass
{

namespace util
{

template
<
	typename T,
	template <typename, typename> class StoragePolicy = ObjectStorage
>
class ScopedPtr: public StoragePolicy<T, NonCopyable>
{
public:

	typedef ScopedPtr<T, StoragePolicy> TSelf;                  /**< type of @c *this */

	typedef T TPointee;                                         /**< type of object pointed too. */
	typedef StoragePolicy<T, NonCopyable> TStoragePolicy;       /**< type of used storage policy. */

	typedef typename TStoragePolicy::TStorage TStorage;      /**< storage object of pointee */
	typedef typename TStoragePolicy::TPointer TPointer;      /**< pointer to pointee */
	typedef typename TStoragePolicy::TReference TReference;  /**< reference to pointee */

	/** Rebind smart pointer with this policies to another type.
	 */
	template <typename U>
	struct Rebind
	{
		typedef ScopedPtr<U, StoragePolicy> Type;
	};


	/** Default constructor initializes to a default set by storage policy, usually NULL.
	 */
	ScopedPtr():
		TStoragePolicy()
	{
	}

	/** Constructs from iPointee allocated in agreement with storage policy or be @c NULL.
	 */
	explicit ScopedPtr(T* iPointee):
		TStoragePolicy(iPointee)
	{
	}

	/** Constructs by stealing pointer from a std::auto_ptr.
	 */
	ScopedPtr(std::auto_ptr<T> iOther):
		TStoragePolicy(iOther.release())
	{
	}

	/** Destructor always disposes pointee.
	 */
	~ScopedPtr()
	{
		TStoragePolicy::dispose();
	}

	/** reset the scoped pointer to default by disposing old pointee and setting to default.
	 */
	void reset()
	{
		TSelf temp;
		swap(temp);
	}

	/** reset the scoped pointer by disposing old pointee and taking new one.
	 */
	void reset(T* iPointee)
	{
		TSelf temp(iPointee);
		swap(temp);
	}

	/** reset the scoped pointer by disposing old pointee and taking new one from std::auto_ptr.
	 */
	void reset(std::auto_ptr<T> iOther)
	{
		TSelf temp(iOther);
		swap(temp);
	}

	/** return pointer to pointee.
	 */
	TPointer get() const
	{
		return TStoragePolicy::pointer();
	}

	/** access pointee as a pointer.
	 */
	TPointer operator->() const
	{
		LASS_ASSERT(TStoragePolicy::pointer());
		return TStoragePolicy::pointer();
	}

	/** access pointee as a reference.
	 */
	TReference operator*() const
	{
		LASS_ASSERT(TStoragePolicy::pointer());
		return *TStoragePolicy::pointer();
	}

	/** access iIndex'th object in pointee.
	 *
	 *  This obviously only works if the pointee is an array.  For this kind of pointees you should
	 *  use a @c StoragePolicy like ArrayStorage which implements @c at(size_t) (ObjectStorage
	 *  doesn't implement that one), and which uses @c delete[] to deallocate the pointee.
	 */
	TReference operator[](size_t iIndex) const
	{
		LASS_ASSERT(TStoragePolicy::pointer());
		return TStoragePolicy::at(iIndex);
	}

	/** access by reference the storage object that stores the link to the pointee.
	 *  This will usually return a reference to a pointer, @c T*&.
	 *
	 *  @warning You can screw up things seriously if you change the storage object.
	 *           DON'T DO THIS UNLESS YOU'RE ABSOLUTELY SURE YOU KNOW WHAT YOU'RE DOING!
	 */
	TStorage& storage()
	{
		return TStoragePolicy::storage();
	}

	/** return by reference the storage object that stores the link to the pointee.
	 *  This will usually return a constant reference to a pointer, const T*&.
	 */
	const TStorage& storage() const
	{
		return TStoragePolicy::storage();
	}

	/** return true if pointer doesn't own any object.
	 *  @return this->get() == 0.
	 */
	bool isEmpty() const
	{
		return TStoragePolicy::isNull();
	}

	/** return true if pointer doesn't own any object (same as isEmpty).
	 *  @return this->get() == 0.
	 */
	bool operator!() const
	{
		return isEmpty();
	}

	/** evaluates to true if pointer owns an object (only in boolean context).
	 *  @return this->get() != 0.
	 *  @sa num::SafeBool
	 */
	operator num::SafeBool() const
	{
		return isEmpty() ? num::safeFalse : num::safeTrue;
	}

	/** exchange the pointees between two shared pointers
	 */
	void swap(TSelf& iOther)
	{
		TStoragePolicy::swap(static_cast<TStoragePolicy&>(iOther));
	}

};



}

}




#endif




