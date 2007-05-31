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

/** @class lass::io::BinaryIMemoryMap
 *  @brief Input Stream for files using memory mapping
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_I_MEMORY_MAP_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_BINARY_I_MEMORY_MAP_H

#include "io_common.h"
#include "binary_i_stream.h"

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
	~BinaryIMemoryMap();

	void open(const char* filename);
	void open(const std::string& filename);
	void close();
	bool is_open() const;

private:

	long doTellg() const;
	void doSeekg(long offset, std::ios_base::seekdir direction);
	void doRead(void* output, size_t numberOfBytes);

	impl::BinaryIMemoryMapImpl* pimpl_;
	char* data_;
	long begin_;
	long end_;
	long size_;
	long position_;
};



}

}

#endif
