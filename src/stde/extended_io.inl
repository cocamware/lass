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



#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_IO_INL
#define LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_IO_INL

#include "stde_common.h"
#include "extended_io.h"
#include "../util/string_cast.h"

#include <cctype>

// --- implemenation details -----------------------------------------------------------------------

namespace lass
{

// a forward declaration in case string_cast.h doesn't make it ... 
//
namespace util
{
template <typename Out, typename In> Out stringCast(const In& iIn);
}

namespace stde
{
namespace impl
{

template <typename Char, typename Traits>
void eat_whitespace(std::basic_istream<Char, Traits>& istream)
{
	Char c = 0;
	while (istream.get(c))
	{
		if (!std::isspace(c))
		{
			istream.putback(c);
			break;
		}
	}
}



struct sequence_traits
{
	template <typename Container, typename T>
	static void push(Container& container, const T& value)
	{
		container.push_back(value);
	}
	template <typename Container>
	static void temp_to_output(Container& temp, Container& output)
	{
		temp.swap(output);
	}
};

struct set_traits
{
	template <typename Container, typename T>
	static void push(Container& container, const T& value)
	{
		container.insert(value);
	}
	template <typename Container>
	static void temp_to_output(Container& temp, Container& output)
	{
		temp.swap(output);
	}
};

struct value_traits
{
	template <typename Char, typename Traits, typename T>
	static bool read(std::basic_istream<Char, Traits>& istream, T& value,
					 Char inter_seperator, Char /*intra_seperator*/, Char closer)
	{
		eat_whitespace(istream);

		// read till next seperator
		//
		std::basic_string<Char, Traits> buffer;
		Char c = 0;
		while (istream.get(c))
		{
			if (c == inter_seperator || c == closer)
			{
				istream.putback(c);
				break;
			}
			buffer += c;
		}

		// convert
		//
		try
		{
			value = util::stringCast<T>(buffer);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
};

struct pair_traits
{
	template <typename Char, typename Traits, typename T, typename U>
	static bool read(std::basic_istream<Char, Traits>& istream, std::pair<T, U>& value,
					 Char inter_seperator, Char intra_seperator, Char closer)
	{
		std::pair<T, U> result;
		if (!value_traits::read<Char>(istream, result.first, intra_seperator, 0, intra_seperator))
		{
			return false;
		}
		istream.ignore();
		if (!value_traits::read<Char>(istream, result.second, inter_seperator, 0, closer))
		{
			return false;
		}
		value = result;
		return true;
	}
};

template <typename Iterator, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
print_sequence(std::basic_ostream<Char, Traits>& ostream,
			  Iterator begin, Iterator end,
			  const Char* opener, const Char* seperator, const Char* closer)
{
	std::basic_ostringstream<Char, Traits> buffer;
	buffer.copyfmt(ostream);
	buffer.width(0);

	buffer << opener;
	for (Iterator i = begin; i != end; ++i)
	{
		if (i != begin)
		{
			buffer << seperator;
		}
		buffer << *i;
	}
	buffer << closer;

	LASS_ENFORCE_STREAM(ostream) << buffer.str();
	return ostream;
}

template <typename Char, typename Traits, typename Iterator>
std::basic_ostream<Char, Traits>&
print_map(std::basic_ostream<Char, Traits>& ostream,
		  Iterator begin, Iterator end,
		  const Char* opener, const Char* seperator_1, const Char* seperator_2, const Char* closer)
{
	std::basic_ostringstream<Char, Traits> buffer;
	buffer.copyfmt(ostream);
	buffer.width(0);

	buffer << opener;
	for (Iterator i = begin; i != end; ++i)
	{
		if (i != begin)
		{
			buffer << seperator_1;
		}
		buffer << i->first << seperator_2 << i->second;
	}
	buffer << closer;

	LASS_ENFORCE_STREAM(ostream) << buffer.str();
	return ostream;
}


template
<
	typename ContainerTraits, typename DataTraits, typename T,
	typename Char, typename Traits,
	typename Container
>
std::basic_istream<Char, Traits>&
read_container(std::basic_istream<Char, Traits>& istream, Container& container,
			 Char opener, Char inter_seperator, Char intra_seperator, Char closer)
{
	Container result;

	bool good = true;
	Char c = 0;
	eat_whitespace(istream);
	if (istream.get(c))
	{
		if (c == opener)
		{
			T temp;
			good = DataTraits::read(istream, temp, inter_seperator, intra_seperator, closer);
			if (good)
			{
				ContainerTraits::push(result, temp);
			}
		}
		else
		{
			istream.putback(c);
			good = false;
		}
	}
	else
	{
		good = false;
	}

	while (good && istream.get(c) && c != closer)
	{
		if (c == inter_seperator)
		{
			T temp;
			good = DataTraits::read(istream, temp, inter_seperator, intra_seperator, closer);
			if (good)
			{
				ContainerTraits::push(result, temp);
			}
		}
		else
		{
			istream.putback(c);
			good = false;
		}
	}

	if (good)
	{
		ContainerTraits::temp_to_output(result, container);
	}
	else
	{
		istream.clear(std::ios::failbit);
	}

	return istream;
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
operator<<(std::basic_ostream<Char, Traits>& ostream,
		   const std::pair<T1, T2>& x)
{
	std::basic_ostringstream<Char, Traits> buffer;
	buffer.copyfmt(ostream);
	buffer.width(0);
	buffer << "(" << x.first << ", " << x.second << ")";
	LASS_ENFORCE_STREAM(ostream) << buffer.str();
	return ostream;
}

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& ostream,
		   const std::vector<T, Alloc>& container)
{
	return lass::stde::impl::print_sequence(
		ostream, container.begin(), container.end(), "[", ", ", "]");
}

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& ostream,
		   const std::list<T, Alloc>& container)
{
	return lass::stde::impl::print_sequence(
		ostream, container.begin(), container.end(), "[", ", ", "]");
}

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& ostream,
		   const std::deque<T, Alloc>& container)
{
	return lass::stde::impl::print_sequence(
		ostream, container.begin(), container.end(), "[", ", ", "]");
}

/** @ingroup extended_io
 */
template <typename Key, typename Data, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& ostream,
		   const std::map<Key, Data, Comp, Alloc>& container)
{
	return lass::stde::impl::print_map<Char>(
		ostream, container.begin(), container.end(), "{", ", ", ": ", "}");
}

/** @ingroup extended_io
 */
template <typename Key, typename Data, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& ostream,
		   const std::multimap<Key, Data, Comp, Alloc>& container)
{
	return lass::stde::impl::print_map<Char>(
		ostream, container.begin(), container.end(), "{", ", ", ": ", "}");
}

/** @ingroup extended_io
 */
template <typename Key, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& ostream,
		   const std::set<Key, Comp, Alloc>& container)
{
	return lass::stde::impl::print_sequence(
		ostream, container.begin(), container.end(), "{", ", ", "}");
}

/** @ingroup extended_io
 */
template <typename Key, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& ostream,
		   const std::multiset<Key, Comp, Alloc>& container)
{
	return lass::stde::impl::print_sequence(
		ostream, container.begin(), container.end(), "{", ", ", "}");
}

#ifdef _STLP_SLIST

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& ostream,
		   const std::slist<T, Alloc>& container)
{
	return lass::stde::impl::print_sequence(
		ostream, container.begin(), container.end(), "[", ", ", "]");
}

#endif



// input

/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename T1, typename T2>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& istream,
		   std::pair<T1, T2>& oPair)
{
	using namespace lass::stde;

	Char c = 0;
	if (istream.get(c))
	{
		if (c == '(')
		{
			std::pair<T1, T2> temp;
			if (impl::pair_traits::read<Char>(istream, temp, ')', ',', ')'))
			{
				oPair = temp;
				istream.ignore();
			}
		}
		else
		{
			istream.putback(c);
			istream.clear(std::ios::failbit);
		}
	}
	else
	{
		istream.clear(std::ios::failbit);
	}

	return istream;
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename T, typename Alloc>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& istream,
		   std::vector<T, Alloc>& container)
{
	using namespace lass::stde;
	return impl::read_container<impl::sequence_traits, impl::value_traits, T, Char>(
		istream, container, '[', ',', 0, ']');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename T, typename Alloc>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& istream,
		   std::list<T, Alloc>& container)
{
	using namespace lass::stde;
	return impl::read_container<impl::sequence_traits, impl::value_traits, T, Char>(
		istream, container, '[', ',', 0, ']');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename T, typename Alloc>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& istream,
		   std::deque<T, Alloc>& container)
{
	using namespace lass::stde;
	return impl::read_container<impl::sequence_traits, impl::value_traits, T, Char>(
		istream, container, '[', ',', 0, ']');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename Key, typename Data, typename Comp, typename Alloc>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& istream,
		   std::map<Key, Data, Comp, Alloc>& container)
{
	using namespace lass::stde;
	return impl::read_container<impl::set_traits, impl::pair_traits, std::pair<Key, Data>, Char>(
		istream, container, '{', ',', ':', '}');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename Key, typename Data, typename Comp, typename Alloc>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& istream,
		   std::multimap<Key, Data, Comp, Alloc>& container)
{
	using namespace lass::stde;
	return impl::read_container<impl::set_traits, impl::pair_traits, std::pair<Key, Data>, Char>(
		istream, container, '{', ',', ':', '}');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename Key, typename Comp, typename Alloc>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& istream,
		   std::set<Key, Comp, Alloc>& container)
{
	using namespace lass::stde;
	return impl::read_container<impl::set_traits, impl::value_traits, Key, Char>(
		istream, container, '{', ',', 0, '}');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename Key, typename Comp, typename Alloc>
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& istream,
		   std::multiset<Key, Comp, Alloc>& container)
{
	using namespace lass::stde;
	return impl::read_container<impl::set_traits, impl::value_traits, Key, Char>(
		istream, container, '{', ',', 0, '}');
}



}

#endif

// EOF
