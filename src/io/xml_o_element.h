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



/** @class lass::io::XmlOElement
 *  @brief Starts a element in a XmlStream you can write to, and that will be closed automatically.
 *
 *  @code
 *	XmlOFile file("foo.xml", "root");
 *	{
 *		XmlOElement element(file, "element");
 *		element << "inside element";
 *	}
 *	file << "outside element";
 *  @endcode
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_XML_O_ELEMENT_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_XML_O_ELEMENT_H



#include "io_common.h"
#include "xml_o_stream.h"

namespace lass
{

namespace io
{

	
class LASS_DLL_EXPORT XmlOElement: public XmlOStream
{
public:

	XmlOElement(XmlOStream& iParent, const std::string& iName); 
	virtual ~XmlOElement();

	virtual bool operator!() const;
	virtual std::ios::iostate rdstate() const;
	virtual bool good() const;
	virtual bool eof() const;
	virtual bool fail() const;
	virtual bool bad() const;

    virtual XmlOElement& operator<<( char iIn );
    virtual XmlOElement& operator<<( signed char iIn );
    virtual XmlOElement& operator<<( unsigned char iIn );
    virtual XmlOElement& operator<<( signed short iIn );
    virtual XmlOElement& operator<<( unsigned short iIn );
    virtual XmlOElement& operator<<( signed int iIn ) ;
    virtual XmlOElement& operator<<( unsigned int iIn );
    virtual XmlOElement& operator<<( signed long iIn );
    virtual XmlOElement& operator<<( unsigned long iIn );
    virtual XmlOElement& operator<<( float iIn );
    virtual XmlOElement& operator<<( double iIn );
    virtual XmlOElement& operator<<( long double iIn );
    virtual XmlOElement& operator<<( bool iIn );
    virtual XmlOElement& operator<<( const void* iIn );
    virtual XmlOElement& operator<<( const char* iIn );
	virtual XmlOElement& operator<<( const std::string& iIn );

	virtual XmlOElement& operator<<( std::ostream& (*iIn) (std::ostream&) );

private:

	XmlOStream& parent_;
	std::string name_;
};



}

}

#endif
