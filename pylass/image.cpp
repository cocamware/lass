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
 *	Copyright (C) 2004-2025 the Initial Developer.
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
PY_CLASS_METHOD_NAME(Image, in, "in_")
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
