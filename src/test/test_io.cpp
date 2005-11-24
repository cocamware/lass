/** @file
 *  @internal
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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



#include "test_common.h"
#include "test_io.h"

#include "test_io_avi.inl"
#include "test_io_arg_parser.inl"
#include "test_io_binary_stream.inl"
#include "test_io_file_attributes.inl"
#include "test_io_proxy_system.inl"
#include "../io/image.h"

namespace lass
{
namespace test
{

void HdrTest()
{
	io::Image image("d:/temp/grace_probe.hdr");
	image.save("d:/temp/grace_probe_1.hdr");
	image.filterExposure(4.f);
	image.save("d:/temp/grace_probe_2.tga");
	
	io::Image image2("d:/temp/grace_probe_2.tga");
	image2.filterInverseExposure(4.f);
	image2.save("d:/temp/grace_probe_3.hdr");
}

TUnitTests testIo()
{
	TUnitTests result;

	//result.push_back(LASS_UNIT_TEST(testIoAvi));
	result.push_back(LASS_UNIT_TEST(testIoArgParser));
	result.push_back(LASS_UNIT_TEST(testIoBinaryStream));
	result.push_back(LASS_UNIT_TEST(testIoFileAttributes));
	result.push_back(LASS_UNIT_TEST(testIoProxySystem));
	result.push_back(LASS_UNIT_TEST(HdrTest));

	return result;
}


}

}
