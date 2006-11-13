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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_FUN_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_THREAD_FUN_INL

#include "util_common.h"
#include "thread_fun.h"
#include "bind.h"

namespace lass
{
namespace util
{

// --- 0 arguments ---------------------------------------------------------------------------------

/** @relates ThreadFun
 */
template <typename Function>
ThreadFun* threadFun(
	Function iFunction,
	ThreadKind iKind)
{
	return new ThreadFun(bind(iFunction), iKind);
}



/** @relates ThreadFun
 */
template <typename ObjectPtr, typename Method>
ThreadFun* threadMemFun(
	ObjectPtr iObject, Method iMethod,
	ThreadKind iKind)
{
	return new ThreadFun(bind(iMethod, iObject), iKind);
}



$[
// --- $x argument(s) -----------------------------------------------------------------------------

/** @relates ThreadFun
 */
template <$(typename P$x)$, typename Function>
ThreadFun* threadFun(
	Function iFunction,
	$(const P$x& iP$x)$,
	ThreadKind iKind)
{
	return new ThreadFun(bind(iFunction, $(iP$x)$), iKind);
}



/** @relates ThreadFun
 */
template <$(typename P$x)$, typename ObjectPtr, typename Method>
ThreadFun* threadMemFun(
	ObjectPtr iObject, Method iMethod,
	$(const P$x& iP$x)$,
	ThreadKind iKind)
{
	return new ThreadFun(bind(iMethod, iObject, $(iP$x)$), iKind);
}



]$
}

}

#endif

// EOF
