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



/** @class lass::io::StreamBase
 *  @brief Base class for LASS streams with some state householding.
 *
 *  This class is only to provide functionality to whatever stream derives from it.
 *  You're not supposed to actually "use" it.  "use" it as in having StreamBase pointers
 *  or references floating in your code.  For that matter, the destructor of StreamBase
 *  is protected so life gets a bit troubled if you decide to try it anyway.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_STREAM_BASE_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_STREAM_BASE_H



#include "io_common.h"
#include "../num/safe_bool.h"
#include "../util/non_copyable.h"
#include <cstdio>

namespace lass
{

namespace io
{

class LASS_DLL_EXPORT StreamBase: public util::NonCopyable
{
public:

	StreamBase(): state_(std::ios_base::goodbit) {}

	const bool good() const { return state_ == std::ios_base::goodbit; }
	const bool eof() const { return (state_ & std::ios_base::eofbit) != 0; }
	const bool fail() const { return (state_ & (std::ios_base::failbit | std::ios_base::badbit)) != 0; }
	const bool bad() const { return (state_ & std::ios_base::badbit) != 0; }
	std::ios_base::iostate rdstate() const { return state_; }
	void clear(std::ios_base::iostate iState = std::ios_base::goodbit) { state_ = iState; }
	void setstate(std::ios_base::iostate iState) { state_ |= iState; }
	operator num::SafeBool() const { return fail() ? num::safeFalse : num::safeTrue; }
	const bool operator!() const { return fail(); }

protected:

	~StreamBase() {}

private:

	std::ios::iostate state_;
};

}

}

#endif
