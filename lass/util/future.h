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
 *	Copyright (C) 2004-2023 the Initial Developer.
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

/** Transporting Values and Exceptions between Threads
 *  @class lass::util::experimental::Future
 *
 *  See:
 *  @arg Peter Dimov: "Transporting Values and Exceptions between Threads", 
 *		http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n2096.html
 *	@arg Matti Rintala: "Handling Multiple Concurrent Exceptions in C++ Using Futures", 
 *		teoksessa Advanced Topics in Exception Handling Techniques, 
 *		toim. C. Dony, J. L. Knudsen, A. Romanovsky, A. Tripathi. 
 *		LNCS 4419, 301 s, ISBN 3-540-37443-4, DOI 10.1007/11818502_4, Springer-Verlag 2006
 *		http://www.cs.tut.fi/cgi-bin/run/bitti/download/lncs-eh-draft.pdf
 *	@arg Matti Rintala: "Exceptions in remote procedure calls using C++ template metaprogramming", 
 *		Software�Practice and Experience, 2007, 37:231-246, 16 s, DOI: 10.1002/spe.754
 *		http://www.cs.tut.fi/cgi-bin/run/bitti/download/spe-mmr-preprint.pdf
 */  
 
#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_FUTURE_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_FUTURE_H

#include "util_common.h"
#include "shared_ptr.h"

namespace lass
{
namespace util
{
namespace experimental
{

/** @ingroup Thread
 *  @relates Future
 */
class FutureBindError: public ExceptionMixin<FutureBindError>
{
public:
	FutureBindError(std::string msg, std::string loc): ExceptionMixin<FutureBindError>(std::move(msg), std::move(loc)) {}
	~FutureBindError() noexcept {}
};



template <typename T>
class Future
{
public:

	Future(): pimpl_(new Impl) {}

	bool isBound() const 
	{
		TLocker lock(pimpl_->mutex_);
		return pimpl_->isBound_ || pimpl_->isBadAlloc_ || pimpl_->error_.get(); 
	}
	bool operator!() const { return !isBound(); }
	explicit operator bool() const { return isBound(); }

	void wait() 
	{
		while (!isBound())
		{
			pimpl_->condition_.wait();
		}
	}
	WaitResult wait(unsigned long milliSeconds)
	{
		return pimpl_->condition_.wait(milliSeconds) == waitSuccess && isBound() ? waitSuccess : waitTimeout;
	}
	
	const T& operator()() const
	{
		wait();
		TLocker lock(pimpl_->mutex_);
		if (pimpl_->isBadAlloc_)
		{
			throw std::bad_alloc();
		}
		if (pimpl_->error_.get())
		{
			pimpl_->error_->throwSelf();
		}
		return *reinterpret_cast<T*>(pimpl_->value_);
	}

	void bind(const T& value)
	{
		TLocker lock(pimpl_->mutex_);
		if (pimpl_->isBound())
		{
			LASS_THROW_EX(FutureBindError, "Future is already bound");
		}
		new (reinterpret_cast<T*>(pimpl_->value_)) T(value);
		pimpl_->isBound_ = true;
		pimpl_->condition_.broadcast();
	}
	void unbind()
	{
		TLocker lock(pimpl_->mutex_);
		pimpl_->unbind();
	}

	void error(const RemoteExceptionBase& error)
	{
		TLocker lock(pimpl_->mutex_);
		try
		{
			pimpl_->error_ = error.clone();
		}
		catch (const std::bad_alloc&)
		{
			pimpl_->isBadAlloc_ = true;
		}
		pimpl_->condition_.broadcast();
	}
	template <typename ExceptionType>
	void error(const ExceptionType& error)
	{
		TLocker lock(pimpl_->mutex_);
		try
		{
			pimpl_->error_ = new RemoteExceptionWrapper<ExceptionType>(error);
		}
		catch (const std::bad_alloc&)
		{
			pimpl_->isBadAlloc_ = true;
		}
		pimpl_->condition_.broadcast();
	}

private:

	typedef util::Semaphore TMutex;
	typedef util::Locker<TMutex> TLocker;

	struct Impl
	{
		char value_[sizeof(T)];
		TRemoteExceptionBasePtr error_;
		Condition condition_;
		TMutex mutex_;
		size_t referenceCount_;
		volatile bool isBound_;
		volatile bool isBadAlloc_;

		Impl(): isBound_(false) {}
		~Impl() 
		{
			unbind();
		}
		void unbind()
		{
			if (isBound_)
			{
				reinterpret_cast<T*>(value_)->~T();
				isBound_ = false;
			}
		}
	};

	typedef util::IntrusiveCounter<Impl, size_t, &Impl::referenceCount_> TCounterPolicy;
	typedef util::SharedPtr<Impl, util::ObjectStorage, TCounterPolicy> TImplPtr;

	TImplPtr pimpl_;
};

}
}
}

#endif
