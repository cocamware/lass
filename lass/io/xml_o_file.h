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
 *	Copyright (C) 2004-2011 the Initial Developer.
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



/** @class lass::io::XmlOFile
 *  @brief Output stream for writing a selection of geometric primitives to XML files.
 *
 *  if you open a XmlOFile with @a iOpenMode (or without a second argument), the file is opened
 *  and that's it.  Not automatic content is written to it:
 *
 *  @code
 *  {
 *      XmlOFile a("foo.xml");
 *      a << "spam\n"
 *  }
 *
 *  // foo.xml:
 *  // spam
 *
 *  {
 *      XmlOFile b("bar.xml, std::ios::trunc);
 *      b << "eggs\n"
 *  }
 *
 *  // bar.xml:
 *  // eggs
 *  @endcode
 *
 *  However, if your second argument is the name of your root element, then a standard XML
 *  declaration will be written to it and the root element will be opened.  When the file is
 *  closed, this root element will be closed automatically as well.
 *
 *  @code
 *  {
 *      XmlOFile c("fun.xml", "root");
 *      c << "ham\n"
 *  }
 *
 *  // fun.xml:
 *  // <?xml version="1.0"?>
 *  // <root>
 *  // ham
 *  // </root>
 *  @endcode
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
	XmlOFile(const char* iFilename, std::ios::openmode iOpenMode = std::ios::out | std::ios::trunc);
	XmlOFile(const char* iFilename, const char* iRoot);
	XmlOFile(const std::string& iFilename, std::ios::openmode iOpenMode = std::ios::out | std::ios::trunc);
	XmlOFile(const std::string& iFilename, const std::string& iRoot);
	virtual ~XmlOFile();

	void open(const char* iFilename, std::ios::openmode iOpenMode = std::ios::out | std::ios::trunc);
	void open(const char* iFilename, const char* iRoot);
	void open(const std::string& iFilename, std::ios::openmode iOpenMode = std::ios::out | std::ios::trunc);
	void open(const std::string& iFilename, const std::string& iRoot);
	void close();
	bool is_open();

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
	std::string root_;
};



}

}

#endif
