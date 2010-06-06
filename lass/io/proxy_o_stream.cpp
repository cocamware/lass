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

#include "lass_common.h"
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
ProxyOStream::TMask ProxyOStream::filter( std::ostream* iDestination ) const
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
	Lock lock(this, iMessageMask);
	return lock;
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
