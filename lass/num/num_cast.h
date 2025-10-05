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
 *	Copyright (C) 2004-2025 the Initial Developer.
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
	BadNumCast(std::string msg, std::string loc): util::ExceptionMixin<BadNumCast>(std::move(msg), std::move(loc)) {}
	~BadNumCast() noexcept {}
};

namespace impl
{

template <typename Out, typename In, bool signedOut, bool signedIn> struct NumCaster;

template <typename Out, typename In>
struct NumCaster<Out, In, true, true>
{
	// Out is signed, In is signed

	static Out cast(In in)
	{
		static_assert(num::NumTraits<In>::isSigned == true);
		static_assert(num::NumTraits<Out>::isSigned == true);

		using TCommon = std::common_type_t<In, Out, int>;
		static_assert(num::NumTraits<TCommon>::isSigned == true);

		constexpr TCommon min = num::NumTraits<Out>::min;
		constexpr TCommon max = num::NumTraits<Out>::max;
		if (static_cast<TCommon>(in) < min)
		{
			LASS_THROW_EX(BadNumCast, "not a " << num::NumTraits<Out>::name() 
				<< ": underflow: " << static_cast<TCommon>(in) << " < " << min);
		}
		if (static_cast<TCommon>(in) > max)
		{
			LASS_THROW_EX(BadNumCast, "not a " << num::NumTraits<Out>::name() 
				<< ": overflow: " << static_cast<TCommon>(in) << " > " << max);
		}
		return static_cast<Out>(in);
	}
};

template <typename Out, typename In>
struct NumCaster<Out, In, false, true>
{
	// Out is unsigned, In is signed

	static Out cast(In in)
	{
		static_assert(num::NumTraits<In>::isSigned == true);
		static_assert(num::NumTraits<Out>::isSigned == false);
		using TUnsignedIn = typename num::NumTraits<In>::unsignedType;

		using TCommon = std::common_type_t<TUnsignedIn, Out, unsigned>;
		static_assert(num::NumTraits<TCommon>::isSigned == false);

		constexpr TCommon max = num::NumTraits<Out>::max;
		if (in < 0)
		{
			using TSigned = std::common_type_t<In, int>;
			LASS_THROW_EX(BadNumCast, "not a " << num::NumTraits<Out>::name() 
				<< ": negative: " << static_cast<TSigned>(in));
		}
		if (static_cast<TCommon>(in) > max)
		{
			LASS_THROW_EX(BadNumCast, "not a " << num::NumTraits<Out>::name() 
				<< ": overflow: " << static_cast<TCommon>(in) << " > " << max);
		}
		return static_cast<Out>(in);
	}
};

template <typename Out, typename In, bool signedOut>
struct NumCaster<Out, In, signedOut, false>
{
	// In is unsigned, Out is signed or unsigned

	static Out cast(In in)
	{
		static_assert(num::NumTraits<In>::isSigned == false);
		using TUnsignedOut = typename num::NumTraits<Out>::unsignedType;

		using TCommon = std::common_type_t<In, TUnsignedOut, unsigned>;
		static_assert(num::NumTraits<TCommon>::isSigned == false);

		constexpr TCommon max = num::NumTraits<Out>::max;
		if (static_cast<TCommon>(in) > max)
		{
			LASS_THROW_EX(BadNumCast, "not a " << num::NumTraits<Out>::name() 
				<< ": overflow: " << static_cast<TCommon>(in) << " > " << max);
		}
		return static_cast<Out>(in);
	}
};

template <typename Out>
struct NumCaster<Out, Out, true, true>
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
