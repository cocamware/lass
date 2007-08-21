/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_TRIANGLE_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_SIMPLE_POLYGON_2D_TRIANGLE_2D_H

#include "prim_common.h"
#include "simple_polygon_2d.h"
#include "triangle_2d.h"

namespace lass
{
namespace prim
{

template <typename T, class DegeneratePolicy, typename OutputIterator>
OutputIterator triangulate(
		const SimplePolygon2D<T, DegenerationPolicy>& polygon, OutputIterator triangles)
{
	if (polygon.isConvex())
	{
		// this is the easy one :)
		const size_t n = polygon.size() - 3;
		for (size_t i = 0; i < n; ++i)
		{
			*triangles++ = Triangle2D<T>(polygon[0],polygon[i+1],polygon[i+2]);
		}
		return triangles;
	}

	// we implement the easiest algorithm: ear clipping
	// we try to find a non-reflex vertex and then clip it

#pragma LASS_FIXME("This naive implementation of ear clipping is broken, let's fix it later =) [Bramz]")
	LASS_THROW("triangulation of concave polygons is not implemented yet [Bramz]");
	//SimplePolygon2D<T, DegenerationPolicy> temp(polygon);
	//while (temp.size()>3)
	//{
	//	const int n = temp.size();
	//	for (int i=0;i<n;++i)
	//	{
	//		if (temp.isReflex(i))
	//		{
	//			*triangles++ = push_back(Triangle2D<T>(temp.at(i-1),temp[i],temp.at(i+1)));
	//			temp.erase(i);
	//			break;
	//		}
	//	}
	//}
	//*triangles++ = Triangle2D<T>(temp[0],temp[1],temp[2]);
	//return triangles;
}

}
}

#endif
