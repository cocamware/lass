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

#include "lass_common.h"
#include "proxy_o_stream.h"
#include "../util/atomic.h"


namespace lass
{

namespace io
{

/** Construct a proxy with a destination, and set a filter for it
 *//*
ProxyOStream::ProxyOStream()
{
	add(&std::cout, acceptAll);
}
*/
/** Construct a proxy with a destination, and set a filter for it
 */
ProxyOStream::ProxyOStream( std::ostream* destination, TMask destinationMask )
{
	add(destination, destinationMask);
}



/** Add a std::ostream to the list of destination streams.
 *  - Proxy stream does NOT take ownership of destination stream, it stays your own.
 *  - If destination stream is already added to proxy, then it's not added again. i.e.
 *    You can safely add the same destination twice, but it will only set the mask.
 */
void ProxyOStream::add( std::ostream* destination, TMask destinationMask )
{
	TDestinations::iterator i = findStream(destination);
	if (i != destinations_.end())
	{
		i->mask = destinationMask;
	}
	else
	{
		Destination dest = { destination, destinationMask };
		destinations_.push_back(dest);
	}
}



/** Remove a std::ostream from the list of destination streams.
 *  - Proxy stream does not deallocate destination stream, it stays your own responsibility.
 *  - If you try to remove a destination that's not there, nothing happens. i.e. it's safe
 *    to remove a stream that the proxy doesn't has.
 */
void ProxyOStream::remove( std::ostream* destination )
{
	TDestinations::iterator i = findStream(destination);
	if (i == destinations_.end())
	{
		return;
	}
	destinations_.erase(i);
}



/** Return accept mask on destination stream.
 *  If destination stream does not exists in proxy, it throws an exception.
 */
ProxyOStream::TMask ProxyOStream::filter( std::ostream* destination ) const
{
	for (TDestinations::const_iterator i = destinations_.begin(), end = destinations_.end(); i != end; ++i)
	{
		if (i->stream == destination)
		{
			return i->mask;
		}
	}
	LASS_THROW("Cannot return filter because std::ostream '" << destination << "' is not "
		<< "connected to the ProxyOStream '" << this << "' as destination.");
}



/** Set filter on destination stream.
 *  If destination stream does not exists, it throws an exception.
 */
void ProxyOStream::setFilter( std::ostream* destination, TMask destinationMask)
{
	TDestinations::iterator i = findStream(destination);
	if (i == destinations_.end())
	{
		LASS_THROW("Cannot set filter because std::ostream '" << destination << "' is not "
			<< "connected to the ProxyOStream '" << this << "' as destination.");
	}
	i->mask = destinationMask;
}



ProxyOStream::Lock ProxyOStream::operator()( TMask messageMask )
{
	Lock lock(this, messageMask);
	return lock;
}



/** flush all destination streams.
 */
void ProxyOStream::flush()
{
	for (TDestinations::iterator i = destinations_.begin(), end = destinations_.end(); i != end; ++i)
	{
		i->stream->flush();
	}
}


ProxyOStream::TDestinations::iterator ProxyOStream::findStream(std::ostream* stream)
{
	// We put things in a map with a O(n) search because:
	// - adding/removing/looking up a destination stream happens rarely
	// - n is small (often 1)
	// - iterating over all streams happens frequently, and vectors are bleeding fast for that ...
	for (TDestinations::iterator i = destinations_.begin(), end = destinations_.end(); i != end; ++i)
	{
		if (i->stream == stream)
		{
			return i;
		}
	}
	return destinations_.end();
}


// --- Lock ----------------------------------------------------------------------------------------

volatile int ProxyOStream::Lock::semaphore_ = 1;

/** Lock (or don't lock if proxy == 0) a proxy.
 */
ProxyOStream::Lock::Lock(ProxyOStream* proxy, TMask messageMask): 
	proxy_(proxy), 
	messageMask_(messageMask) 
{
	util::atomicLock(semaphore_);
}


/** The copy constructor passes the lock on the proxy to the copy.
 *  The pointer is passed to the copy, so that this copy can continue the job of the
 *  original.  Since the original looses it's proxy pointer, it won't flush it on
 *  destruction.
 *  @warning DO NOT COPY LOCKS YOURSELF.  It's no good, well, unless you know what you're
 *           doing.  But be warned: it might not do what you expect.
 */
ProxyOStream::Lock::Lock(Lock& other): 
	proxy_(other.proxy_), 
	messageMask_(other.messageMask_)
{
	other.proxy_ = 0;
	other.messageMask_ = 0;

	// this also implicitly transfers the ownership of the semaphore lock.
	// the other won't unlock it anymore because it not longer has a proxy_.
}


/** On the end of the lock, flush the proxy.
 */
ProxyOStream::Lock::~Lock()
{
	if (proxy_)
	{
		proxy_->flush();
		util::atomicUnlock(semaphore_);
	}
}


}

}
