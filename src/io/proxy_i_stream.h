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



/** @class lass::io::ProxyTextIStream
 *	@brief A proxy text input stream can get its input from another input stream.
 *	@author BdG
 *	@date 2003
 *
 *  @note a fresh constructed proxy stream gets input from std::cin.
 *
 *  OK, seriously, i have no longer any idea why we need this ... get rid of it?
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_PROXY_TEXT_I_STREAM_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_PROXY_TEXT_I_STREAM_H
#pragma once



#include "io_common.h"



namespace lass
{

namespace io
{

class LASS_DLL ProxyIStream
{
public:
	//ProxyIStream();
	ProxyIStream( std::istream* iSource = &std::cin );
	void setSource( std::istream* iSource );

	template <typename T> ProxyIStream& operator>> ( T& oOut )
	{
		LASS_ASSERT(source_);
		LASS_ENFORCE(*source_) >> oOut;
		return *this;
	}

private:

	std::istream* stdSource_;
};



}

}

//#include "proxy_i_stream.inl"

#endif
