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



#include "test_common.h"
#include "../lass/spat/mesh_interpolator.h"
#include "../lass/io/matlab_o_stream.h"

namespace lass
{
namespace test
{
	void testSpatMeshInterpolator()
	{
		typedef prim::Aabb2D<double> TAabb2D;
		typedef prim::Point2D<double> TPoint2D;
		typedef spat::LinearMeshInterpolator<double, double>    TLinearMeshInterpolator;
		TAabb2D aabb(TPoint2D(0,0),TPoint2D(100,100));
		TLinearMeshInterpolator interpolator(aabb, 0);

		for (double y=10;y<90;y+=20)
		{
			for (double x=10;x<90;x+=20)
			{
				interpolator.insertSite(TPoint2D(x, y), x*y);
			}
		}

		std::ofstream meshIO("meshinterp.txt");
		for (double y=10;y<90;y+=2)
		{
			for (double x=10;x<90;x+=2)
			{
				double value = interpolator.interpolate(TPoint2D(x, y));
				meshIO << value << "\t";
			}
			meshIO << std::endl;
		}
		meshIO.close();

		std::vector<std::pair<TPoint2D, double> > result;
		TLinearMeshInterpolator::TPolyLine2D polyLine;
		polyLine.push_back(TPoint2D(10,10));
		polyLine.push_back(TPoint2D(20,10));
		polyLine.push_back(TPoint2D(30,10));
		polyLine.push_back(TPoint2D(40,10));
		interpolator.interpolate(polyLine,std::back_inserter(result));
		const int n = static_cast<int>(result.size());
		LASS_ASSERT(n >= 0);
		for (int i=0;i<n;++i)
		{
			std::cout << "Interpolation " << i << " : " << result[i].first << " -> " << result[i].second << "\n";
		}
	}


	TUnitTest test_spat_mesh_interpolator()
	{
		return TUnitTest(1, LASS_TEST_CASE(testSpatMeshInterpolator));
	}

}
}
