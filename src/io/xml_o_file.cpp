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
#include "xml_o_file.h"
#include "../util/bit_manip.h"

namespace lass
{
namespace io
{

// --- public --------------------------------------------------------------------------------------

XmlOFile::XmlOFile():
	XmlOStream(),
	file_()
{
}



XmlOFile::XmlOFile(const char* iFileName, std::ios_base::open_mode iOpenMode):
	XmlOStream()
{
	open(iFileName, iOpenMode);
}



XmlOFile::~XmlOFile()
{
}



void XmlOFile::open(const char* iFilename, std::ios_base::open_mode iOpenMode)
{
	file_.open(iFilename, iOpenMode);
	if (util::checkMaskedAll<std::ios_base::open_mode>(iOpenMode, std::ios_base::trunc))
	{
		file_ << "<?xml version=\"1.0\"?>" << std::endl;
	}
}



#define LASS_IO_XML_FILE_METHOD( method__ )\
	void XmlOFile::method__()\
	{\
		file_.method__();\
	}

LASS_IO_XML_FILE_METHOD( close )



#define LASS_IO_XML_FILE_GETTER( type__, getter__ )\
	type__ XmlOFile::getter__() const\
	{\
		return file_.getter__();\
	}

LASS_IO_XML_FILE_GETTER( bool, is_open )
LASS_IO_XML_FILE_GETTER( bool, operator! )
LASS_IO_XML_FILE_GETTER( std::ios::iostate, rdstate )
LASS_IO_XML_FILE_GETTER( bool, good )
LASS_IO_XML_FILE_GETTER( bool, eof )
LASS_IO_XML_FILE_GETTER( bool, fail )
LASS_IO_XML_FILE_GETTER( bool, bad )



#define LASS_IO_XML_O_FILE_INSERTOR( type__ )\
	XmlOFile& XmlOFile::operator<<( type__ iIn )\
	{\
		file_ << iIn;\
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
	return *this;
}




// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------



}

}

// EOF
