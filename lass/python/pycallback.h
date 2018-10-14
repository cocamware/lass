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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYCALLBACK_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYCALLBACK_H

#include "python_common.h"
#include "pyshadow_object.h"
#include "pyiteratorrange.h"
#include "container.h"
#include "pyobject_util.h"
#include <map>
#include "../stde/access_iterator.h"
#include "../meta/type_traits.h"

#include "../util/multi_callback.h"
#include "callback_python.h"

namespace lass
{
namespace python
{

class MultiCallback;

namespace impl
{

	class LASS_PYTHON_DLL MultiCallbackImplBase
	{
	public:
		virtual ~MultiCallbackImplBase() {};
		virtual void reset() = 0;
		virtual Py_ssize_t length() const = 0;
		virtual const std::type_info& type() const = 0;
		virtual void* raw(bool writable) = 0;
		virtual void call(const python::TPyObjPtr& args, PyObject* self) = 0;
		virtual void add(const python::TPyObjPtr& args) = 0;

		const std::string repr() const;
	};


	template <typename Callback> 
	class MultiCallbackImpl : public MultiCallbackImplBase
	{
	public:
		typedef Callback TCallback;
		typedef util::SharedPtr<Callback> TCallbackPtr;
		typedef util::SharedPtr<const Callback> TConstCallbackPtr;
	
		MultiCallbackImpl( const TCallbackPtr & callback, bool readonly) : callback_(callback), readOnly_(readonly)
		{
		}

		virtual void reset() { return callback_->reset(); }									// move to traits class, to discern multi and non-multi
		virtual Py_ssize_t length() const { return static_cast<Py_ssize_t>(callback_->size()); };				// move to traits class, to discern multi and non-multi

		const std::type_info& type() const 
		{ 
			return typeid(TCallbackPtr); 
		}
		void* raw(bool writable)
		{ 
			if (writable && readOnly_)
			{
				return 0;
			}
			return &callback_; 
		}
		virtual void call(const python::TPyObjPtr& args, PyObject* self);
		virtual void add(const python::TPyObjPtr& args)
		{
			// the "thing" we add must be convertible to a callback
			// [TODO] use a decodeTuple
			LockGIL LASS_UNUSED(lock);
			Callback tempCallback;
			if (args.get() && PyTuple_Check(args.get()) && PyTuple_Size(args.get())==1)
			{
				Py_XINCREF(args.get());
				Py_XINCREF(PyTuple_GetItem(args.get(),0));
				int rv = pyGetSimpleObject(PyTuple_GetItem(args.get(),0),tempCallback);
				if (rv)
				{
					impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
				}
				for (size_t i=0;i<tempCallback.size();++i)
					callback_->add(tempCallback[i]);
			}
		}
	protected:
		TCallbackPtr callback_;
		bool readOnly_;
	};
}



class MultiCallback;
typedef PyObjectPtr<MultiCallback>::Type TMultiCallbackPtr;

/** MultiCallback.  Object for interfacing MultiCallbacks with Python 
*/
class LASS_PYTHON_DLL MultiCallback : public PyObjectPlus, util::NonCopyable
{
	PY_HEADER(PyObjectPlus)
public:
	template <typename CallbackType> MultiCallback( const util::SharedPtr<CallbackType>& callback)
	{
		// [TODO] untested
		LASS_THROW("unsupported");
		TPimpl pimpl(
			new impl::MultiCallbackImpl<CallbackType>(callback,false));
		init(pimpl);
	}
	template <typename CallbackType> MultiCallback( const util::SharedPtr<const CallbackType>& /*callback*/)
	{
		// [TODO] untested
		LASS_THROW("unsupported");
		/*TPimpl pimpl(
			new impl::MultiCallbackImpl<CallbackType>(p,true));
		init(pimpl);*/
	}
	template<typename CallbackType> MultiCallback( const CallbackType& callback )
	{
		util::SharedPtr<CallbackType> p(new CallbackType(callback));
#pragma LASS_FIXME("This should be the read-only version")
		TPimpl pimpl(new impl::MultiCallbackImpl<CallbackType>(p,false));
		init(pimpl);
	}
	~MultiCallback();

	std::string repr(void) const;
	void reset();

	void call(const python::TPyObjPtr& args);
	void add(const python::TPyObjPtr& args);
	PyObject* callVar(PyObject* args);
	PyObject* addVar(PyObject* args);

	static Py_ssize_t length(PyObject* self);

	const std::type_info& type() const;
	void* raw(bool writable) const;

private:

#if LASS_HAVE_STD_AUTO_PTR
	typedef std::auto_ptr<impl::MultiCallbackImplBase> TPimpl;
#elif LASS_HAVE_STD_UNIQUE_PTR
	typedef std::unique_ptr<impl::MultiCallbackImplBase> TPimpl; 
#else
#	error "Must have either std::auto_ptr or std::unique_ptr"
#endif

	MultiCallback(TPimpl& pimpl);
	void init(TPimpl& pimpl);
	static void initializeType();
	static PyObject * _tp_call(PyObject *, PyObject *, PyObject *); 

	util::ScopedPtr<impl::MultiCallbackImplBase> pimpl_;
	static bool isInitialized;
};

namespace impl
{
	template <typename Callback>
	struct CallIntermediateShadowTraits
	{
		typedef Callback TCallback;
		typedef const Callback* TConstCallbackPtr;

		typedef TCallback TCppClass;
		typedef TConstCallbackPtr TConstCppClassPtr;

		static int getObject(PyObject* self, TConstCppClassPtr& forCalling)
		{
			lass::util::SharedPtr<TCallback>* almost = static_cast<lass::util::SharedPtr<TCallback>*>(static_cast<MultiCallback*>(self)->raw(false));
			forCalling = almost->get();
			return 0;
		}
	};

	template <typename C>
	void MultiCallbackImpl<C>::call(const python::TPyObjPtr& args, PyObject* self)
	{
		LockGIL LASS_UNUSED(lock); // as we're going to release it again ...
		impl::CallMethod< CallIntermediateShadowTraits<C> >::call(args.get(), self, &C::call ); 
	}
}

}
}

#include "pycallback_export_traits.inl"

#endif

// EOF
