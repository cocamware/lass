/** @file
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



/** @class lass::spat::world_3d::Cell3D
 *  @brief A 3D Edge
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_CELL_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_CELL_3D_H

#include "../spat_common.h"
#include "world_3d.h"
#include "../../prim/aabb_3d.h"
#include "../../prim/point_3d.h"
#include "../../util/non_copyable.h"

namespace lass
{
namespace spat
{
namespace world_3d
{

template <typename T>
class Cell3D: util::NonCopyable
{
public:

	typedef Cell3D<T> TSelf;

	typedef typename World3D<T> TWorld;
	typedef typename World3D<T>::TFace TFace;
	typedef typename World3D<T>::THandle THandle;
	typedef typename World3D<T>::TObject TObject;
	typedef typename World3D<T>::TParam TParam;
	typedef typename World3D<T>::TPoint TPoint;
	typedef typename World3D<T>::TPointH TPointH;
	typedef typename World3D<T>::TValue TValue;

	typedef std::vector<TFace*> TFaces;
	typedef std::vector<TObject*> TObjects;

	Cell3D(const TWorld& a_world, THandle a_medium);
	~Cell3();

	// METHODS

	const TWorld& world() const;
	THandle medium() const;

	void addFace(TFace& iFace);
	TFaces::size_type sizeFaces() const;
	TFaces::iterator beginFace();
	TFaces::const_iterator beginFace() const;
	TFaces::iterator endFace();
	TFaces::const_iterator endFace() const;
	TFaces::iterator eraseFace(TFaces::iterator);
	void clearFaces();
	void clearFacePointers();

	void addObject(TObject& a_object);
	bool removeObject(TObject& a_object);
	TObjects::size_type sizeObjects() const;
	TObjects::iterator beginObject();
	TObjects::const_iterator beginObject() const;
	TObjects::iterator endObject();
	TObjects::const_iterator endObject() const;

	unsigned visitId() const;
	void setVisitId(unsigned iVisitId);

	TPointH barycenter() const;
	bool contains(const TPoint& iPoint, TParam iSpacing = 0.0) const;
	void eraseObsolete();


	// STATIC METHODS

	static unsigned newVisitId();

private:

	void deleteFace(TFace& iFace);

	// data

	TFaces faces_;
	TObjeccts objects_;
	const TWorld& world_;
	THandle medium_;
	unsigned visitId_;

	// static data

	static unsigned currentVisitId_;
};



}

}

}

#include "cell_3d.inl"

#endif

// EOF
