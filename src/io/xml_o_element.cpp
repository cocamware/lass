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
#include "xml_o_element.h"

namespace lass
{
namespace io
{

// --- public --------------------------------------------------------------------------------------

XmlOElement::XmlOElement(XmlOStream& iParent, const std::string& iName):
	XmlOStream(),
	parent_(iParent),
	name_(iName)
{
	parent_ << "<" << name_ << ">" << std::endl;
	clear(parent_.rdstate()); // copy state from parent_ to io::StreamBase
}



XmlOElement::~XmlOElement()
{
	parent_ << "</" << name_ << ">" << std::endl;
}



#define LASS_IO_XML_O_ELEMENT_INSERTOR( type__ )\
	XmlOElement& XmlOElement::operator<<( type__ iIn )\
	{\
		parent_ << iIn;\
		clear(parent_.rdstate());\
		return *this;\
	}

LASS_IO_XML_O_ELEMENT_INSERTOR( char )
LASS_IO_XML_O_ELEMENT_INSERTOR( signed char )
LASS_IO_XML_O_ELEMENT_INSERTOR( unsigned char )
LASS_IO_XML_O_ELEMENT_INSERTOR( signed short  )
LASS_IO_XML_O_ELEMENT_INSERTOR( unsigned short  )
LASS_IO_XML_O_ELEMENT_INSERTOR( signed int  )
LASS_IO_XML_O_ELEMENT_INSERTOR( unsigned int  )
LASS_IO_XML_O_ELEMENT_INSERTOR( signed long  )
LASS_IO_XML_O_ELEMENT_INSERTOR( unsigned long  )
LASS_IO_XML_O_ELEMENT_INSERTOR( float  )
LASS_IO_XML_O_ELEMENT_INSERTOR( double  )
LASS_IO_XML_O_ELEMENT_INSERTOR( long double  )
LASS_IO_XML_O_ELEMENT_INSERTOR( bool  )
LASS_IO_XML_O_ELEMENT_INSERTOR( const void*  )
LASS_IO_XML_O_ELEMENT_INSERTOR( const char*  )
LASS_IO_XML_O_ELEMENT_INSERTOR( const std::string& )

XmlOElement& XmlOElement::operator<<( std::ostream& (*iIn) (std::ostream&) )
{
	parent_ << iIn;
	clear(parent_.rdstate()); // copy state from parent_ to io::StreamBase
	return *this;
}




// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------



}

}

// EOF
