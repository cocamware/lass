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

/** @class lass::io::BinaryIMemoryMap
 *  @brief Input Stream for files using memory mapping
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_I_MEMORY_MAP_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_I_MEMORY_MAP_H

#include "io_common.h"
#include "binary_i_stream.h"
#if LASS_HAVE_STD_FILESYSTEM
#	include <filesystem>
#endif

namespace lass
{
namespace io
{
namespace impl
{
	class BinaryIMemoryMapImpl;
}

class LASS_DLL BinaryIMemoryMap: public BinaryIStream
{
public:

	BinaryIMemoryMap();
	BinaryIMemoryMap(const char* filename);
	BinaryIMemoryMap(const std::string& filename);
#if LASS_HAVE_WCHAR_SUPPORT
	BinaryIMemoryMap(const wchar_t* filename);
	BinaryIMemoryMap(const std::wstring& filename);
#endif
#if LASS_HAVE_STD_FILESYSTEM
	BinaryIMemoryMap(const std::filesystem::path& filename);
#endif

	~BinaryIMemoryMap();

	void open(const char* filename);
	void open(const std::string& filename);
#if LASS_HAVE_WCHAR_SUPPORT
	void open(const wchar_t* filename);
	void open(const std::wstring& filename);
#endif
#if LASS_HAVE_STD_FILESYSTEM
	void open(const std::filesystem::path& filename);
#endif
	void close();
	bool is_open() const;

private:

	pos_type doTellg() const override;
	void doSeekg(pos_type position) override;
	void doSeekg(off_type offset, std::ios_base::seekdir direction) override;
	size_t doRead(void* output, size_t numberOfBytes) override;

	std::unique_ptr<impl::BinaryIMemoryMapImpl> pimpl_;
	size_t position_ { 0 };
};



}

}

#endif
