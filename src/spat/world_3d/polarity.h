/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



/** @class lass::spat::world_3d::Polarity
 *  @brief polarity of pairs and faces
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_POLARITY_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_POLARITY_H

#include "../spat_common.h"

namespace lass
{
namespace spat
{
namespace world_3d
{

class Polarity
{
public:

	typedef Polarity TSelf;

    Polarity(unsigned iValue);

    operator unsigned() const;
    Polarity operator!() const;

    friend Polarity operator&&(Polarity iA, Polarity iB);
    friend Polarity operator||(Polarity iA, Polarity iB);
    friend Polarity operator==(Polarity iA, Polarity iB);
    friend Polarity operator!=(Polarity iA, Polarity iB);

private:

    unsigned value_;
};

Polarity operator&&(Polarity iA, Polarity iB);
Polarity operator||(Polarity iA, Polarity iB);
Polarity operator==(Polarity iA, Polarity iB);
Polarity operator!=(Polarity iA, Polarity iB);

const Polarity positive(1);
const Polarity negative(0);



}

}

}

#endif

// EOF
