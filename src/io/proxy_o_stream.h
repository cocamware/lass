/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



/** @class lass::io::ProxyOStream
 *	@brief A proxy output stream can distribute output to multiple destination streams.
 *	@author BdG
 *	@date 2003
 *
 *  You can add multiple destination streams to a proxy stream, and all output to the
 *  proxy stream will be redirected to all these destinations.
 *
 *  You can also assign a filter to the proxy, and only messages with levels set in the 
 *  filter will pass.  e.g. if you set the filter of the proxy as 
 *  proxy.setFilter() = 0, then only messages of warning or error levels will 
 *  pass.
 * 
 *  How do you tell what level your message has?  Simple, before you use the insertor,
 *  you call the function operator on the stream with the correct level.  This returns
 *  a lock on the proxy, and now you send the message to the lock (by the insertor)
 *  Wheter this message will pass or not, is determined by the lock.
 *  At the end of the lifetime of the lock, the proxy is also flushed.
 *
 *  proxy(Warning) << "This is a warning: " << theWarning;
 *
 *  In the above example, the proxy is locked in warning level, and the string and
 *  theWarning is passed to the lock.  At the end, the lock flushes te proxy.
 *
 *  If you don't use the function operator to set the message level, the level is implicit
 *  set to ProxyOStream::Note.  
 *
 *  OK, seriously, i have no longer any idea why we need this ... get rid of it?
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_PROXY_O_STREAM_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_PROXY_O_STREAM_H




#include "io_common.h"
#include "../util/bit_manip.h"


namespace lass
{

namespace io
{

class LASS_DLL_EXPORT ProxyOStream
{
public:

	typedef unsigned TMask;
	static const TMask acceptAll = (TMask) -1;

	// innerclasses

	class Lock
	{
	public:
		/** Lock (or don't lock if iProxy == 0) a proxy.
		 */
		Lock( ProxyOStream* iProxy, TMask iMessage ): proxy_(iProxy), messageMask_(iMessage) {}

		/** The copy constructor passes the lock on the proxy to the copy. 
		 *  The pointer is passed to the copy, so that this copy can continue the job of the
		 *  original.  Since the original looses it's proxy pointer, it won't flush it on 
		 *  destruction.
		 *  @warning DO NOT COPY LOCKS YOURSELF.  It's no good, well, unless you know what you're
		 *           doing.  But be warned: it might not do what you expect. 
		 */
		Lock( const ProxyOStream::Lock& iOther ): proxy_(iOther.proxy_), messageMask_(iOther.messageMask_)
		{
			iOther.proxy_ = 0;
			iOther.messageMask_ = 0;
		}

		/** On the end of the lock, flush the proxy.
		 */
		ProxyOStream::Lock::~Lock()
		{
			if (proxy_)
			{
				proxy_->flush();
			}
		}

		/** distribute input over all destination streams.
		*  Only distribute input if it's still locking a proxy (i.e. if proxy_ != Null).
		*/
		template <typename T> Lock& operator<< ( const T& iIn )
		{
			if (proxy_)
			{
				ProxyOStream::TDestinations::iterator dit;
				for (dit = proxy_->destinations_.begin(); dit != proxy_->destinations_.end(); ++dit)
				{
					if (util::checkMaskedSome(dit->second, messageMask_))
					{
						LASS_ENFORCE_STREAM(*(dit->first)) << iIn;
					}
				}
			}
			return *this;
		}

		/** distribute std manipulators over all destination streams.
		*  Only distribute input if it's still locking a proxy (i.e. if proxy_ != Null).
		*/
		Lock& operator<<(std::ostream& (*iIn) (std::ostream&))
		{
			if (proxy_)
			{
				ProxyOStream::TDestinations::iterator dit;
				for (dit = proxy_->destinations_.begin(); dit != proxy_->destinations_.end(); ++dit)
				{
					if (util::checkMaskedSome(dit->second, messageMask_))
					{
						LASS_ENFORCE_STREAM(*(dit->first)) << iIn;
					}
				}
			}
			return *this;
		}

	private:
		Lock& operator=( const Lock& iOther ) { LASS_ASSERT(false); return *this; }
		mutable ProxyOStream* proxy_;
		mutable TMask messageMask_;
	};


	// structors & methods
	
	//ProxyOStream();
	ProxyOStream( std::ostream* iDestination = &std::cout, TMask iFilterMask = acceptAll);

	void add( std::ostream* iDestination, TMask iFilterMask = acceptAll );
	void remove( std::ostream* iDestination );

	const TMask filter( std::ostream* iDestination ) const;
	void setFilter( std::ostream* iDestination, TMask iFilterMask  );

	Lock operator()( TMask iFilterMask = acceptAll );

	/** Lock proxy stream for output on 'acceptAll' and distribute input.
	 *  This command will give command to a lock, as if the message mask is acceptAll.
	 *  This has the same effect as (*this)(acceptAll) << iIn;
	 */
	template <typename T> Lock operator<< ( const T& iIn )
	{
		Lock result(this, acceptAll);
		result << iIn;
		return result;
	}

	/** Lock proxy stream for output on 'acceptAll' and distribute the std manipulator.
	 *  This command will give command to a lock, as if the message mask is acceptAll.
	 *  This has the same effect as (*this)(acceptAll) << iIn;
	 */
	Lock operator<< ( std::ostream& (*iIn) (std::ostream&) )
	{
		Lock result(this, acceptAll);
		result << iIn;
		return result;
	}

	void flush();

private:

	friend class Lock;

    typedef std::map<std::ostream*, TMask> TDestinations;
    
	TDestinations destinations_;
};



}

}

#endif
