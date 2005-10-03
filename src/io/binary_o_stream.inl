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

#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_O_STREAM_INL
#define LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_O_STREAM_INL




#include "binary_o_stream.h"



namespace lass
{

namespace io
{

template <typename T>
BinaryOStream& BinaryOStream::operator<<(const std::vector<T>& iIn)
{
	typedef typename std::vector<T>::size_type size_type;
	const size_type size = iIn.size();
	const num::Tuint32 n = static_cast<unsigned>(size);
	LASS_ASSERT(size == static_cast<size_t>(n));
	*this << n;

	for (size_type i = 0; i < size; ++i)
	{
		LASS_ASSERT(i < iIn.size());
		*this << iIn[i];
	}
	return *this;
}



template <typename T>
BinaryOStream& BinaryOStream::operator<<(const std::complex<T>& iIn)
{
	*this << iIn.real() << iIn.imag();
	return *this;
}





}

}


#endif

// EOF
