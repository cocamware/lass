/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
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



/** @class lass::io::XmlOFile
 *  @brief Output stream for writing a selection of geometric primitives to XML files.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_XML_O_FILE_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_XML_O_FILE_H



#include "io_common.h"
#include "xml_o_stream.h"

namespace lass
{

namespace io
{

	
class LASS_DLL XmlOFile: public XmlOStream
{
public:

	XmlOFile();
	XmlOFile(const char* iFilename, 
			 std::ios_base::open_mode iOpenMode = std::ios_base::out | std::ios_base::trunc);
	virtual ~XmlOFile();

	void open(const char* iFilename,
			  std::ios_base::open_mode iOpenMode = std::ios_base::out | std::ios_base::trunc);
	void close();
	bool is_open() const;


	virtual bool operator!() const;
	virtual std::ios::iostate rdstate() const;
	virtual bool good() const;
	virtual bool eof() const;
	virtual bool fail() const;
	virtual bool bad() const;

    virtual XmlOFile& operator<<( char iIn );
    virtual XmlOFile& operator<<( signed char iIn );
    virtual XmlOFile& operator<<( unsigned char iIn );
    virtual XmlOFile& operator<<( signed short iIn );
    virtual XmlOFile& operator<<( unsigned short iIn );
    virtual XmlOFile& operator<<( signed int iIn ) ;
    virtual XmlOFile& operator<<( unsigned int iIn );
    virtual XmlOFile& operator<<( signed long iIn );
    virtual XmlOFile& operator<<( unsigned long iIn );
    virtual XmlOFile& operator<<( float iIn );
    virtual XmlOFile& operator<<( double iIn );
    virtual XmlOFile& operator<<( long double iIn );
    virtual XmlOFile& operator<<( bool iIn );
    virtual XmlOFile& operator<<( const void* iIn );
    virtual XmlOFile& operator<<( const char* iIn );
	virtual XmlOFile& operator<<( const std::string& iIn );

	virtual XmlOFile& operator<<( std::ostream& (*iIn) (std::ostream&) );

private:

	std::ofstream file_;
};



}

}

#endif