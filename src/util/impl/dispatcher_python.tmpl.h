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

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H

#include "../py_tuple.h"

namespace lass
{
namespace util
{
namespace impl
{

LASS_DLL std::string LASS_CALL fetchException(); // see dispatcher_python.cpp

}
}
}

#endif



namespace lass
{
namespace util
{
namespace impl
{

#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_0_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_0
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_0

/** Dispatcher for lass::util::Callback0 to an python callback.
 *  @internal
 *  @sa Callback0
 *  @author Bram de Greve [Bramz]
 */
class Dispatcher0Python
{
public:

	Dispatcher0Python(const python::TPyObjPtr& iCallable):
		callable_(iCallable)
	{
	}

	void operator()() const
	{
		LASS_ASSERT(callable_);
		python::TPyObjPtr result(PyObject_CallObject(callable_.get(), 0));
		if (!result)
		{
			LASS_THROW(fetchException());
		}
	}

	bool operator==(const Dispatcher0Python& iOther) const
	{
		return callable_.get() == iOther.callable_.get();
	}

private:

	python::TPyObjPtr callable_;
};

#	endif
#endif

$[
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_$x_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_$x
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_$x

/** Dispatcher for lass::util::Callback$x to an python callback.
 *  @internal
 *  @sa Callback$x
 *  @author Bram de Greve [Bramz]
 */
template <$(typename P$x)$>
class Dispatcher$xPython
{
public:

	Dispatcher$xPython(const python::TPyObjPtr& iCallable):
		callable_(iCallable)
	{
	}

	void operator()($(typename util::CallTraits<P$x>::TParam iP$x)$) const
	{
		LASS_ASSERT(callable_);
		python::TPyObjPtr args(python::makeTuple($(iP$x)$));
		python::TPyObjPtr result(PyObject_CallObject(callable_.get(), args.get()));
		if (!result)
		{
			LASS_THROW(fetchException());
		}
	}

private:

	python::TPyObjPtr callable_;
};

#	endif
#endif
]$

#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_R_0_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_R0
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_R0

/** Dispatcher for lass::util::CallbackR0 to an python callback.
 *  @internal
 *  @sa CallbackR0
 *  @author Bram de Greve [Bramz]
 */
template <typename R>
class DispatcherR0Python
{
public:

	DispatcherR0Python(const python::TPyObjPtr& iCallable):
		callable_(iCallable)
	{
	}

	R operator()() const
	{
		LASS_ASSERT(callable_);
		python::TPyObjPtr result(PyObject_CallObject(callable_.get(), 0));
		if (!result)
		{
			LASS_THROW(fetchException());
		}

		typedef python::impl::ArgumentTraits<R> TraitsR;
		typename TraitsR::TStorage temp;
		if (python::pyGetSimpleObject(result.get(), temp) != 0)
		{
			LASS_THROW("bad result");
		}
		return TraitsR::arg(temp);
	}

private:

	python::TPyObjPtr callable_;
};

#	endif
#endif

$[
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_CALLBACK_R_$x_H
#	ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_R$x
#	define LASS_GUARDIAN_OF_INCLUSION_UTIL_IMPL_DISPATCHER_PYTHON_H_R$x

/** Dispatcher for lass::util::CallbackR$x to an python callback.
 *  @internal
 *  @sa CallbackR$x
 *  @author Bram de Greve [Bramz]
 */
template <typename R, $(typename P$x)$>
class DispatcherR$xPython
{
public:

	DispatcherR$xPython(const python::TPyObjPtr& iCallable):
		callable_(iCallable)
	{
	}

	R operator()($(typename util::CallTraits<P$x>::TParam iP$x)$) const
	{
		LASS_ASSERT(callable_);
		python::TPyObjPtr args(python::makeTuple($(iP$x)$));
		python::TPyObjPtr result(PyObject_CallObject(callable_.get(), args.get()));
		if (!result)
		{
			LASS_THROW(fetchException());
		}

		typedef python::impl::ArgumentTraits<R> TraitsR;
		typename TraitsR::TStorage temp;
		if (python::pyGetSimpleObject(result.get(), temp) != 0)
		{
			LASS_THROW("bad result");
		}
		return TraitsR::arg(temp);
	}

private:

	python::TPyObjPtr callable_;
};

#	endif
#endif
]$

}

}

}

// EOF
