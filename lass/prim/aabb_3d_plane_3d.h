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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_PLANE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_PLANE_3D_H

#include "prim_common.h"
#include "aabb_3d.h"
#include "plane_3d.h"

namespace lass
{
namespace prim
{

/*  @relates lass::prim::Plane3D
 *	@sa lass::prim::Aabb3D
 */
template <typename T, typename EP, typename NP, typename MMP>
bool intersects(const Plane3D<T, EP, NP>& plane, const Aabb3D<T, MMP>& box)
{
	typedef typename Plane3D<T, EP, NP>::TVector TVector;
	typedef typename Plane3D<T, EP, NP>::TValue TValue;

	TVector a = box.min().position();
	TVector b = box.max().position();
	TVector n;
	TValue d;
	plane.getCartesian(n, d);

	for (size_t k = 0; k < 3; ++k)
	{
		if (n[k] < 0)
		{
			std::swap(a[k], b[k]);
		}
	}

	return prim::dot(n, a) + d <= 0 && prim::dot(n, b) + d >= 0;
}


}
}

#endif

// EOF
