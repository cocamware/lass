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

#include "pylass_common.h"
#include "image.h"

namespace pylass
{
namespace image
{
inline const prim::ColorRGBA& getPixel(const io::Image& iImage, unsigned iRow, unsigned iCol)
{
	return iImage(iRow, iCol);
}
inline void setPixel(io::Image& ioImage, unsigned iRow, unsigned iCol, const prim::ColorRGBA& iPixel)
{
	ioImage(iRow, iCol) = iPixel;
}
}

PY_DECLARE_CLASS(Image)
PY_CLASS_CONSTRUCTOR_0(Image)
PY_CLASS_CONSTRUCTOR_2(Image, unsigned, unsigned)
PY_CLASS_CONSTRUCTOR_1(Image, const std::string&)
PY_CLASS_METHOD_QUALIFIED_0(Image, reset, void)
PY_CLASS_METHOD_QUALIFIED_2(Image, reset, void, size_t, size_t)
PY_CLASS_METHOD_QUALIFIED_1(Image, reset, void, const std::string&)
PY_CLASS_METHOD_QUALIFIED_1(Image, open, void, const std::string&)
PY_CLASS_METHOD_QUALIFIED_1(Image, save, void, const std::string&)
PY_CLASS_FREE_METHOD_NAME_DOC(Image, image::getPixel, "get", "get(row, col)")
PY_CLASS_FREE_METHOD_NAME_DOC(Image, image::setPixel, "set", "set(row, col, color)")
PY_CLASS_MEMBER_R(Image, cols)
PY_CLASS_MEMBER_R(Image, rows)
PY_CLASS_MEMBER_R(Image, isEmpty)
PY_CLASS_METHOD(Image, over)
PY_CLASS_METHOD(Image, in)
PY_CLASS_METHOD(Image, out)
PY_CLASS_METHOD(Image, atop)
PY_CLASS_METHOD(Image, through)
PY_CLASS_METHOD(Image, rover)
PY_CLASS_METHOD(Image, rin)
PY_CLASS_METHOD(Image, rout)
PY_CLASS_METHOD(Image, ratop)
PY_CLASS_METHOD(Image, rthrough)
PY_CLASS_METHOD(Image, plus)
PY_CLASS_METHOD(Image, filterMedian)
PY_CLASS_METHOD(Image, filterGamma)
PY_CLASS_METHOD(Image, filterExposure)
PY_CLASS_METHOD(Image, filterInverseExposure)

}
