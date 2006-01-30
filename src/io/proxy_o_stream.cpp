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

#include "io_common.h"
#include "proxy_o_stream.h"



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
ProxyOStream::ProxyOStream( std::ostream* iDestination, TMask iMessageMask )
{
	add(iDestination, iMessageMask);
}



/** Add a std::ostream to the list of destination streams.
 *  - Proxy stream does NOT take ownership of destination stream, it stays your own.
 *  - If destination stream is already added to proxy, then it's not added again. i.e.
 *    You can safely add the same destination twice, but nothing will happen, it will appear
 *    only once in the proxy (and some word to the log will be written).
 */
void ProxyOStream::add( std::ostream* iDestination, TMask iMessageMask )
{
	TDestinations::iterator dit = destinations_.find(iDestination);
	if (dit == destinations_.end())
	{
		destinations_.insert(TDestinations::value_type(iDestination, iMessageMask));
	}
	else
	{
		LASS_WARNING("Trying to add std::ostream '" << iDestination << "' to ProxyOStream '" << this
			<< "' while it's already connected to the proxy.  Nothing happens.");
	}
}



/** Remove a std::ostream from the list of destination streams.
 *  - Proxy stream does not deallocate destination stream, it stays your own responsibility.
 *  - If you try to remove a destination that's not there, nothing happens. i.e. it's safe
 *    to remove a stream that the proxy doesn't has.
 */
void ProxyOStream::remove( std::ostream* iDestination )
{
	TDestinations::iterator dit = destinations_.find(iDestination);
	if (dit != destinations_.end())
	{
		destinations_.erase(dit);
	}
	else
	{
		LASS_WARNING("Trying to remove std::ostream '" << iDestination << "' from ProxyOStream '"
			<< this << "' while it's not connected to the proxy.  Nothing happens.");
	}
}



/** Return accept mask on destination stream.
 *  If destination stream does not exists in proxy, it throws an exception.
 */
const ProxyOStream::TMask ProxyOStream::filter( std::ostream* iDestination ) const
{
	TDestinations::const_iterator dit = destinations_.find(iDestination);
	if (dit == destinations_.end())
	{
		LASS_THROW("Cannot return filter because std::ostream '" << iDestination << "' is not "
			<< "connected to the ProxyOStream '" << this << "' as destination.");
	}

	return dit->second;
}



/** Set filter on destination stream.
 *  If destination stream does not exists, it throws an exception.
 */
void ProxyOStream::setFilter( std::ostream* iDestination, TMask iFilterMask)
{
	TDestinations::iterator dit = destinations_.find(iDestination);
	if (dit == destinations_.end())
	{
		LASS_THROW("Cannot set filter because std::ostream '" << iDestination << "' is not "
			<< "connected to the ProxyOStream '" << this << "' as destination.");
	}

	dit->second = iFilterMask;
}



ProxyOStream::Lock ProxyOStream::operator()( TMask iMessageMask )
{
	return Lock(this, iMessageMask);
}



/** flush all destination streams.
 */
void ProxyOStream::flush()
{
	TDestinations::iterator dit;
	for (dit = destinations_.begin(); dit != destinations_.end(); ++dit)
	{
		(*dit).first->flush();
	}
}



}

}
