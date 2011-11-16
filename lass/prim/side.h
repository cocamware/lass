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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIDE_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SIDE_H

#include "prim_common.h"

namespace lass
{
namespace prim
{

/** @enum Side
 *  @brief Different sides of a surface.
 *  @author [BdG]
 *  @date 2003
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
	sInvalid    = 0x00,         /**< 0 is an invalid value */
	sFront      = 0x01,         /**< in front of the surface */
	sLeft		= 0x01,			/**< alias for sFront in 2D */
	sBack       = 0x02,         /**< in back of the surface */
	sRight		= 0x02,			/**< alias for sBack in 2D */
	sInside     = 0x04,         /**< inside the surface */
	sOutside    = 0x08,         /**< outside the surface */
	sSurface    = 0x10          /**< right on the surface */
};



}

}

#endif
