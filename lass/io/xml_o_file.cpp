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

#include "lass_common.h"
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
