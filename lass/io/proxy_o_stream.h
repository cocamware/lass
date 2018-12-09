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
 *	Copyright (C) 2004-2018 the Initial Developer.
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



/** @class lass::io::ProxyOStream
 *  @brief A proxy output stream can distribute output to multiple destination streams.
 *  @author BdG
 *  @date 2003
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

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(push)
#	pragma warning(disable: 4267) // 'argument' : conversion from 'size_t' to 'unsigned int', possible loss of data
#endif

namespace lass
{
namespace io
{

class ProxyOStream;

namespace impl
{

class LASS_DLL ProxyOStreamLock
{
public:
	typedef unsigned TMask;

	ProxyOStreamLock(ProxyOStream* proxy, TMask messageMask);
	ProxyOStreamLock(ProxyOStreamLock& other);
	~ProxyOStreamLock();

	template <typename T> ProxyOStreamLock& operator<< (const T& x);
	ProxyOStreamLock& operator<<(std::ostream& (*x) (std::ostream&));

private:
	ProxyOStreamLock & operator=(const ProxyOStreamLock& /*other*/);

	ProxyOStream* proxy_;
	TMask messageMask_;
	static volatile int semaphore_;
};

}


class LASS_DLL ProxyOStream
{
public:

	typedef unsigned TMask;
	static const TMask acceptAll = static_cast<TMask>(-1);

	// structors & methods

	//ProxyOStream();
	ProxyOStream(std::ostream* destination = &std::cout, TMask filterMask = acceptAll);

	void add(std::ostream* destination, TMask filterMask = acceptAll);
	void remove(std::ostream* destination);

	TMask filter(std::ostream* destination) const;
	void setFilter(std::ostream* destination, TMask filterMask);

	impl::ProxyOStreamLock operator()(TMask filterMask = acceptAll);

	/** ProxyOStreamLock proxy stream for output on 'acceptAll' and distribute input.
	 *  This command will give command to a lock, as if the message mask is acceptAll.
	 *  This has the same effect as (*this)(acceptAll) << x;
	 */
	template <typename T> impl::ProxyOStreamLock operator<< (const T& x)
	{
		impl::ProxyOStreamLock result(this, acceptAll);
		result << x;
		return result;
	}

	impl::ProxyOStreamLock operator<< (std::ostream& (*x) (std::ostream&));

	void flush();

private:
	friend class impl::ProxyOStreamLock;

	struct Destination
	{
		std::ostream* stream;
		TMask mask;
	};

	typedef std::vector<Destination> TDestinations;

	TDestinations::iterator findStream(std::ostream* stream);

	TDestinations destinations_;
};


namespace impl
{

/** distribute input over all destination streams.
 *  Only distribute input if it's still locking a proxy (i.e. if proxy_ != Null).
 */
template <typename T>
ProxyOStreamLock& ProxyOStreamLock::operator<< (const T& x)
{
	if (proxy_)
	{
		for (ProxyOStream::TDestinations::iterator i = proxy_->destinations_.begin(), end = proxy_->destinations_.end(); i != end; ++i)
		{
			if (util::checkMaskedSome(i->mask, messageMask_))
			{
				LASS_ENFORCE_STREAM(*(i->stream)) << x;
			}
		}
	}
	return *this;
}

}

}

}

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(pop)
#endif

#endif
