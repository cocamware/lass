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


 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_I_STREAM_INL
#define LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_I_STREAM_INL
#pragma once



#include "binary_i_stream.h"



namespace lass
{

namespace io
{

template <typename T>
BinaryIStream& BinaryIStream::operator>>(std::vector<T>& oOut)
{
	unsigned size = 0;
	*this >> size;

	std::vector<T> result;
	result.resize(size);

	for (unsigned i = 0; i < size; ++i)
	{
		T t;
		*this >> t;
		result.push_back(t);
	}

	oOut.swap(result);	// "copy" result to output
	return *this;
}



template <typename T>
BinaryIStream& BinaryIStream::operator>>(std::complex<T>& oOut)
{
	T re;
	*this >> re;
	T im;
	*this >> im;

	oOut = std::complex<T>(re, im);
	return *this;
}

	



}

}


#endif
