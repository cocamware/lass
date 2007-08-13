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
 *		Software—Practice and Experience, 2007, 37:231-246, 16 s, DOI: 10.1002/spe.754
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
class FutureBindError: public util::Exception
{
public:
	FutureBindError(const std::string& msg, const std::string& loc): util::Exception(msg, loc) {}
private:
	LASS_UTIL_EXCEPTION_PRIVATE_IMPL(FutureBindError)
};



template <typename T>
class Future
{
public:

	Future(): pimpl_(new Impl) {}

	const bool isBound() const 
	{
		TLocker lock(pimpl_->mutex_);
		return pimpl_->isBound_ || pimpl_->isBadAlloc_ || pimpl_->error_.get(); 
	}
	const bool operator!() const { return !isBound(); }
	operator num::SafeBool() const { return isBound() ? num::safeFalse : num::safeTrue; }

	void wait() 
	{
		while (!isBound())
		{
			pimpl_->condition_.wait();
		}
	}
	const WaitResult wait(unsigned long milliSeconds)
	{
		return pimpl_->condition_.wait() == waitSuccess && isBound() ? waitSuccess : waitTimeout;
	}
	
	const T& operator()() const
	{
		wait();
		TLocker lock(pimpl_->mutex_);
		if (pimpl_->isBadAlloc_)
		{
			throw std::bad_alloc("::lass::Future::bindError: failed to clone exception");
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

	void error(const experimental::RemoteExceptionBase& error)
	{
		TLocker lock(pimpl_->mutex_);
		try
		{
			pimpl_->error_ = error.clone();
		}
		catch (std::bad_alloc)
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
			pimpl_->error_ = new experimental::RemoteExceptionWrapper<ExceptionType>(error);
		}
		catch (std::bad_alloc)
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
		std::auto_ptr<experimental::RemoteExceptionBase> error_;
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
