/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
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
 *	Copyright (C) 2004-2010 the Initial Developer.
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
/**
*   @author Bram de Greve (bram@cocamware.com)
*   @author Tom De Muer (tom@cocamware.com)
*
*   Distributed under the terms of the GPL (GNU Public License)
*
*   The LASS License:
*
*   Copyright 2004-2006 Bram de Greve and Tom De Muer
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



class MatlabOStream: public std::ofstream
{
public:
	MatlabOStream(): 
		std::ofstream(), 
		color_(mcBlack) 
	{
	}

	MatlabOStream(const char* iFilename, std::ios::openmode iOpenMode = std::ios::out | std::ios::trunc): 
		std::ofstream(iFilename, iOpenMode), 
		color_(mcBlack)
	{
	}

	virtual ~MatlabOStream() 
	{
	}

	void setColor(MatlabColor iColor) 
	{
		color_ = iColor; 
	}

	const std::string& color() const
	{
		static const std::string colors[] = {"'y'", "'m'", "'c'", "'r'", "'g'", "'b'", "'w'", "'k'"};
		return colors[color_];
	}

	void setFlag(const std::string& iName, bool iValue)
	{
		flags_[iName] = iValue;
	}

	bool flag(const std::string& iName, bool iDefault = false) const
	{
		TFlags::const_iterator i = flags_.find(iName);
		return i != flags_.end() ? i->second : iDefault;
	}

private:

	typedef std::map<std::string, bool> TFlags;

	TFlags flags_;
	MatlabColor color_;
};



}

}

#endif
