/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#include "io_common.h"
#include "proxy_o_stream.h"
#include "proxy_i_stream.h"
#include "../util/non_copyable.h"

namespace lass
{
namespace io
{

class LASS_DLL ProxyMan: util::NonCopyable
{
public:

	ProxyMan();
	~ProxyMan();

	ProxyOStream* cout() { return cout_; }
	ProxyOStream* cerr() { return cerr_; }
	ProxyOStream* clog() { return clog_; }
	ProxyIStream* cin() { return cin_; }

private:

	ProxyOStream* cout_;
	ProxyOStream* cerr_;
	ProxyOStream* clog_;
	ProxyIStream* cin_;
};

LASS_DLL ProxyMan* LASS_CALL proxyMan();

}

}

#endif
