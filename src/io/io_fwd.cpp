/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2004
 */

#include "io_common.h"
#include "io_fwd.h"
#include "proxy_man.h"

namespace lass
{
namespace io
{

// --- free ----------------------------------------------------------------------------------------

/** return reference to 'cout' proxy stream
 */
ProxyOStream& cout()
{
	return *proxyMan()->cout();
}



/** return reference to 'cerr' proxy stream
 */
ProxyOStream& cerr()
{
	return *proxyMan()->cerr();
}



/** return reference to 'clog' proxy stream
 */
ProxyOStream& clog()
{
	return *proxyMan()->clog();
}



/** return reference to 'cin' proxy stream
 */
ProxyIStream& cin()
{
	return *proxyMan()->cin();
}



}

}

// EOF
