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



/** @class lass::io::BinaryStreamBase
 *  @brief Base class for LASS binary streams with byte-order househoulding
 *
 *  This class is only to provide functionality to whatever stream derives from it.
 *  You're not supposed to actually "use" it.  "use" it as in having BinaryStream pointers
 *  or references floating in your code.  For that matter, the destructor of BinaryStream
 *  is protected so life gets a bit troubled if you decide to try it anyway.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_STREAM_BASE_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_STREAM_BASE_H



#include "io_common.h"
#include "stream_base.h"
#include "../num/endianness.h"
#include "../num/basic_types.h"

namespace lass
{

namespace io
{

class LASS_DLL BinaryStreamBase: public StreamBase
{
public:
	using pos_type = size_t;
#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC && LASS_ADDRESS_SIZE == 64
	using off_type = __int64;
#else
	using off_type = long;
#endif

	static_assert(num::NumTraits<off_type>::max < num::NumTraits<pos_type>::max, "max off_type value must fit in pos_type");

	num::Endianness endianness() const { return streamEndianness_; }
	void setEndianness(num::Endianness iEndianness) { streamEndianness_ = iEndianness; }

protected:

	BinaryStreamBase(num::Endianness iEndianness = num::littleEndian);
	virtual ~BinaryStreamBase();

private:

	num::Endianness streamEndianness_;	
};




class LASS_DLL EndiannessSetter: public util::NonCopyable
{
public:
	EndiannessSetter(BinaryStreamBase& iStream, num::Endianness iEndianness);
	~EndiannessSetter();
private:
	BinaryStreamBase& stream_;
	num::Endianness oldEndianness_;
};

}

}

#endif
