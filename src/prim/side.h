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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIDE_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SIDE_H

#include "prim_common.h"

namespace lass
{
namespace prim
{

/** @enum Side
 *  @brief Different sides of a surface.
 *	@author [BdG]
 *	@date 2003
 *
 *  Not all possibilities make sense for all surfaces.  e.g. in case of a plane, only
 *  sFront, sSurface, sBack mean something, and in case of a sphere or bounding box you
 *  have sInside, sSurface, sOutside.
 *
 *  Side can be OR'ed, like (sFront | sBack) means in the front or in the back of the surface.
 *  However, return values of Side Foo::classify(const Bar& iBar) normally won't use this,
 *  and the equality test of this enums doesn't understand this neither (which is btw,
 *  a good thing).  So if you try to do 'if (foo.classify(bar) == sFront | sBack)', this will
 *  always evaluate to false, no matter what the return value of classify is.
 *
 *  This is a simple workaround though.  In many cases (sFont | sBack) means exaclty the 
 *  same as !sSurface (OK OK WHAT I JUST HAVE WRITTEN HERE IS NONSES, but you get the idea :)
 *  So, if you want the above test to work, you should test for 
 *  'if (foo.classify(bar) != sSurface)'
 *
 *  In the future there will be another work around with bit and mask operations that will
 *  be available in the util namespace, but I still have to code these :D
 */
enum Side
{
	sInvalid	= 0x00,			/**< 0 is an invalid value */
	sFront		= 0x01,			/**< in front of the surface */
	sBack		= 0x02,			/**< in back of the surface */
	sInside		= 0x04,			/**< inside the surface */
	sOutside	= 0x08,			/**< outside the surface */
	sSurface	= 0x10			/**< right on the surface */
};



}

}

#endif