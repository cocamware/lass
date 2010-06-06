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
	parent_ << "<" << name_ << ">\n";
	clear(parent_.rdstate()); // copy state from parent_ to io::StreamBase
}



XmlOElement::~XmlOElement()
{
	parent_ << "</" << name_ << ">\n";
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
