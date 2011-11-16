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
 *	Copyright (C) 2004-2011 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_NUM_CAST_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_NUM_CAST_H

#include "num_common.h"

namespace lass
{
namespace num
{

class BadNumCast: public util::ExceptionMixin<BadNumCast>
{
public:
	BadNumCast(const std::string& msg, const std::string& loc): util::ExceptionMixin<BadNumCast>(msg, loc) {}
	~BadNumCast() throw() {}
};

namespace impl
{

template <typename Out, typename In, bool signedOut, bool signedIn>
struct NumCaster
{
	static Out cast(In in)
	{
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_INTEL
		volatile Out min = num::NumTraits<Out>::min;
		volatile Out max = num::NumTraits<Out>::max;
#else
		const Out min = num::NumTraits<Out>::min;
		const Out max = num::NumTraits<Out>::max;
#endif
		if (in < min)
		{
			LASS_THROW_EX(BadNumCast, "not a " << num::NumTraits<Out>::name() 
				<< ": underflow: " << in << " < " << min);
		}
		if (in > max)
		{
			LASS_THROW_EX(BadNumCast, "not a " << num::NumTraits<Out>::name() 
				<< ": overflow: " << in << " > " << max);
		}
		return static_cast<Out>(in);
	}
};

template <typename Out, typename In, bool signedIn>
struct NumCaster<Out, In, false, signedIn>
{
	static Out cast(In in)
	{
		typedef typename num::NumTraits<In>::unsignedType TUnsignedIn;
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_INTEL
		volatile Out max = num::NumTraits<Out>::max;
#else
		const Out max = num::NumTraits<Out>::max;
#endif
		if (in < 0)
		{
			LASS_THROW_EX(BadNumCast, "not a " << num::NumTraits<Out>::name() 
				<< ": negative: " << in);
		}
		if (static_cast<TUnsignedIn>(in) > max)
		{
			LASS_THROW_EX(BadNumCast, "not a " << num::NumTraits<Out>::name() 
				<< ": overflow: " << in << " > " << max);
		}
		return static_cast<Out>(in);
	}
};

template <typename Out, typename In, bool signedOut>
struct NumCaster<Out, In, signedOut, false>
{
	static Out cast(In in)
	{
		typedef typename num::NumTraits<Out>::unsignedType TUnsignedOut;
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_INTEL
		volatile TUnsignedOut max = static_cast<TUnsignedOut>(num::NumTraits<Out>::max);
#else
		const TUnsignedOut max = static_cast<TUnsignedOut>(num::NumTraits<Out>::max);
#endif
		if (in > max)
		{
			LASS_THROW_EX(BadNumCast, "not a " << num::NumTraits<Out>::name() 
				<< ": overflow: " << in << " > " << max);
		}
		return static_cast<Out>(in);
	}
};

template <typename Out, typename In>
struct NumCaster<Out, In, false, false>
{
	static Out cast(In in)
	{
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_INTEL
		volatile Out max = num::NumTraits<Out>::max;
#else
		const Out max = num::NumTraits<Out>::max;
#endif
		if (in > max)
		{
			LASS_THROW_EX(BadNumCast, "not a " << num::NumTraits<Out>::name() 
				<< ": overflow: " << in << " > " << max);
		}
		return static_cast<Out>(in);
	}
};

template <typename Out, bool signedOut>
struct NumCaster<Out, Out, signedOut, signedOut>
{
	static Out cast(Out in) { return in; }
};

template <typename Out>
struct NumCaster<Out, Out, false, false>
{
	static Out cast(Out in) { return in; }
};

}

template <typename Out, typename In>
Out numCast(In in)
{
	return impl::NumCaster<Out, In, NumTraits<Out>::isSigned, NumTraits<In>::isSigned>::cast(in);
}

}
}

#endif

// EOF
