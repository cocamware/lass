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
#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_REFL_DIFFR_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_REFL_DIFFR_H

#include <complex>

namespace lass
{
namespace num
{


	/** diffractionNord2000.  Computes the diffraction coefficient according to the
	*	Nord2000 standard.  This should be the Hadden-Pierce solution */
	std::complex<double> diffractionNord2000(
			const double& thetaR,const double& thetaS, const double& beta,
			const std::complex<double>& ZR,const std::complex<double>& ZS,
			const double& RS, const double& RR,const double& freq);

	void pyInitNumReflDiffraction(void);
}
}

#endif