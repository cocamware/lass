/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_IO_INL
#define LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_IO_INL

#include "stde_common.h"
#include "extended_io.h"
#include "../util/string_cast.h"

#include <cctype>

// --- implemenation details -----------------------------------------------------------------------

namespace lass
{
namespace stde
{
namespace impl
{

template <typename Char, typename Traits>
void eatWhitespace(std::basic_istream<Char, Traits>& ioIStream)
{
	Char c = 0;
	while (ioIStream.get(c))
	{
		if (!std::isspace(c))
		{
			ioIStream.putback(c);
			break;
		}
	}
}



struct SequenceTraits
{
	template <typename Container, typename T>
	static void push(Container& oContainer, const T& iElement)
	{
		oContainer.push_back(iElement);
	}
};

struct SetTraits
{
	template <typename Container, typename T>
	static void push(Container& oContainer, const T& iElement)
	{
		oContainer.insert(iElement);
	}
};

struct ElementTraits
{
	template <typename Char, typename Traits, typename T>
	static bool read(std::basic_istream<Char, Traits>& ioIStream, T& oElement,
					 Char iSeperatorInter, Char iSeperatorIntra, Char iClose)
	{
		eatWhitespace(ioIStream);

		// read till next seperator
		//
		std::basic_string<Char, Traits> buffer;
		Char c = 0;
		while (ioIStream.get(c))
		{
			if (c == iSeperatorInter || c == iClose)
			{
				ioIStream.putback(c);
				break;
			}
			buffer += c;
		}

		// convert
		//
		try
		{
			oElement = util::stringCast<T>(buffer);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
};

struct PairTraits
{
	template <typename Char, typename Traits, typename T, typename U>
	static bool read(std::basic_istream<Char, Traits>& ioIStream, std::pair<T, U>& oElement,
					 Char iSeperatorInter, Char iSeperatorIntra, Char iClose)
	{
		T t;
		if (!ElementTraits::read<Char>(ioIStream, t, iSeperatorIntra, 0, iSeperatorIntra))
		{
			return false;
		}
		ioIStream.ignore();
		U u;
		if (!ElementTraits::read<Char>(ioIStream, u, iSeperatorInter, 0, iClose))
		{
			return false;
		}
		oElement = std::pair<T, U>(t, u);
		return true;
	}
};

template <typename Iterator, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
printSequence(std::basic_ostream<Char, Traits>& ioOStream, 
              Iterator iBegin, Iterator iEnd, 
              const Char* iOpen, const Char* iSeperator, const Char* iClose)
{
	std::basic_ostringstream<Char, Traits> buffer;
	buffer.copyfmt(ioOStream);
    buffer.width(0);

	buffer << iOpen;
	for (Iterator i = iBegin; i != iEnd; ++i)
	{
		if (i != iBegin)
		{
			buffer << iSeperator;
		}
		buffer << *i;
	}
	buffer << iClose;

	LASS_ENFORCE_STREAM(ioOStream) << buffer.str();
	return ioOStream;
}

template <typename Char, typename Traits, typename Iterator>
std::basic_ostream<Char, Traits>& 
printMap(std::basic_ostream<Char, Traits>& ioOStream, 
         Iterator iBegin, Iterator iEnd,
         const Char* iOpen, const Char* iSeperator1, const Char* iSeperator2, const Char* iClose)
{
	std::basic_ostringstream<Char, Traits> buffer;
	buffer.copyfmt(ioOStream);
    buffer.width(0);

	buffer << iOpen;
	for (Iterator i = iBegin; i != iEnd; ++i)
	{
		if (i != iBegin)
		{
			buffer << iSeperator1;
		}
		buffer << i->first << iSeperator2 << i->second;
	}
	buffer << iClose;

	LASS_ENFORCE_STREAM(ioOStream) << buffer.str();
	return ioOStream;
}


template
<
	typename Pusher, typename Reader, typename T,
	typename Char, typename Traits, 
	typename Container
>
std::basic_istream<Char, Traits>& 
readContainer(std::basic_istream<Char, Traits>& ioIStream, Container& oContainer, 
			 Char iOpen, Char iSeperatorInter, Char iSeperatorIntra, Char iClose)
{
	Container result;

	bool good = true;
	Char c = 0;
	eatWhitespace(ioIStream);
	if (ioIStream.get(c))
	{
		if (c == iOpen)
		{
			T temp;
			good = Reader::read(ioIStream, temp, iSeperatorInter, iSeperatorIntra, iClose);
			if (good)
			{        
				Pusher::push(result, temp);
			}
		}
		else
		{
			ioIStream.putback(c);
			good = false;
		}
	}
	else
	{
		good = false;
	}

	while (good && ioIStream.get(c) && c != iClose)
	{
		if (c == iSeperatorInter)
		{
			T temp;
			good = Reader::read(ioIStream, temp, iSeperatorInter, iSeperatorIntra, iClose);
			if (good)
			{        
				Pusher::push(result, temp);
			}
		}
		else
		{
			ioIStream.putback(c);
			good = false;
		}
	}

	if (good)
	{
		oContainer.swap(result);
	}
	else
	{
		ioIStream.clear(std::ios::failbit);
	}

	return ioIStream;
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
	return lass::stde::impl::printSequence(
		ioOStream, iContainer.begin(), iContainer.end(), "[", ", ", "]");
}

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::list<T, Alloc>& iContainer)
{
	return lass::stde::impl::printSequence(
		ioOStream, iContainer.begin(), iContainer.end(), "[", ", ", "]");
}

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::deque<T, Alloc>& iContainer)
{
	return lass::stde::impl::printSequence(
		ioOStream, iContainer.begin(), iContainer.end(), "[", ", ", "]");
}

/** @ingroup extended_io
 */
template <typename Key, typename Data, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::map<Key, Data, Comp, Alloc>& iContainer)
{
	return lass::stde::impl::printMap<Char>(
		ioOStream, iContainer.begin(), iContainer.end(), "{", ", ", ": ", "}");
}

/** @ingroup extended_io
 */
template <typename Key, typename Data, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::multimap<Key, Data, Comp, Alloc>& iContainer)
{
	return lass::stde::impl::printMap<Char>(
		ioOStream, iContainer.begin(), iContainer.end(), "{", ", ", ": ", "}");
}

/** @ingroup extended_io
 */
template <typename Key, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::set<Key, Comp, Alloc>& iContainer)
{
    return lass::stde::impl::printSequence(
		ioOStream, iContainer.begin(), iContainer.end(), "{", ", ", "}");
}

/** @ingroup extended_io
 */
template <typename Key, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::multiset<Key, Comp, Alloc>& iContainer)
{
	return lass::stde::impl::printSequence(
		ioOStream, iContainer.begin(), iContainer.end(), "{", ", ", "}");
}

#ifdef _STLP_SLIST

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::slist<T, Alloc>& iContainer)
{
	return lass::stde::impl::printSequence(
		ioOStream, iContainer.begin(), iContainer.end(), "[", ", ", "]");
}

#endif



// input

/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename T1, typename T2>
std::basic_istream<Char, Traits>& 
operator>>(std::basic_istream<Char, Traits>& ioIStream, 
           std::pair<T1, T2>& oPair)
{
	using namespace lass::stde;

	Char c = 0;
	if (ioIStream.get(c))
	{
		if (c == '(')
		{
			std::pair<T1, T2> temp;
			if (impl::PairTraits::read<Char>(ioIStream, temp, ')', ',', ')'))
			{        
				oPair = temp;
				ioIStream.ignore();
			}
		}
		else
		{
			ioIStream.putback(c);
			ioIStream.clear(std::ios::failbit);
		}
	}
	else
	{
		ioIStream.clear(std::ios::failbit);
	}

	return ioIStream;
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename T, typename Alloc>
std::basic_istream<Char, Traits>& 
operator>>(std::basic_istream<Char, Traits>& ioIStream, 
           std::vector<T, Alloc>& oContainer)
{
	using namespace lass::stde;
	return impl::readContainer<impl::SequenceTraits, impl::ElementTraits, T, Char>(
		ioIStream, oContainer, '[', ',', 0, ']');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename T, typename Alloc>
std::basic_istream<Char, Traits>& 
operator>>(std::basic_istream<Char, Traits>& ioIStream, 
           std::list<T, Alloc>& oContainer)
{
	using namespace lass::stde;
	return impl::readContainer<impl::SequenceTraits, impl::ElementTraits, T, Char>(
		ioIStream, oContainer, '[', ',', 0, ']');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename T, typename Alloc>
std::basic_istream<Char, Traits>& 
operator>>(std::basic_istream<Char, Traits>& ioIStream, 
           std::deque<T, Alloc>& oContainer)
{
	using namespace lass::stde;
	return impl::readContainer<impl::SequenceTraits, impl::ElementTraits, T, Char>(
		ioIStream, oContainer, '[', ',', 0, ']');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename Key, typename Data, typename Comp, typename Alloc>
std::basic_istream<Char, Traits>& 
operator>>(std::basic_istream<Char, Traits>& ioIStream, 
           std::map<Key, Data, Comp, Alloc>& oContainer)
{
	using namespace lass::stde;
	return impl::readContainer<impl::SetTraits, impl::PairTraits, std::pair<Key, Data>, Char>(
		ioIStream, oContainer, '{', ',', ':', '}');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename Key, typename Data, typename Comp, typename Alloc>
std::basic_istream<Char, Traits>& 
operator>>(std::basic_istream<Char, Traits>& ioIStream, 
           std::multimap<Key, Data, Comp, Alloc>& oContainer)
{
	using namespace lass::stde;
	return impl::readContainer<impl::SetTraits, impl::PairTraits, std::pair<Key, Data>, Char>(
		ioIStream, oContainer, '{', ',', ':', '}');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename Key, typename Comp, typename Alloc>
std::basic_istream<Char, Traits>& 
operator>>(std::basic_istream<Char, Traits>& ioIStream, 
           std::set<Key, Comp, Alloc>& oContainer)
{
	using namespace lass::stde;
	return impl::readContainer<impl::SetTraits, impl::ElementTraits, Key, Char>(
		ioIStream, oContainer, '{', ',', 0, '}');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename Key, typename Comp, typename Alloc>
std::basic_istream<Char, Traits>& 
operator>>(std::basic_istream<Char, Traits>& ioIStream, 
           std::multiset<Key, Comp, Alloc>& oContainer)
{
	using namespace lass::stde;
	return impl::readContainer<impl::SetTraits, impl::ElementTraits, Key, Char>(
		ioIStream, oContainer, '{', ',', 0, '}');
}



}

#endif

// EOF
