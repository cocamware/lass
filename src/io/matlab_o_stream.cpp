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

#include "io_common.h"
#include "matlab_o_stream.h"

namespace lass
{
namespace io
{

// --- public --------------------------------------------------------------------------------------

MatlabOStream::MatlabOStream():
	std::ofstream(),
	color_(mcBlack)
{
}



MatlabOStream::MatlabOStream(const char* iFileName, std::ios_base::open_mode iOpenMode):
	std::ofstream(iFileName, iOpenMode),
	color_(mcBlack)
{
}



MatlabOStream::~MatlabOStream()
{
}



void MatlabOStream::setColor(MatlabColor iColor)
{
	color_ = iColor;
}



const std::string& MatlabOStream::color() const
{
	static const std::string colors[] = 
	{
		"'y'", 
		"'m'", 
		"'c'",
		"'r'",
		"'g'",
		"'b'",
		"'w'",
		"'k'"
	};

	return colors[color_];
}



void MatlabOStream::setFlag(const std::string& iName, bool iValue)
{
	flags_[iName] = iValue;
}



bool MatlabOStream::flag(const std::string& iName, bool iDefault) const
{
	TFlags::const_iterator i = flags_.find(iName);
	return i != flags_.end() ? i->second : iDefault;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------



}

}

// EOF
