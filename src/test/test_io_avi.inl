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

#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_IO_AVI_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_IO_AVI_INL

#include "test_common.h"

#include "../io/avi.h"

#if !defined(LASS_IO_AVI_NO_SUPPORT)

#include "../prim/color_rgba.h"
#include "../num/random.h"
#include "../num/distribution.h"

namespace lass
{
namespace test
{

void testIoAvi()
{
	const int width = 320;
	const int height = 240;

	num::RandomMT19937 random;
	num::DistributionUniform<float, num::RandomMT19937> uniform01(random);

	LASS_COUT << "SPECIFY AVI FORMAT IN DIALOG!\n";
	io::Avi avi("test.avi", 25, width, height, 24);
	std::vector<prim::ColorRGBA> buffer(width * height);

	LASS_COUT << "\n writing avi file: ";
	for (int i = 0; i < 25; ++i)
	{
		LASS_COUT << ".";
		for (int y = 0; y < height; ++y)
		{
			const int x0 = y * width;
			for (int x = 0; x < width; ++x)
			{
				const float r = uniform01();
				const float g = uniform01();
				const float b = 0;//uniform01();
				buffer[x0 + x] = prim::ColorRGBA(r, g, b);
			}
		}
		avi.frame(&buffer[0]);
	}
	LASS_COUT << "\n";
}



}

}

#endif

#endif

// EOF
