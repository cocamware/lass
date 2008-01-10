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

#include <cctype>
#include "extended_string.h"

// --- implemenation details -----------------------------------------------------------------------

namespace lass
{
namespace stde
{
namespace impl
{

/** @ingroup extended_io
 *  @internal
 */
template <typename Char, typename Traits>
void eat_whitespace(std::basic_istream<Char, Traits>& stream)
{
	Char c = 0;
	while (stream.get(c))
	{
		if (!std::isspace(c))
		{
			stream.putback(c);
			break;
		}
	}
}

/** @ingroup extended_io
 *  @internal
 */
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

/** @ingroup extended_io
 *  @internal
 */
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

/** @ingroup extended_io
 *  @internal
 *  @note may change value on failure, but that's ok since it's a temp of the caller.
 */
struct value_traits
{
	template <typename Char, typename Traits, typename T>
	static bool read(
			std::basic_istream<Char, Traits>& stream, T& value,
			Char inter_seperator, Char /*intra_seperator*/, Char closer)
	{
		eat_whitespace(stream);

		// read till next seperator
		//
		std::basic_string<Char, Traits> buffer;
		Char c = 0;
		while (stream.get(c))
		{
			if (c == inter_seperator || c == closer)
			{
				stream.putback(c);
				break;
			}
			buffer += c;
		}

		// convert
		//
		return cast(buffer, value);

	}
private:
	static bool cast(const std::string& buffer, std::string& value)
	{
		value = rstrip(buffer);
		return true;
	}
	template <typename T>
	static bool cast(const std::string& buffer, T& value)
	{
		std::stringstream buffer_stream(buffer);
		if (std::numeric_limits<T>::is_specialized)
		{
			buffer_stream.precision(std::numeric_limits<T>::digits10 + 1);
		}
		return buffer_stream >> value && (buffer_stream >> std::ws).eof();
	}
};

/** @ingroup extended_io
 *  @internal
 *  @note may change value on failure, but that's ok since it's a temp of the caller.
 */
struct pair_traits
{
	template <typename Char, typename Traits, typename T, typename U>
	static bool read(
			std::basic_istream<Char, Traits>& stream, std::pair<T, U>& value,
			Char inter_seperator, Char intra_seperator, Char closer)
	{
		if (!value_traits::read<Char>(stream, value.first, intra_seperator, 0, intra_seperator))
		{
			return false;
		}
		stream.ignore();
		if (!value_traits::read<Char>(stream, value.second, inter_seperator, 0, closer))
		{
			return false;
		}
		return true;
	}
};

/** @ingroup extended_io
 *  @internal
 */
template <typename Iterator, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& print_sequence(
		std::basic_ostream<Char, Traits>& stream, Iterator begin, Iterator end,
		const Char* opener, const Char* seperator, const Char* closer)
{
	std::basic_ostringstream<Char, Traits> buffer;
	buffer.copyfmt(stream);
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

	LASS_ENFORCE_STREAM(stream) << buffer.str();
	return stream;
}

/** @ingroup extended_io
 *  @internal
 */
template <typename Char, typename Traits, typename Iterator>
std::basic_ostream<Char, Traits>& print_map(
		std::basic_ostream<Char, Traits>& stream, Iterator begin, Iterator end,
		const Char* opener, const Char* seperator_1, const Char* seperator_2, const Char* closer)
{
	std::basic_ostringstream<Char, Traits> buffer;
	buffer.copyfmt(stream);
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

	LASS_ENFORCE_STREAM(stream) << buffer.str();
	return stream;
}

/** @ingroup extended_io
 *  @internal
 */
template
<
	typename ContainerTraits, typename DataTraits, typename T,
	typename Char, typename Traits,
	typename Container
>
std::basic_istream<Char, Traits>& read_container(
		std::basic_istream<Char, Traits>& stream, Container& container,
		Char opener, Char inter_seperator, Char intra_seperator, Char closer)
{
	Container result;

	bool good = true;
	Char c = 0;
	eat_whitespace(stream);
	if (stream.get(c))
	{
		if (c == opener)
		{
			T temp;
			good = DataTraits::read(stream, temp, inter_seperator, intra_seperator, closer);
			if (good)
			{
				ContainerTraits::push(result, temp);
			}
		}
		else
		{
			stream.putback(c);
			good = false;
		}
	}
	else
	{
		good = false;
	}

	while (good && stream.get(c) && c != closer)
	{
		if (c == inter_seperator)
		{
			T temp;
			good = DataTraits::read(stream, temp, inter_seperator, intra_seperator, closer);
			if (good)
			{
				ContainerTraits::push(result, temp);
			}
		}
		else
		{
			stream.putback(c);
			good = false;
		}
	}

	if (good)
	{
		ContainerTraits::temp_to_output(result, container);
	}
	else
	{
		stream.clear(std::ios::failbit);
	}

	return stream;
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
std::basic_ostream<Char, Traits>& operator<<(
		std::basic_ostream<Char, Traits>& stream, const std::pair<T1, T2>& x)
{
	std::basic_ostringstream<Char, Traits> buffer;
	buffer.copyfmt(stream);
	buffer.width(0);
	buffer << "(" << x.first << ", " << x.second << ")";
	LASS_ENFORCE_STREAM(stream) << buffer.str();
	return stream;
}

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
		std::basic_ostream<Char, Traits>& stream, const std::vector<T, Alloc>& container)
{
	return lass::stde::impl::print_sequence(
		stream, container.begin(), container.end(), "[", ", ", "]");
}

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
		std::basic_ostream<Char, Traits>& stream, const std::list<T, Alloc>& container)
{
	return lass::stde::impl::print_sequence(
		stream, container.begin(), container.end(), "[", ", ", "]");
}

/** @ingroup extended_io
 */
template <typename T, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
		std::basic_ostream<Char, Traits>& stream, const std::deque<T, Alloc>& container)
{
	return lass::stde::impl::print_sequence(
		stream, container.begin(), container.end(), "[", ", ", "]");
}

/** @ingroup extended_io
 */
template <typename Key, typename Data, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
		std::basic_ostream<Char, Traits>& stream,
		const std::map<Key, Data, Comp, Alloc>& container)
{
	return lass::stde::impl::print_map<Char>(
		stream, container.begin(), container.end(), "{", ", ", ": ", "}");
}

/** @ingroup extended_io
 */
template <typename Key, typename Data, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
		std::basic_ostream<Char, Traits>& stream,
		const std::multimap<Key, Data, Comp, Alloc>& container)
{
	return lass::stde::impl::print_map<Char>(
		stream, container.begin(), container.end(), "{", ", ", ": ", "}");
}

/** @ingroup extended_io
 */
template <typename Key, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
		std::basic_ostream<Char, Traits>& stream, 
		const std::set<Key, Comp, Alloc>& container)
{
	return lass::stde::impl::print_sequence(
		stream, container.begin(), container.end(), "{", ", ", "}");
}

/** @ingroup extended_io
 */
template <typename Key, typename Comp, typename Alloc, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
		std::basic_ostream<Char, Traits>& stream,
		const std::multiset<Key, Comp, Alloc>& container)
{
	return lass::stde::impl::print_sequence(
		stream, container.begin(), container.end(), "{", ", ", "}");
}



// input

/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename T1, typename T2>
std::basic_istream<Char, Traits>& operator>>(
		std::basic_istream<Char, Traits>& stream, std::pair<T1, T2>& x)
{
	using namespace lass::stde;

	Char c = 0;
	if (stream.get(c))
	{
		if (c == '(')
		{
			std::pair<T1, T2> temp;
			if (impl::pair_traits::read<Char>(stream, temp, ')', ',', ')'))
			{
				x = temp;
				stream.ignore();
			}
		}
		else
		{
			stream.putback(c);
			stream.clear(std::ios::failbit);
		}
	}
	else
	{
		stream.clear(std::ios::failbit);
	}

	return stream;
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename T, typename Alloc>
std::basic_istream<Char, Traits>& operator>>(
		std::basic_istream<Char, Traits>& stream, std::vector<T, Alloc>& container)
{
	return ::lass::stde::impl::read_container<
		::lass::stde::impl::sequence_traits, 
		::lass::stde::impl::value_traits, 
		T, Char>(
			stream, container, '[', ',', 0, ']');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename T, typename Alloc>
std::basic_istream<Char, Traits>& operator>>(
		std::basic_istream<Char, Traits>& stream, std::list<T, Alloc>& container)
{
	return ::lass::stde::impl::read_container<
		::lass::stde::impl::sequence_traits, 
		::lass::stde::impl::value_traits,
		T, Char>(
			stream, container, '[', ',', 0, ']');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename T, typename Alloc>
std::basic_istream<Char, Traits>& operator>>(
		std::basic_istream<Char, Traits>& stream, std::deque<T, Alloc>& container)
{
	return ::lass::stde::impl::read_container<
		::lass::stde::impl::sequence_traits, 
		::lass::stde::impl::value_traits, 
		T, Char>(
			stream, container, '[', ',', 0, ']');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename Key, typename Data, typename Comp, typename Alloc>
std::basic_istream<Char, Traits>& operator>>(
		std::basic_istream<Char, Traits>& stream, std::map<Key, Data, Comp, Alloc>& container)
{
	return ::lass::stde::impl::read_container<
		::lass::stde::impl::set_traits, 
		::lass::stde::impl::pair_traits, 
		std::pair<Key, Data>, Char>(
			stream, container, '{', ',', ':', '}');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename Key, typename Data, typename Comp, typename Alloc>
std::basic_istream<Char, Traits>& operator>>(
		std::basic_istream<Char, Traits>& stream,
		std::multimap<Key, Data, Comp, Alloc>& container)
{
	return ::lass::stde::impl::read_container<
		::lass::stde::impl::set_traits, 
		::lass::stde::impl::pair_traits, 
		std::pair<Key, Data>, Char>(
			stream, container, '{', ',', ':', '}');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename Key, typename Comp, typename Alloc>
std::basic_istream<Char, Traits>& operator>>(
		std::basic_istream<Char, Traits>& stream, std::set<Key, Comp, Alloc>& container)
{
	return ::lass::stde::impl::read_container<
		::lass::stde::impl::set_traits, 
		::lass::stde::impl::value_traits, 
		Key, Char>(
			stream, container, '{', ',', 0, '}');
}



/** @ingroup extended_io
 */
template <typename Char, typename Traits, typename Key, typename Comp, typename Alloc>
std::basic_istream<Char, Traits>& operator>>(
		std::basic_istream<Char, Traits>& stream, std::multiset<Key, Comp, Alloc>& container)
{
	return ::lass::stde::impl::read_container<
		::lass::stde::impl::set_traits, 
		::lass::stde::impl::value_traits, 
		Key, Char>(
			stream, container, '{', ',', 0, '}');
}



}

// EOF
