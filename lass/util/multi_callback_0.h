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



/** @defgroup Callback Callback
 *  @brief library to group generalized callback functions 
 *  @date 2009
 *  @author Tom De Muer (contact: tom@cocamware.com)
 *
 */

#include <algorithm>
#include "callback_0.h"

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_MULTI_CALLBACK_0_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_MULTI_CALLBACK_0_H

namespace lass
{
namespace util
{

struct MultiCallback0
{
	typedef MultiCallback0 TSelf;
	typedef Callback0 TCallback;
	
	/** Default constructor, construct empty callback list.
	 */
	MultiCallback0() 
	{
	}

	/** Constructor: construct multi callback with a single callback.
	*/
	MultiCallback0(const TCallback& iCallback) 
	{
		callbacks_.push_back(iCallback);
	}

	/** Constructor: construct multi callback with a two callbacks.
	*/
	MultiCallback0(const TCallback& iCallback1, const TCallback& iCallback2) 
	{
		callbacks_.push_back(iCallback1);
		callbacks_.push_back(iCallback2);
	}

	/** copy constructor
	 */
	MultiCallback0(const TSelf& iOther):
		callbacks_(iOther.callbacks_)
	{
	}

	/** move constructor
	 */
	MultiCallback0(TSelf&& iOther) noexcept:
		callbacks_(std::move(iOther.callbacks_))
	{
	}

	/** assignment operator (also usable for conversion)
	 */
	TSelf& operator=(const TSelf& iOther)
	{
		TSelf temp(iOther);
		swap(temp);
		return *this;
	}

	/** move assignment operator
	 */
	TSelf& operator=(TSelf&& iOther) noexcept
	{
		callbacks_ = std::move(iOther.callbacks_);
		return *this;
	}

	/** add a callback to the list of callbacks to be dispatched
	 */
	void add( const TCallback& iCallback )
	{
		callbacks_.push_back(iCallback);
	}

	/** removes a callback from the list of callbacks to be dispatched, 
	 *	callbacks are tested for equivalence.  
	 */
	void remove( const TCallback& iCallback )
	{
		static_cast<void>(std::remove( callbacks_.begin(), callbacks_.end(), iCallback ));
	}

	/** pop a callback from the list of callbacks 
	 */
	void pop( TCallback& iCallback )
	{
		iCallback = callbacks_.back();
		callbacks_.pop_back();
	}

	/** add a callback to the list of callbacks to be dispatched
	 */
	TSelf& operator += ( const TCallback& iCallback )
	{
		callbacks_.push_back(iCallback);
		return *this;
	}

	/** removes a callback from the list of callbacks to be dispatched
	 */
	TSelf& operator -= ( const TCallback& iCallback )
	{
		remove(iCallback);
		return *this;
	}

	/** the number of callbacks that will be dispatched
	*/
	size_t size() const
	{
		return callbacks_.size();
	}

	/** THE operator.  Executes the multi-callback.
	 */
	void operator()() const
	{
		call();
	}
	void call() const
	{
		size_t nrCallbacks = callbacks_.size();
		for (size_t i=0;i<nrCallbacks;++i)
			callbacks_[i]();
	}

	/** retrieve a callback */
	const TCallback& operator[](size_t index) const
	{
		return callbacks_.at(index);
	}
	

	// METHODS

	/** Reset to empty callback.
	 */
	void reset()
	{
		callbacks_.clear();
	}

	/** Returns true if no callback dispatcher is assigned to this object.
	 */
	bool isEmpty() const
	{
		return callbacks_.size()==0;
	}

	/** return this->isEmpty()
	 */
	bool operator!() const
	{
		return isEmpty(); 
	}

	/** return !this->isEmpty())
	 */
	explicit operator bool() const
	{
		return !isEmpty();
	}

	/** Swaps the dispatchers of this multi_callback with the dispatchers of another.
	 */
	void swap(TSelf& iOther)
	{
		callbacks_.swap(iOther.callbacks_);
	}

	/** return true if two callbacks call the same function/method,
	 *  NEEDS RTTI!
	 */
	bool operator==(const TSelf& iOther) const
	{
		size_t nrCallbacks = callbacks_.size();
		if (nrCallbacks != iOther.callbacks_.size())
			return false;
		for (size_t i=0;i<nrCallbacks;++i)
			if (callbacks_[i]!=iOther.callbacks_[i])
				return false;
		return true;
	}
private:
	std::vector<TCallback> callbacks_;
};

}
}

#endif

// EOF
