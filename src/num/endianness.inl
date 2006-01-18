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



#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_ENDIANNESS_INL
#define LASS_GUARDIAN_OF_INCLUSION_NUM_ENDIANNESS_INL

namespace lass
{
namespace num
{

// --- implementation ------------------------------------------------------------------------------

namespace impl
{

template <size_t numOfBytes>
struct Revertor
{

	static void revert(void* ioIn) 
	{ 
		std::reverse(static_cast<Tuint8*>(ioIn), static_cast<Tuint8*>(ioIn) + numOfBytes); 
	}
};

template <>
struct Revertor<0>
{
	static void revert(void* ioIn)
	{
	}
};

template <>
struct Revertor<1>
{
	static void revert(void* /* ioIn */)
	{
	}
};

template <>
struct Revertor<2>
{
	static void revert(void* ioIn)
	{
		Tuint16& temp = *static_cast<Tuint16*>(ioIn);
		temp = ((temp & 0x00ff) << 8) | (temp >> 8);
	}
};

template <>
struct Revertor<4>
{
	static void revert(void* ioIn)
	{
		Tuint32& temp = *static_cast<Tuint32*>(ioIn);
		temp = ((temp & 0x000000ff) << 24) | ((temp & 0x0000ff00) << 8) |
			((temp & 0x00ff0000) >> 8) | (temp >> 24);
	}
};



template <size_t numOfBytes, Endianness outEndian, Endianness inEndian>
struct EndianCaster
{
	inline static void cast(void* ioIn) 
	{
		Revertor<numOfBytes>::revert(ioIn);
	}
};

template <size_t numOfBytes, Endianness endian>
struct EndianCaster<numOfBytes, endian, endian>
{
	inline static void cast(void* ioIn)
	{
	}
};

}

// --- free ----------------------------------------------------------------------------------------

template <Endianness outEndian, Endianness inEndian, typename T> 
inline T endianCast(T iIn)
{
	impl::EndianCaster<sizeof(T), outEndian, inEndian>::cast(reinterpret_cast<void*>(&iIn));
	return iIn;
}

template <typename T>
inline T fixEndianness(T iIn, Endianness iEndianness)
{
	return iEndianness == systemEndian ? iIn : endianCast<bigEndian, littleEndian>(iIn);
}

}

}

#endif

// EOF
