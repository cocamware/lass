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

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_IO_INL
#define LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_IO_INL

#include "stde_common.h"
#include "extended_io.h"

// --- implemenation details -----------------------------------------------------------------------

namespace lass
{
namespace stde
{
namespace impl
{

template <typename Iterator, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
printSequence(std::basic_ostream<Char, Traits>& ioOStream, 
              Iterator iBegin, Iterator iEnd, 
              Char iOpen, Char iClose)
{
	std::basic_ostringstream<Char, Traits> buffer;
	buffer.copyfmt(ioOStream);
    buffer.width(0);

	buffer << iOpen;
	for (Iterator i = iBegin; i != iEnd; ++i)
	{
		if (i != iBegin)
		{
			buffer << ", ";
		}
		buffer << *i;
	}
	buffer << iClose;

	LASS_ENFORCE_STREAM(ioOStream) << buffer.str();
	return ioOStream;
}

template <typename Iterator, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
printMap(std::basic_ostream<Char, Traits>& ioOStream, 
         Iterator iBegin, Iterator iEnd,
         Char iOpen, Char iClose)
{
	std::basic_ostringstream<Char, Traits> buffer;
	buffer.copyfmt(ioOStream);
    buffer.width(0);

	buffer << iOpen;
	for (Iterator i = iBegin; i != iEnd; ++i)
	{
		if (i != iBegin)
		{
			buffer << ", ";
		}
		buffer << i->first << ": " << i->second;
	}
	buffer << iClose;

	LASS_ENFORCE_STREAM(ioOStream) << buffer.str();
	return ioOStream;
}

}

}

}

// --- free ----------------------------------------------------------------------------------------

namespace std
{

// output

/** @ingroup extended_io
 */
template <typename T1, typename T2, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::pair<T1, T2>& iPair)
{
	std::basic_ostringstream<Char, Traits> buffer;
	buffer.copyfmt(ioOStream);
    buffer.width(0);
	buffer << "(" << iPair.first << ", " << iPair.second << ")";
	LASS_ENFORCE_STREAM(ioOStream) << buffer.str();
	return ioOStream;
}

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::vector<T, Alloc>& iContainer)
{
	return lass::stde::impl::printSequence(ioOStream, iContainer.begin(), iContainer.end(), '[', ']');
}

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::list<T, Alloc>& iContainer)
{
	return lass::stde::impl::printSequence(ioOStream, iContainer.begin(), iContainer.end(), '[', ']');
}

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::deque<T, Alloc>& iContainer)
{
	return lass::stde::impl::printSequence(ioOStream, iContainer.begin(), iContainer.end(), '[', ']');
}

/** @ingroup extended_io
 */
template <typename Key, typename Data, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::map<Key, Data, Comp, Alloc>& iContainer)
{
	return lass::stde::impl::printMap(ioOStream, iContainer.begin(), iContainer.end(), '{', '}');
}

/** @ingroup extended_io
 */
template <typename Key, typename Data, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::multimap<Key, Data, Comp, Alloc>& iContainer)
{
	return lass::stde::impl::printMap(ioOStream, iContainer.begin(), iContainer.end(), '{', '}');
}

/** @ingroup extended_io
 */
template <typename Key, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::set<Key, Comp, Alloc>& iContainer)
{
    return lass::stde::impl::printSequence(ioOStream, iContainer.begin(), iContainer.end(), '{', '}');
}

/** @ingroup extended_io
 */
template <typename Key, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::multiset<Key, Comp, Alloc>& iContainer)
{
	return lass::stde::impl::printSequence(ioOStream, iContainer.begin(), iContainer.end(), '{', '}');
}

#ifdef _STLP_SLIST

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::slist<T, Alloc>& iContainer)
{
	return lass::stde::impl::printSequence(ioOStream, iContainer.begin(), iContainer.end(), '[', ']');
}

#endif



// input

/** @ingroup extended_io
 */
template <typename T1, typename T2, typename Char, typename Traits>
std::basic_istream<Char, Traits>& 
operator>>(std::basic_istream<Char, Traits>& ioIStream, 
           std::pair<T1, T2>& oPair)
{
	std::pair<T1, T2> result;
	char c;

    if (ioIStream.peek() == '(')
    {
        ioIStream.ignore();
        ioIStream >> result.first;
	}
	else
	{
		ioIStream.clear(std::ios_base::failbit);
	}

    if (ioIStream.peek() == ',')
	{
        ioIStream.ignore();
		ioIStream >> result.second;
	}
	else
	{
		ioIStream.clear(std::ios_base::failbit);
	}

    if (ioIStream.peek() == ')')
	{
        ioIStream.ignore();
    }
    else
    {
		ioIStream.clear(std::ios_base::failbit);
	}
}



}

#endif

// EOF
