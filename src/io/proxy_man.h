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


/** @class lass::io::ProxyMan
 *  @brief manages the four proxy streams.
 *  @date 2003
 *
 *  Lass has four proxy streams that replace the regular std streams and lass::io::ProxyMan owns them
 *  You can access these streams by three manners:
 *  - through the four member functions lass::io::ProxyMan::cout(), lass::io::ProxyMan::clog(), 
 *    lass::io::ProxyMan::cerr() and lass::io::ProxyMan::cin() which return pointers to these streams.
 *  - through the four free functions lass::io::cout(), lass::io::cerr(), lass::io::clog() and
 *    lass::io::cin() which return references to these streams
 *  - by the four macros LASS_COUT, LASS_CERR, LASS_CLOG and LASS_CIN which return references too.
 *
 *  ProxyMan is a singleton, you can access it through ProxyMan::instance() or proxyMan(), both
 *  returning a pointer to the singleton.
 *
 *  The four proxy streams are created on first access of the ProxyMan singleton or any of its
 *  proxy streams and are automatically redirected to std::cout, std::cerr, std::clog and std::cin
 *  respectively.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_PROXY_MAN_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_PROXY_MAN_H

// included interfaces

#include "io_common.h"
#include "proxy_o_stream.h"
#include "proxy_i_stream.h"



// new interfaces

namespace lass
{
namespace io
{

class LASS_DLL ProxyMan
{
public:

	ProxyMan();

	ProxyOStream* cout();
	ProxyOStream* cerr();
	ProxyOStream* clog();
	ProxyIStream* cin();

private:

	ProxyOStream cout_;
	ProxyOStream cerr_;
	ProxyOStream clog_;
	ProxyIStream cin_;
};

LASS_DLL ProxyMan* LASS_CALL proxyMan();

}

}

#endif
