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
#include "../meta/null_type.h"

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


template<typename T, typename TPI>
class MeshInterpolator
{
public:
	typedef prim::Aabb2D<T> TAabb2D;
public:
	typedef PlanarMesh<T, TPI*, meta::EmptyType , meta::EmptyType >  TPlanarMesh;
	typedef typename TPlanarMesh::TPoint2D  TPoint2D;
	typedef std::vector<TPoint2D> TPolyLine2D;

	MeshInterpolator( const TAabb2D& iAabb );
	virtual ~MeshInterpolator() {}

	const TPlanarMesh& mesh() const { return mesh_; }
	const TAabb2D& aabb() const { return aabb_; }

	virtual void insertSite( const TPoint2D& iPoint, const TPI& iPointInfo );
	virtual void insertPolyLine( const TPolyLine2D& iPoly, const TPI& iPointInfo );
	virtual TPI   interpolate( const TPoint2D& iQuery ) const = 0;
	template <typename OutputIterator> OutputIterator interpolate(  const TPolyLine2D& iQuery, OutputIterator oOutput ) const;
protected:
	MeshInterpolator() {}

	typedef std::deque<TPI> TInfoList;    /**< type must support stable iterators */
	TInfoList& info() { return info_; }

private:
	TPlanarMesh mesh_;
	TAabb2D		aabb_;

	TInfoList info_;
};


template<typename T, typename TPI>
MeshInterpolator<T,TPI>::MeshInterpolator( const TAabb2D& iAabb ) : mesh_( iAabb )
{
	aabb_ = iAabb;
}

template<typename T, typename TPI>
void MeshInterpolator<T,TPI>::insertSite( const TPoint2D& iPoint, const TPI& iPointInfo )
{
	if (!aabb_.contains( iPoint ))
	{
		LASS_THROW("MeshInterpolator: cannot insert point outside bounding box");
	}

	typename TPlanarMesh::TEdge* e = mesh_.insertSite( iPoint );
	//e = mesh_.locate( iPoint );
	//LASS_ASSERT( TPlanarMesh::org(e) == iPoint );

	info_.push_back( iPointInfo );
	TPlanarMesh::setPointHandle( e, &info_.back() );
}

template<typename T, typename TPI>
void MeshInterpolator<T,TPI>::insertPolyLine( const TPolyLine2D& iPoly, const TPI& iPointInfo )
{
	if (iPoly.size() < 2)
	{
		LASS_THROW("MeshInterpolator: A poly line needs at least two vertices ... poly, remember?");
	}

	for (size_t i = 0; i < iPoly.size(); ++i)
	{
        if (!aabb_.contains( iPoly[i] ))
		{
			LASS_THROW("MeshInterpolator: cannot insert point outside bounding box");
		}
	}

	info_.push_back( iPointInfo );
	for (size_t i = 1; i < iPoly.size(); ++i)
	{
		mesh_.insertEdge(typename TPlanarMesh::TLineSegment2D(iPoly[i - 1], iPoly[i]), meta::EmptyType(),meta::EmptyType(),&info_.back(), true);
	}

	/*
	for (size_t i = 0; i < iPoly.size(); ++i)
	{
        typename TPlanarMesh::TEdge* e = mesh_.locate(iPoly[i]);
		LASS_ASSERT(TPlanarMesh::org(e) == iPoly[i]);
		TPlanarMesh::setPointHandle(e, &info_.back());
	}
	*/
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


template<typename T, typename TPI>
class LinearMeshInterpolator : public MeshInterpolator<T,TPI>
{
public:
	typedef typename MeshInterpolator<T,TPI>::TPoint2D TPoint2D;
	typedef typename MeshInterpolator<T,TPI>::TAabb2D TAabb2D;
	typedef typename MeshInterpolator<T,TPI>::TPolyLine2D TPolyLine2D;
	typedef typename MeshInterpolator<T,TPI>::TPlanarMesh TPlanarMesh;

	LinearMeshInterpolator( const TAabb2D& iAabb, const TPI& iValueOutside );
	virtual ~LinearMeshInterpolator() {}
	TPI interpolate( const TPoint2D& iQuery ) const override;
	template <typename OutputIterator> OutputIterator interpolate(  const TPolyLine2D& iQuery, OutputIterator oOutput ) const;
	
private:
	LinearMeshInterpolator() {}
	virtual TPI interpolate( const TPoint2D& iQuery, typename TPlanarMesh::TEdge* iEdge ) const;
	TPI valueOutside_;
};


template<typename T, typename TPI>
LinearMeshInterpolator<T,TPI>::LinearMeshInterpolator( const TAabb2D& iAabb, const TPI& iValueOutside ) : MeshInterpolator<T,TPI>( iAabb )
{
	valueOutside_ = iValueOutside;
	TPoint2D topleft( iAabb.min().x, iAabb.max().y );
	TPoint2D topright( iAabb.max().x, iAabb.max().y );
	TPoint2D bottomleft( iAabb.min().x, iAabb.min().y );
	TPoint2D bottomright( iAabb.max().x, iAabb.min().y );

	typename TPlanarMesh::TEdge*    e;

	this->info().push_back(iValueOutside);

	e = this->mesh().locate(topleft);
	LASS_ASSERT(TPlanarMesh::org(e) == topleft);
	TPlanarMesh::setPointHandle(e, &this->info().back());

	e = this->mesh().locate(topright);
	LASS_ASSERT(TPlanarMesh::org(e) == topright);
	TPlanarMesh::setPointHandle( e, &this->info().back() );

	e = this->mesh().locate(bottomleft);
	LASS_ASSERT(TPlanarMesh::org(e) == bottomleft);
	TPlanarMesh::setPointHandle(e, &this->info().back());

	e = this->mesh().locate(bottomright);
	LASS_ASSERT(TPlanarMesh::org(e) == bottomright);
	TPlanarMesh::setPointHandle( e, &this->info().back() );

}

template<typename T, typename TPI>
TPI LinearMeshInterpolator<T,TPI>::interpolate( const TPoint2D& iQuery, typename TPlanarMesh::TEdge* e ) const
{
	if (!this->aabb().contains(iQuery))
		return valueOutside_;

	TPoint2D a = TPlanarMesh::fastOrg(e);
	TPoint2D b = TPlanarMesh::fastDest(e);
	TPoint2D c = TPlanarMesh::fastDest(e->lNext());

	TPI* ia = TPlanarMesh::pointHandle(e);
	TPI* ib = TPlanarMesh::pointHandle(e->lNext());
	TPI* ic = TPlanarMesh::pointHandle(e->lNext()->lNext());

	T ba,bb,bc;

	barycenters(iQuery, a, b, c, ba, bb, bc);

	return (*ia)*ba+(*ib)*bb+(*ic)*bc;
}


template<typename T, typename TPI>
TPI LinearMeshInterpolator<T,TPI>::interpolate( const TPoint2D& iQuery ) const
{
	typename TPlanarMesh::TEdge* e = this->mesh().locate(iQuery);
	if (this->mesh().isBoundingPoint(TPlanarMesh::fastOrg(e)) && !TPlanarMesh::hasLeftFace(e))
		e = e->sym();
	return interpolate(iQuery,e);
}

template<typename T, typename TPI>
template <typename OutputIterator> 
OutputIterator LinearMeshInterpolator<T,TPI>::interpolate(  const TPolyLine2D& iQuery, OutputIterator oOutput ) const
{
	/* This function can be optimized.  What is done now is compute all intersection points from a poly line with
	   the mesh.  Subsequent for all these points the interpolated value is determined.  Each of these subsequent interpolations
	   requires a locate (has now been partially optimized away) although it must be clear that that information has already been retrieved by the walkIntersections.  The
	   points of intersection always lie on an edge (of course, or it wouldn't be intersections with edges) and computing the bary-
	   centric coordinates is overkill.  So an optimization consists of:
			* keeping track of the edges passed together with the intersection point
			* compute the values on the org and dest of crossed edges and use this for interpolation
	*/

	LASS_ENFORCE(iQuery.size()>1);
	typedef std::pair<TPoint2D, typename TPlanarMesh::TEdge*> TPointEdgePair;
	std::vector<TPointEdgePair> crossings;
	//TPolyLine2D crossings;
	for (size_t i=0;i<iQuery.size()-1;++i)
	{
		crossings.push_back(TPointEdgePair(iQuery[i],this->mesh().locate(iQuery[i])));
		this->mesh().walkIntersections(typename TPlanarMesh::TLineSegment2D(iQuery[i],iQuery[i+1]),
			std::back_inserter(crossings));
	}
	crossings.push_back(TPointEdgePair(iQuery.back(),this->mesh().locate(iQuery.back())));

	TPoint2D lastInterpolate = crossings[0].first;
	typename TPlanarMesh::TEdge* interpEdge = crossings[0].second;
	if (this->mesh().isBoundingPoint(TPlanarMesh::fastOrg(interpEdge)) && !TPlanarMesh::hasLeftFace(interpEdge))
		interpEdge = interpEdge->sym();
	(*oOutput++) = std::pair<TPoint2D, TPI>(crossings[0].first,interpolate(crossings[0].first,interpEdge));
	for (size_t i=1;i<crossings.size();++i)
	{
		if (crossings[i].first!=lastInterpolate)
		{
			interpEdge = crossings[i].second;
			if (this->mesh().isBoundingPoint(TPlanarMesh::fastOrg(interpEdge)) && !TPlanarMesh::hasLeftFace(interpEdge))
				interpEdge = interpEdge->sym();
			(*oOutput++) = std::pair<TPoint2D, TPI>(crossings[i].first,interpolate(crossings[i].first,crossings[i].second ));
			lastInterpolate = crossings[i].first;
		}
	}
	return oOutput;
}


}
}
#endif
