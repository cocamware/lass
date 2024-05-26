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
 *	Copyright (C) 2004-2024 the Initial Developer.
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



/** @class lass::prim::XYZ
 *  @brief cyclic iterator over xyz indices
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_XYZ_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_XYZ_H

#include "prim_common.h"
#include "../num/modulo.h"

namespace lass
{
namespace prim
{

class XYZ
{
public:

	static constexpr size_t dimension = 3;

	/** intializes iterator to @a x axis.
	 */
	constexpr XYZ(): value_(0) {}

	/** initializes iterator to an axis by character: 'x', 'y' or 'z'.
 */
	constexpr XYZ(char axis)
	{
		switch (axis)
		{
		case 0:
		case 'x':
		case 'X':
			value_ = 0;
			break;

		case 1:
		case 'y':
		case 'Y':
			value_ = 1;
			break;

		case 2:
		case 'z':
		case 'Z':
			value_ = 2;
			break;

		default:
			throw util::Exception("Invalid parameter axis.  Try 'x', 'X', 'y', 'Y', 'z' or 'Z'.", LASS_PRETTY_FUNCTION);
		}
	}

	/** initializes iterator to an axis by number.
	 *  ..., -1 == @a z, 0 == @a x, 1 == @a y, 2 == @a z, 3 == @a x, ...
	 */
	explicit constexpr XYZ(int value): value_(value) {}

	/** initializes iterator to an axis by character: "x", "y" or "z".
	 */
	explicit XYZ(const std::string& axis)
	{
		if (axis.length() != 1)
		{
			LASS_THROW("Invalid parameter axis '" << axis << "'.  It must be a single character.");
		}
		*this = XYZ(axis[0]);
	}

	constexpr XYZ(const XYZ& other) = default;
	constexpr XYZ& operator=(const XYZ& other) = default;

	/** return axis by character: 'x', 'y' or 'z'.
 	 */
	constexpr char axis() const
	{
		LASS_ASSERT(value_ >= 0 && value_ < static_cast<int>(dimension));
		constexpr char axes[] = { 'x', 'y', 'z' };
		return axes[value_];
	}

	constexpr operator int() const { return value_; } /**< convert axis to integer. 0 == @a x, 1 == @a y, 2 == @a z */
	constexpr operator size_t() const { return static_cast<size_t>(value_); } /**< convert axis to integer. 0 == @a x, 1 == @a y, 2 == @a z */

	constexpr XYZ& operator++()
	{
		++value_;
		return *this;
	}
	constexpr XYZ& operator--()
	{
		--value_;
		return *this;
	}

	constexpr XYZ operator++(int)
	{
		const XYZ result(*this);
		++*this;
		return result;
	}
	constexpr XYZ operator--(int)
	{
		const XYZ result(*this);
		--*this;
		return result;
	}

	constexpr XYZ& operator+=(int offset)
	{
		value_ += offset;
		return *this;
	}
	constexpr XYZ& operator-=(int offset)
	{
		value_ -= offset;
		return *this;
	}

	constexpr XYZ operator+(int offset) const
	{
		return XYZ(value_ + offset);
	}
	constexpr XYZ operator-(int offset) const
	{
		return XYZ(value_ - offset);
	}

	constexpr bool operator==(XYZ other) const
	{
		return value_ == other.value_;
	}
	constexpr bool operator==(int other) const
	{
		return static_cast<int>(value_) == other;
	}
	constexpr bool operator==(size_t other) const
	{
		return static_cast<size_t>(value_) == other;
	}
	constexpr bool operator==(char other) const
	{
		return *this == XYZ(other);
	}
	bool operator==(const std::string& other) const
	{
		return *this == XYZ(other);
	}

	constexpr bool operator!=(XYZ other) const
	{
		return value_ != other.value_;
	}
	constexpr bool operator!=(int other) const
	{
		return static_cast<int>(value_) != other;
	}
	constexpr bool operator!=(size_t other) const
	{
		return static_cast<size_t>(value_) != other;
	}
	constexpr bool operator!=(char other) const
	{
		return *this != XYZ(other);
	}
	bool operator!=(const std::string& other) const
	{
		return *this != XYZ(other);
	}

private:

	typedef num::Modulo<dimension, int> TValue;

	TValue value_;
};

/** @relates lass::prim::XYZ
 */
inline constexpr bool operator==(int a, const XYZ& b)
{
	return b == a;
}

/** @relates lass::prim::XYZ
 */
inline constexpr bool operator==(size_t a, const XYZ& b)
{
	return b == a;
}

/** @relates lass::prim::XYZ
 */
inline constexpr bool operator==(char a, const XYZ& b)
{
	return b == a;
}

/** @relates lass::prim::XYZ
 */
inline bool operator==(const std::string& a, const XYZ& b)
{
	return b == a;
}

/** @relates lass::prim::XYZ
 */
inline constexpr bool operator!=(int a, const XYZ& b)
{
	return b != a;
}

/** @relates lass::prim::XYZ
 */
inline constexpr bool operator!=(size_t a, const XYZ& b)
{
	return b != a;
}

/** @relates lass::prim::XYZ
 */
inline constexpr bool operator!=(char a, const XYZ& b)
{
	return b != a;
}

/** @relates lass::prim::XYZ
 */
inline bool operator!=(const std::string&  a, const XYZ& b)
{
	return b != a;
}

/** @relates lass::prim::XYZ
 */
template <typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& stream, const XYZ& xyz)
{
	stream << xyz.axis();
	return stream;
}



}

}

#define LASS_PRIM_HAVE_PY_EXPORT_TRAITS_XYZ
#ifdef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYOBJECT_PLUS_H
#	include "../python/export_traits_prim.h"
#endif

#endif

// EOF
