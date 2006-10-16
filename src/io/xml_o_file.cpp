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

#include "io_common.h"
#include "xml_o_file.h"
#include "../util/bit_manip.h"

namespace lass
{
namespace io
{

// --- public --------------------------------------------------------------------------------------

XmlOFile::XmlOFile():
	XmlOStream(),
	file_(),
	root_("")
{
}



XmlOFile::XmlOFile(const char* iFileName, std::ios::openmode iOpenMode):
	XmlOStream()
{
	open(iFileName, iOpenMode);
}



XmlOFile::XmlOFile(const char* iFileName, const char* iRoot):
	XmlOStream()
{
	open(iFileName, iRoot);
}



XmlOFile::XmlOFile(const std::string& iFileName, std::ios::openmode iOpenMode):
	XmlOStream()
{
	open(iFileName, iOpenMode);
}



XmlOFile::XmlOFile(const std::string& iFileName, const std::string& iRoot):
	XmlOStream()
{
	open(iFileName, iRoot);
}



XmlOFile::~XmlOFile()
{
	close();
}



void XmlOFile::open(const char* iFilename, std::ios::openmode iOpenMode)
{
	file_.open(iFilename, iOpenMode);
	root_ = "";
	clear(file_.rdstate()); // copy state from std::ofstream to io::StreamBase
}



void XmlOFile::open(const char* iFilename, const char* iRoot)
{
	file_.open(iFilename, std::ios::out | std::ios::trunc);
	root_ = iRoot;
	file_ << "<?xml version=\"1.0\"?>\n";
	file_ << "<" << root_ << ">\n";
	clear(file_.rdstate()); // copy state from std::ofstream to io::StreamBase
}



void XmlOFile::open(const std::string& iFilename, std::ios::openmode iOpenMode)
{
	open(iFilename.c_str(), iOpenMode);
}



void XmlOFile::open(const std::string& iFilename, const std::string& iRoot)
{
	open(iFilename.c_str(), iRoot.c_str());
}



void XmlOFile::close()
{
	if (file_.is_open() && !root_.empty())
	{
		file_ << "</" << root_ << ">\n";
	}
	file_.close();
	clear(file_.rdstate()); // copy state from std::ofstream to io::StreamBase
}



bool XmlOFile::is_open()
{
	return file_.is_open();
}



#define LASS_IO_XML_O_FILE_INSERTOR( type__ )\
	XmlOFile& XmlOFile::operator<<( type__ iIn )\
	{\
		file_ << iIn;\
		clear(file_.rdstate());\
		return *this;\
	}

LASS_IO_XML_O_FILE_INSERTOR( char )
LASS_IO_XML_O_FILE_INSERTOR( signed char )
LASS_IO_XML_O_FILE_INSERTOR( unsigned char )
LASS_IO_XML_O_FILE_INSERTOR( signed short  )
LASS_IO_XML_O_FILE_INSERTOR( unsigned short  )
LASS_IO_XML_O_FILE_INSERTOR( signed int  )
LASS_IO_XML_O_FILE_INSERTOR( unsigned int  )
LASS_IO_XML_O_FILE_INSERTOR( signed long  )
LASS_IO_XML_O_FILE_INSERTOR( unsigned long  )
LASS_IO_XML_O_FILE_INSERTOR( float  )
LASS_IO_XML_O_FILE_INSERTOR( double  )
LASS_IO_XML_O_FILE_INSERTOR( long double  )
LASS_IO_XML_O_FILE_INSERTOR( bool  )
LASS_IO_XML_O_FILE_INSERTOR( const void*  )
LASS_IO_XML_O_FILE_INSERTOR( const char*  )
LASS_IO_XML_O_FILE_INSERTOR( const std::string& )

XmlOFile& XmlOFile::operator<<( std::ostream& (*iIn) (std::ostream&) )
{
	file_ << iIn;
	clear(file_.rdstate()); // copy state from std::ofstream to io::StreamBase
	return *this;
}




// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------



}

}

// EOF
