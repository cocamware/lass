/**
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*   Distributed under the terms of the GPL (GNU Public License)
*
*   The LASS License:
*
*   Copyright 2004 Bram de Greve and Tom De Muer
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



/** @class lass::io::MatlabOStream
 *  @brief Output stream for writing a selection of geometric primitives to matlab M files.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_MATLAB_O_STREAM_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_MATLAB_O_STREAM_H



#include "io_common.h"
#include <fstream>

namespace lass
{

namespace io
{

enum MatlabColor
{
	mcYellow,
	mcMagenta,
	mcCyan,
	mcRed,
	mcGreen,
	mcBlue,
	mcWhite,
	mcBlack
};



class LASS_DLL_EXPORT MatlabOStream :
	public std::ofstream
{
public:
	MatlabOStream();
	MatlabOStream(const char* iFilename, std::ios::openmode iOpenMode);
	virtual ~MatlabOStream();

	void setColor(MatlabColor iColor);
	const std::string& color() const;

	void setFlag(const std::string& iName, bool iValue);
	bool flag(const std::string& iName, bool iDefault = false) const;

private:

	typedef std::map<std::string, bool> TFlags;

	TFlags flags_;
	MatlabColor color_;
};



}

}

#endif
