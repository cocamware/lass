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



/** @defgroup extended_io ExtendedIo
 *  @brief extra insertors and extractors.
 *  @author Bram de Greve [BdG]
 *
 *  ExtendedIo groups additional stream operators for @c std::pair and standard containers like 
 *  @c std::vector, @c std::list, ...  By including this header, it will possible to output
 *  these types to a stream, just like a regular float.
 *
 *  Currently, the containers only have output operators, only std::pair has both the output
 *  and input operator.  This will probably change in the future when there's a need for it.
 *
 *  The supported standard containers are @c std::vector, @c std::list, @c std::deque,
 *  @c std::map, @c std::multimap, @c std::set, @c std::multiset.
 *
 *  STLport specific: in case @c std::slist is detected as included, it is supported as well.
 *
 *  @code
 *  std::pair<int, std::string> a(5, "hello");
 *	std::cout << a; // (5, hello)
 *  std::cin >> a;
 *
 *	std::vector<int> b;
 *	b.push_back(1);
 *	b.push_back(2);
 *	b.push_back(3);
 *	std::cout << b; // [1, 2, 3]
 *
 *	std::map<std::string, int> c;
 *	c["foo"] = 1;
 *	c["bar"] = 2;
 *	c["spam"] = 3;
 *	c << map; // {bar: 2, foo: 1, spam: 3}
 *
 *	std::set<std::string> d;
 *	d.insert("foo");
 *	d.insert("bar");
 *	d.insert("spam");
 *	stream << d; // {bar, foo, spam}
 *  @endcode
 *
 *  @note we need to inject all this stuff in the std namespace for the look up thingies to work.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_IO_H
#define LASS_GUARDIAN_OF_INCLUSION_STDE_EXTENDED_IO_H

#include "stde_common.h"

namespace std
{

// output

template <typename T1, typename T2, typename Char, typename Traits> 
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::pair<T1, T2>& iPair);

template <typename T, typename Alloc, typename Char, typename Traits> 
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::vector<T, Alloc>& iContainer);

template <typename T, typename Alloc, typename Char, typename Traits> 
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::list<T, Alloc>& iContainer);

template <typename T, typename Alloc, typename Char, typename Traits> 
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::deque<T, Alloc>& iContainer);

template <typename Key, typename Data, typename Comp, typename Alloc, typename Char, typename Traits> 
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::map<Key, Data, Comp, Alloc>& iContainer);

template <typename Key, typename Data, typename Comp, typename Alloc, typename Char, typename Traits> 
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::multimap<Key, Data, Comp, Alloc>& iContainer);

template <typename Key, typename Comp, typename Alloc, typename Char, typename Traits> 
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::set<Key, Comp, Alloc>& iContainer);

template <typename Key, typename Comp, typename Alloc, typename Char, typename Traits> 
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::multiset<Key, Comp, Alloc>& iContainer);

#ifdef _STLP_SLIST
template <typename T, typename Alloc, typename Char, typename Traits> 
std::basic_ostream<Char, Traits>& 
operator<<(std::basic_ostream<Char, Traits>& ioOStream, 
           const std::slist<T, Alloc>& iContainer);
#endif



// input
/*
template <typename T1, typename T2, typename Char, typename Traits> 
std::basic_istream<Char, Traits>& 
operator>>(std::basic_istream<Char, Traits>& ioIStream, std::pair<T1, T2>& oPair);

template <typename T, typename Alloc, typename Char, typename Traits> 
std::basic_istream<Char, Traits>& 
operator>>(std::basic_istream<Char, Traits>& ioIStream, std::vector<T, Alloc>& oContainer);

template <typename T, typename Alloc, typename Char, typename Traits> 
std::basic_istream<Char, Traits>& 
operator>>(std::basic_istream<Char, Traits>& ioIStream, std::list<T, Alloc>& oContainer);

template <typename T, typename Alloc, typename Char, typename Traits> 
std::basic_istream<Char, Traits>& 
operator>>(std::basic_istream<Char, Traits>& ioIStream, std::deque<T, Alloc>& oContainer);
*/
}

#include "extended_io.inl"

#endif

// EOF
