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



/** @class lass::spat::MeshInterpolator
 *  Interface for interpolators on meshes.
 *  @brief a planar mesh
 *  @author Tom De Muer [TDM]
 *  @relates PlanarMesh
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_MESH_INTERPOLATOR_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_MESH_INTERPOLATOR_H

#include "spat_common.h"
#include "planar_mesh.h"

namespace lass
{
namespace spat
{


template<typename T>
void barycenters( const prim::Point2D<T>& q, const prim::Point2D<T>& a, const prim::Point2D<T>& b, const prim::Point2D<T>& c,
						T& bA, T& bB, T& bC )
{
	typedef prim::Vector2D<T>   TVector2D;

	TVector2D   v02 = a-c;
	TVector2D   v12 = b-c;
	TVector2D   vq2 = q-c;

	T m00 = dot(v02,v02);
	T m01 = dot(v02,v12);
	T m11 = dot(v12,v12);
	T r0 = dot(v02,vq2);
	T r1 = dot(v12,vq2);
	T det = m00*m11-m01*m01;

	LASS_ASSERT( det != 0 );

	T invDet = static_cast<T>(1)/det;

	bA = (m11*r0-m01*r1)*invDet;
	bB = (m00*r1-m01*r0)*invDet;
	bC = static_cast<T>(1) - bA - bB;
}


template<typename T, typename PI>
class MeshInterpolator
{
protected:
	typedef PlanarMesh<T, PI, void, void >  TPlanarMesh;
	MeshInterpolator() {}

	TPlanarMesh mesh_;
	typedef std::list<PI> TInfoList;    /**< type must support stable iterators */
	TInfoList info_;
public:
	typedef typename TPlanarMesh::TPoint2D  TPoint2D;
	typedef prim::Aabb2D<T> TAabb2D;

	MeshInterpolator( const TAabb2D& iAabb );
	virtual ~MeshInterpolator() {}

	virtual void insertSite( const TPoint2D& iPoint, const PI& iPointInfo );
	virtual PI   interpolate( const TPoint2D& iQuery ) const = 0;
};


template<typename T, typename PI>
MeshInterpolator<T,PI>::MeshInterpolator( const TAabb2D& iAabb ) : mesh_( iAabb )
{

}

template<typename T, typename PI>
void MeshInterpolator<T,PI>::insertSite( const TPoint2D& iPoint, const PI& iPointInfo )
{
	typename TPlanarMesh::TEdge* e = mesh_.insertSite( iPoint );
	e = mesh_.locate( iPoint );

	LASS_ASSERT( TPlanarMesh::org(e) == iPoint );

	info_.push_back( iPointInfo );
	TPlanarMesh::setPointHandle( e, &info_.back() );
}


/** @class lass::spat::LinearMeshInterpolator
 *  Interpolator working on triangulated mesh.  Interpolation is done linearly by finding a
 *  triangle containing the point of interest.  Via barycentric coordinates the linear weighted
 *  interpolation is performed and returned.
 *  Buildup of mesh is incremental so after querying for interpolation new points can be inserted.
 *  @brief a linear interpolator using planar meshes
 *  @author Tom De Muer [TDM]
 *  @relates PlanarMesh
 */


template<typename T, typename PI>
class LinearMeshInterpolator : public MeshInterpolator<T,PI>
{
	typedef typename MeshInterpolator<T,PI>::TPlanarMesh TPlanarMesh;

	LinearMeshInterpolator() {}
public:
	typedef typename MeshInterpolator<T,PI>::TPoint2D TPoint2D;
	typedef typename MeshInterpolator<T,PI>::TAabb2D TAabb2D;

	LinearMeshInterpolator( const TAabb2D& iAabb, const PI& iValueOutside );
	virtual ~LinearMeshInterpolator() {}
	virtual PI interpolate( const TPoint2D& iQuery ) const;
};


template<typename T, typename PI>
LinearMeshInterpolator<T,PI>::LinearMeshInterpolator( const TAabb2D& iAabb, const PI& iValueOutside ) : MeshInterpolator<T,PI>( iAabb )
{
	TPoint2D topleft( iAabb.min().x, iAabb.max().y );
	TPoint2D topright( iAabb.max().x, iAabb.max().y );
	TPoint2D bottomleft( iAabb.min().x, iAabb.min().y );
	TPoint2D bottomright( iAabb.max().x, iAabb.min().y );

	typename TPlanarMesh::TEdge*    e;

	info_.push_back(iValueOutside);

	e = mesh_.locate(topleft);
	LASS_ASSERT(TPlanarMesh::org(e) == topleft);
	TPlanarMesh::setPointHandle(e, &info_.back());

	e = mesh_.locate(topright);
	LASS_ASSERT(TPlanarMesh::org(e) == topright);
	TPlanarMesh::setPointHandle( e, &info_.back() );

	e = mesh_.locate(bottomleft);
	LASS_ASSERT(TPlanarMesh::org(e) == bottomleft);
	TPlanarMesh::setPointHandle(e, &info_.back());

	e = mesh_.locate(bottomright);
	LASS_ASSERT(TPlanarMesh::org(e) == bottomright);
	TPlanarMesh::setPointHandle( e, &info_.back() );

}

template<typename T, typename PI>
PI LinearMeshInterpolator<T,PI>::interpolate( const TPoint2D& iQuery ) const
{
	typename TPlanarMesh::TEdge* e=mesh_.locate(iQuery);
	if (!TPlanarMesh::hasLeftFace(e))
		e = e->sym();

	TPoint2D a = TPlanarMesh::org(e);
	TPoint2D b = TPlanarMesh::dest(e);
	TPoint2D c = TPlanarMesh::dest(e->lNext());

	PI* ia = TPlanarMesh::pointHandle(e);
	PI* ib = TPlanarMesh::pointHandle(e->lNext());
	PI* ic = TPlanarMesh::pointHandle(e->lNext()->lNext());

	T ba,bb,bc;

	barycenters(iQuery, a, b, c, ba, bb, bc);

	return (*ia)*ba+(*ib)*bb+(*ic)*bc;
}


}
}
#endif
