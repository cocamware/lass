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
#include "proxy_man.h"
#include "../util/singleton.h"

namespace lass
{
namespace io
{

ProxyMan::ProxyMan():
	cout_(&std::cout, ProxyOStream::acceptAll),
	cerr_(&std::cerr, ProxyOStream::acceptAll),
	clog_(&std::clog, ProxyOStream::acceptAll),
	cin_(&std::cin)
{
}



/** return pointer to 'cout' proxy.
 */
ProxyOStream* ProxyMan::cout()
{
	return &cout_;
}



/** return pointer to 'cerr' proxy.
 */
ProxyOStream* ProxyMan::cerr()
{
	return &cerr_;
}



/** return pointer to 'clog' proxy.
 */
ProxyOStream* ProxyMan::clog()
{
	return &clog_;
}



/** return pointer to 'cin' proxy.
 */
ProxyIStream* ProxyMan::cin()
{
	return &cin_;
}



/** access the ProxyMan singleton
 */
ProxyMan* proxyMan()
{
	return util::Singleton<ProxyMan, 0>::instance();
}




}

}

// EOF
