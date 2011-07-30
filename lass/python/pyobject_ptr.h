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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PYTHON_PYOBJECT_PTR_H
#define LASS_GUARDIAN_OF_INCLUSION_PYTHON_PYOBJECT_PTR_H

#include "python_common.h"
#include "gil.h"
#include "../util/shared_ptr.h"
#include "../util/singleton.h"
#include "../util/thread.h"

namespace lass
{
namespace python
{

class PyObjectPlus;

namespace impl
{
	/** @internal
	 *  Fast reentrant lock for reference count operations
	 */
	inline util::CriticalSection& referenceMutex()
	{
		using namespace util;
		return *Singleton<CriticalSection, destructionPriorityInternalPythonMutex>::instance();
	}

	template <typename PyObjectPtr>
	void forceObjectType(PyObjectPtr object, PyTypeObject* type)
	{
		if (object && object->ob_type != type)
		{
			Py_XINCREF(type);
			std::swap(object->ob_type, type);
			Py_XDECREF(type);
		}
	}

	LASS_PYTHON_DLL void LASS_CALL doFixObjectType(PyObjectPlus* object);
	inline void doFixObjectType(const PyObjectPlus* object) { doFixObjectType(const_cast<PyObjectPlus*>(object)); }
	inline void doFixObjectType(const PyObject*) {}

	/** @internal
	 *  On creation, PyObjectPlus are typeless (ob_type == 0), 
	 *  call this function to fix that for you.  
	 *  Objects created in C++ should call this at least once before
	 *  being used in Python.
	 */
	template <typename T> T* fixObjectType(T* object)
	{
		doFixObjectType(object);
		return object;
	}
}



/** Recommended storage policy for single PyObject objects, implementation of StoragePolicy concept
*  @ingroup Python
*  @author Tom De Muer [TDM]
*  @see ObjectStorage
*/
template <typename T, typename Cascade = meta::EmptyType>
class PyObjectStorage: public Cascade
{
public:

	typedef PyObjectStorage<T, Cascade> TSelf;
	typedef T* TStorage;
	typedef T* TPointer;
	typedef T& TReference;

	TStorage& storage() { return storage_; }
	const TStorage& storage() const { return storage_; }

protected:

	PyObjectStorage(): Cascade(), storage_(defaultStorage()) {}
	explicit PyObjectStorage(T* pointee): Cascade(), storage_(impl::fixObjectType(pointee)) {}
	PyObjectStorage(const PyObjectStorage& other): Cascade(other), storage_(other.storage_) {}
	template <typename U> PyObjectStorage(const PyObjectStorage<U, Cascade>& other): 
		Cascade(other), storage_(other.storage()) {}
	TPointer pointer() const { return storage_; }
	void dispose() { storage_ = 0; }
	bool isNull() const { return !storage_; }
	void swap(TSelf& other) { Cascade::swap(other); std::swap(storage_, other.storage_);   }
	static TStorage defaultStorage() { return 0; }
	template <typename U> const PyObjectStorage<U, Cascade> staticCast() const 
	{ 
		return PyObjectStorage<U, Cascade>(static_cast<U*>(storage_), *this);
	}
	template <typename U> const PyObjectStorage<U, Cascade> dynamicCast() const
	{
		if (U* p = dynamic_cast<U*>(storage_))
		{		
			return PyObjectStorage<U, Cascade>(p, *this);
		}
		return PyObjectStorage<U, Cascade>();
	}
	template <typename U> const PyObjectStorage<U, Cascade> constCast() const 
	{ 
		return PyObjectStorage<U, Cascade>(const_cast<U*>(storage_), *this);
	}
private:
	template <typename U, typename C> friend class PyObjectStorage;
	PyObjectStorage(T* pointee, const Cascade& cascade): Cascade(cascade), storage_(pointee) {}
	TStorage storage_;
};



/** The recommended counter for the pyobject pointers, implementation of CounterPolicy concept.
*  @ingroup Python
*  @author Tom De Muer [TDM]
*  @see DefaultCounter
*/
class LASS_PYTHON_DLL PyObjectCounter
{
public:
	typedef int TCount;
protected:
	PyObjectCounter() {}
	template <typename T> void init(T* /*pointee*/) {}
	template <typename T> void dispose(T* /*pointee*/) {}
	template <typename T> void increment(T* pointee)
	{
		if (Py_IsInitialized())
		{
			LockGIL LASS_UNUSED(lock);
			Py_INCREF(pointee);
		}
		else
		{
			Py_INCREF(pointee);
		}
	}
	template <typename T> void increment(const T* pointee)
	{
		increment(const_cast<T*>(pointee));
	}
	template <typename T> bool decrement(T* pointee)
	{
		LASS_ASSERT(pointee);
		bool r;
		if (Py_IsInitialized())
		{
			LockGIL LASS_UNUSED(lock);
			r = pointee->ob_refcnt <=1;
			Py_DECREF(pointee);
		}
		else
		{
			r = pointee->ob_refcnt <=1;
			Py_DECREF(pointee);
		}
		return r;
	}
	template <typename T> bool decrement(const T* pointee)
	{
		return decrement(const_cast<T*>(pointee));
	}
	template <typename T> TCount count(T* pointee) const
	{
		LASS_ASSERT(pointee);
		return pointee->ob_refcnt;
	}
	void swap(PyObjectCounter& /*other*/) {}
};



/** templated "typedef" to a python shared pointer
 *  @ingroup Python
 */
template<class T>
struct PyObjectPtr
{
	typedef util::SharedPtr<T, PyObjectStorage, PyObjectCounter> Type;
};



/** PyObjectPtr to a PyObject
 *  @ingroup Python
 *
 *  Due to historical reasons, the names PyObjectPtr and TPyObjPtr are a bit
 *  confusing: TPyObjPtr is a PyObjectPtr for PyObject, while PyObjectPtr
 *  is the general one.
 */
typedef PyObjectPtr<PyObject>::Type TPyObjPtr;



/** fromNakedToSharedPtrCast.
*   @ingroup Python
*   Helper function casting a PyObject coming from the Python interface to a SharedPtr
*   object for use in C++.  Reference counts are taken care of (it's icremented by one).
*/
template<class T>
lass::util::SharedPtr<T, PyObjectStorage, PyObjectCounter>
fromNakedToSharedPtrCast(PyObject* object)
{
	LockGIL LASS_UNUSED(lock);
	Py_XINCREF(object);
	return util::SharedPtr<T,PyObjectStorage,PyObjectCounter>(static_cast<T*>(object));
}



/** fromSharedPtrToNakedCast.
*   @ingroup Python
*   Helper function casting an object used in C++ for use in Python.  The key operation
*   done here is to take care of the reference counting. Failing to use this function may
*   yield unexpected reference count.
*/
template<class T>
PyObject*
fromSharedPtrToNakedCast(const util::SharedPtr<T,PyObjectStorage,PyObjectCounter>& object)
{
	LockGIL LASS_UNUSED(lock);
	PyObject* const obj = object.get();
	Py_XINCREF(obj);
	return obj;
}



/** static_cast for python pointers
 *  @ingroup Python
 */  
template <typename Out, typename In> inline
Out	staticPyCast(const In& in)
{
	typedef typename Out::TPointer TPtr;
	LockGIL LASS_UNUSED(lock);
	TPtr ptr = static_cast<TPtr>(in.get());
	Py_XINCREF(ptr);
	return Out(ptr);
}



/** dynamic_cast for python pointers
 *  @ingroup Python
 */  
template <typename Out, typename In> inline
Out	dynamicPyCast(const In& in)
{
	typedef typename Out::TPointer TPtr;
	LockGIL LASS_UNUSED(lock);
	TPtr ptr = dynamic_cast<TPtr>(in.get());
	Py_XINCREF(ptr);
	return Out(ptr);
}

}
}

#endif

// EOF
