/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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

namespace lass
{
namespace util
{

// --- ThreadFun0 ----------------------------------------------------------------------------------

inline
ThreadFun0::ThreadFun0(
	const Callback0& iFun,
	ThreadKind iKind):

	Thread(iKind),
	fun_(iFun)
{
}



inline
void* ThreadFun0::entry()
{
	fun_();
	return 0;
}



/** @relates ThreadFun0
 *  @ingroup ThreadFun
 */
template <typename Function>
ThreadFun0* threadFun(
	Function iFunction,
	ThreadKind iKind)
{
	ThreadFun0* result = new ThreadFun0(Callback0(iFunction), iKind);
	result->create();
	result->run();
	return result;
}



/** @relates ThreadFun0
 *  @ingroup ThreadFun
 */
template <typename ObjectPtr, typename Method>
ThreadFun0* threadMemFun(
	ObjectPtr iObject, Method iMethod,
	ThreadKind iKind)
{
	ThreadFun0* result = new ThreadFun0(Callback0(iObject, iMethod), iKind);
	result->create();
	result->run();
	return result;
}



$[
// --- ThreadFun$x --------------------------------------------------------------------------------

template <$(typename P$x)$>
ThreadFun$x<$(P$x)$>::ThreadFun$x(
	const Callback$x<$(P$x)$>& iFun,
	$(typename CallTraits<P$x>::TParam iP$x)$,
	ThreadKind iKind):

	Thread(iKind),
	fun_(iFun),
	$(p$x_(iP$x))$
{
}



template <$(typename P$x)$>
void* ThreadFun$x<$(P$x)$>::entry()
{
	fun_($(p$x_)$);
	return 0;
}



/** @relates ThreadFun$x
 *  @ingroup ThreadFun
 */
template <$(typename P$x)$, typename Function>
ThreadFun$x<$(P$x)$>* threadFun(
	Function iFunction,
	$(const P$x& iP$x)$,
	ThreadKind iKind)
{
	ThreadFun$x<$(P$x)$>* result =
		new ThreadFun$x<$(P$x)$>(Callback$x<$(P$x)$>(iFunction), $(iP$x)$, iKind);
	result->create();
	result->run();
	return result;
}



/** @relates ThreadFun$x
 *  @ingroup ThreadFun
 */
template <$(typename P$x)$, typename ObjectPtr, typename Method>
ThreadFun$x<$(P$x)$>* threadMemFun(
	ObjectPtr iObject, Method iMethod,
	$(const P$x& iP$x)$,
	ThreadKind iKind)
{
	ThreadFun$x<$(P$x)$>* result =
		new ThreadFun$x<$(P$x)$>(Callback$x<$(P$x)$>(iObject, iMethod), $(iP$x)$, iKind);
	result->create();
	result->run();
	return result;
}



]$
}

}

#endif

// EOF
