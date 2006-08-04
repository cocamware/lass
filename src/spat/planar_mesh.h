/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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




/** @class lass::spat::PlanarMesh
 *  @brief a planar mesh
 *  @author Tom De Muer [TDM]
 *  @par Credits:
 *      This code is inspired on the code of Dani Lischinski distributed under the GPL (Graphics Gems IV)
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_PLANAR_MESH_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_PLANAR_MESH_H

#include "spat_common.h"
#include "quad_edge.h"
#include "../prim/point_2d.h"
#include "../prim/aabb_2d.h"
#include "../prim/ray_2d.h"
#include "../prim/line_2d.h"
#include "../prim/line_segment_2d.h"
#include "../prim/simple_polygon_2d.h"
#include "../prim/triangle_2d.h"
#include "../prim/side.h"
#include "../util/callback_r_1.h"
#include "../util/small_object.h"
#include "../io/matlab_o_stream.h"
#include "../meta/null_type.h"

#define DEBUG_MESH	0

namespace lass
{
namespace spat
{
	#define TEMPLATE_DEF    template< \
		typename T, \
		typename PointHandle, \
		typename EdgeHandle,  \
		typename FaceHandle >

	namespace impl
	{
		TEMPLATE_DEF class EdgeToMatlab;
		TEMPLATE_DEF class EdgeGatherer;
		TEMPLATE_DEF class EdgeMarker;
	
		template < typename PointHandle, typename EdgeHandle, typename FaceHandle >
		class HandleHolder : public lass::util::SmallObject<16384,64,unsigned char>
		{
		public:
			HandleHolder() : pointHandle_(), edgeHandle_(), faceHandle_() {}
			const PointHandle	pointHandle() const { return pointHandle_; }
			const EdgeHandle	edgeHandle() const { return edgeHandle_; }
			const FaceHandle	faceHandle() const { return faceHandle_; }
			PointHandle&		pointHandle(){ return pointHandle_; }
			EdgeHandle&			edgeHandle() { return edgeHandle_; }
			FaceHandle&			faceHandle() { return faceHandle_; }
		private:
			PointHandle	pointHandle_;
			EdgeHandle	edgeHandle_;
			FaceHandle	faceHandle_;
		};

		template < typename PointHandle >
		class HandleHolder< PointHandle, lass::meta::NullType, lass::meta::NullType > // [this is too slow for really large meshes]: public lass::util::SmallObject<16384,64,unsigned char>
		{
		public:
			typedef lass::meta::NullType EdgeHandle;
			typedef lass::meta::NullType FaceHandle;
			HandleHolder() : pointHandle_(NULL) {}
			const PointHandle	pointHandle() const { return pointHandle_; }
			const EdgeHandle	edgeHandle() const { return lass::meta::NullType::Null(); }
			const FaceHandle	faceHandle() const { return lass::meta::NullType::Null(); }
			PointHandle&		pointHandle(){ return pointHandle_; }
			EdgeHandle&			edgeHandle() { return lass::meta::NullType::Null(); }
			FaceHandle&			faceHandle() { return lass::meta::NullType::Null(); }
		private:
			PointHandle	pointHandle_;
		};
	}

	TEMPLATE_DEF
	class PlanarMesh
	{
	public:
		typedef lass::prim::Point2D<T> TPoint2D;
		typedef lass::prim::Vector2D<T> TVector2D;
		typedef lass::prim::Line2D<T, prim::Cartesian, prim::Unnormalized > TLine2D;
		typedef lass::prim::Ray2D<T, prim::Unnormalized, prim::Unbounded > TRay2D;
		typedef lass::prim::LineSegment2D<T> TLineSegment2D;
		typedef lass::prim::SimplePolygon2D<T> TSimplePolygon2D;
		typedef lass::prim::Triangle2D<T> TTriangle2D;

		static const int PLANAR_MESH_STACK_DEPTH = 4;   /**< this determines the maximum nesting depth of forAllVertices and forAllFaces */
	private:
		struct ProxyHandle : public impl::HandleHolder<PointHandle, EdgeHandle, FaceHandle> 
		{
			TPoint2D*       point_;
//#pragma LASS_TODO("Use a long for amortized marking but reuse it for the stack by using the individual bits")
			std::vector<bool>   internalMark_;
			bool            mark_;
			ProxyHandle() : point_(NULL), mark_(false) { internalMark_ = std::vector<bool>(PLANAR_MESH_STACK_DEPTH,false); }
		};
		class StackIncrementer
		{
			int*    stackDepth_;    /**< pointer to int containing real stack depth */
			int     maxStackDepth_; /**< maximum allowed stack depth */
			StackIncrementer() {}
		public:
			StackIncrementer( int* iStackVar, int iMaxStackDepth ) : stackDepth_(iStackVar), maxStackDepth_( iMaxStackDepth )
			{
				++(*stackDepth_);
				if (*stackDepth_>maxStackDepth_)
				{
					--(*stackDepth_);
					throw std::runtime_error("PlanarMesh: stack overflow");
				}
			}
			~StackIncrementer()
			{
				--(*stackDepth_);
			}
		};

	public:
		typedef lass::spat::QuadEdge<ProxyHandle>       TQuadEdge;
		typedef typename TQuadEdge::Edge    TEdge;
		typedef std::vector< TQuadEdge* >     TQuadEdgeList;
		typedef std::vector< ProxyHandle* >   TProxyHandleList;
		typedef lass::util::CallbackR1<bool,TEdge*> TEdgeCallback;

	public:
		PlanarMesh( const TPoint2D& a, const TPoint2D& b, const TPoint2D& c);
		PlanarMesh( const TPoint2D& a, const TPoint2D& b, const TPoint2D& c, const TPoint2D& d);
		PlanarMesh( const prim::Aabb2D<T>& iAabb );
		void setTolerance(const T& iTolerance) {tolerance_ = iTolerance;}	//<* set the relative tolerance */
		const T& tolerance() { return tolerance_; }
		void setPointDistanceTolerance(const T& iPointDistanceTolerance) {pointDistanceTolerance_ = iPointDistanceTolerance;}	//<* set the relative tolerance */
		const T& pointDistanceTolerance() { return pointDistanceTolerance_; }
		virtual ~PlanarMesh();

		TEdge*  startEdge() const;

		void    forAllPrimaryEdges( const TEdgeCallback& iCallback );
		void    forAllPrimaryUndirectedEdges( const TEdgeCallback& iCallback );
		void    forAllDualEdges( const TEdgeCallback& iCallback );
		void    forAllEdges( const TEdgeCallback& iCallback );
		void    forAllVertices( const TEdgeCallback& iCallback );
		void    forAllFaces( const TEdgeCallback& iCallback );

		TEdge*  locate( const TPoint2D& iPoint ) const;				/**< locate an edge of the triangle containing iPoint */
		TEdge*  pointLocate( const TPoint2D& iPoint ) const;		/**< locate an edge of which the org is the same as iPoint, useful for known degeneracy point location, possibly slower than the regular locate */
		TEdge*	shoot( const TRay2D& iRay ) const;			/**< locate the edge found by shooting the ray from within the triangle containt the tail of the ray */
		template <typename OutputIterator>	OutputIterator walk( const TLineSegment2D& iSegment, OutputIterator oCrossedEdges ) const;
		template <typename OutputIterator>	OutputIterator walkIntersections( const TLineSegment2D& iSegment, OutputIterator oIntersections ) const;
		TEdge*  insertSite( const TPoint2D& iPoint, bool makeDelaunay = true, bool forceOnEdge = false);
		TEdge*  insertEdge( const TLineSegment2D& iSegment, EdgeHandle iLeftHandle = EdgeHandle(), EdgeHandle iRightHandle = EdgeHandle(), PointHandle iPointHandle = PointHandle(), bool forcePointHandle = false, bool makeDelaunay = true);
		TEdge*  insertPolygon( const TSimplePolygon2D& iSegment, EdgeHandle iLeftHandle = EdgeHandle(), EdgeHandle iRightHandle = EdgeHandle(), bool makeDelaunay = true);
		void    markPolygon( TEdge* iStartEdge, const TSimplePolygon2D& iPolygon, FaceHandle iFaceHandle );
		template <typename InputPolygonIterator, typename InputFaceHandleIterator>
		void	markPolygons( InputPolygonIterator iFirstPolygon, InputPolygonIterator iLastPolygon, InputFaceHandleIterator iFirstFaceHandle);
		void	markPolygons( const std::vector<TSimplePolygon2D>& iPolygons, const std::vector<FaceHandle>& iFaceHandles);
		bool    deleteEdge( TEdge* iEdge );
		long    edgeCount() const { return edgeCount_; }
		void    makeMaximalConvexPolygon();

		static	TTriangle2D triangle( TEdge* iEdge);	
		static	TSimplePolygon2D polygon( TEdge* iEdge);
		static  const TPoint2D& org( TEdge* iEdge );
		static  const TPoint2D& dest( TEdge* iEdge );
		static  const TPoint2D& fastOrg( TEdge* iEdge );
		static  const TPoint2D& fastDest( TEdge* iEdge );
		static  TPoint2D along( TEdge* iEdge, const T& iParam );
		static  TPoint2D fastAlong( TEdge* iEdge, const T& iParam );
		static	const TVector2D direction( TEdge* iEdge );
		static  bool  rightOf( const TPoint2D& iPoint, TEdge* iEdge );
		static  bool  leftOf( const TPoint2D& iPoint, TEdge* iEdge );
		static  bool  onEdge( const TPoint2D& iPoint, TEdge* iEdge );
		static  bool  hasLeftFace( TEdge* iEdge );
		static  bool  hasRightFace( TEdge* iEdge );
		static  int   chainOrder( TEdge* iEdge );
		static  int   vertexOrder( TEdge* iEdge );
		static	bool  allEqualChainOrder( TEdge* iEdge );
		static  bool  inConvexCell( const TPoint2D& iPoint, TEdge* iEdge );

		static  bool inPrimaryMesh( TEdge* iEdge );
		static  bool inDualMesh( TEdge* iEdge );
		static  bool marking( TEdge* iEdge );
		static  void setMarking( TEdge* iEdge, bool iMark );
		static  PointHandle    pointHandle( TEdge* iEdge );
		static  EdgeHandle     edgeHandle( TEdge* iEdge );
		static  FaceHandle     faceHandle( TEdge* iEdge );
		static  PointHandle&   pointHandleRef( TEdge* iEdge );
		static  EdgeHandle&    edgeHandleRef( TEdge* iEdge );
		static  FaceHandle&    faceHandleRef( TEdge* iEdge );
		static  void setPointHandle( TEdge* iEdge, PointHandle iHandle );
		static  void setEdgeHandle( TEdge* iEdge, EdgeHandle iHandle );
		static  void setFaceHandle( TEdge* iEdge, FaceHandle iHandle );
		static  void setOrientedEdgeHandle( TEdge* iEdge, EdgeHandle iLeftHandle, EdgeHandle iRightHandle, const TVector2D& iDirection );
	private:
		TEdge*  startEdge_;
		TQuadEdgeList       quadEdgeList_;
		std::vector<TPoint2D>	boundingPoints_;

		mutable TEdge*  lastLocateEdge_;
		T		tolerance_;
		T		pointDistanceTolerance_;
		long    edgeCount_;
		int     stackDepth_;
	private:
		PlanarMesh();
		void init4( const TPoint2D& a, const TPoint2D& b, const TPoint2D& c, const TPoint2D& d);

		TEdge* makeEmptyEdge( bool makeConstrained );
		TEdge* makeEdge( const TPoint2D& a, const TPoint2D& b, bool makeConstrained );
		void swap( TEdge* iEdge );
		TEdge* connect( TEdge* a, TEdge* b );
		void triangulate( TEdge* iEdge );
		void fixEdge( TEdge* e );
		void splitEdge(TEdge *e, const TPoint2D& iPoint );
		TEdge*	pointShoot( const TRay2D& iRay ) const;				/**< locate the edge found by shooting the ray from within the triangle with the support of the ray as a known point */
		bool  isBoundingPoint( const TPoint2D& iPoint) const;		/**< true for points defining the boundary */
		template <typename OutputIterator>	OutputIterator pointWalk( const TLineSegment2D& iSegment, OutputIterator oCrossedEdges ) const;
		void safeSplitEdge(TEdge *e, const TPoint2D& iPoint );
		TPoint2D snap(const TPoint2D& a, const TPoint2D& b, const TPoint2D& c);

		TEdge*  bruteForceLocate( const TPoint2D& iPoint ) const;
		TEdge*  bruteForceExactLocate( const TPoint2D& iPoint ) const;

		static  void setOrg( const TPoint2D& iOrg, TEdge* iEdge );
		static  void setDest( const TPoint2D& iDest, TEdge* iEdge );

		friend class impl::EdgeToMatlab<T, PointHandle, EdgeHandle, FaceHandle>;
		friend class impl::EdgeGatherer<T, PointHandle, EdgeHandle, FaceHandle>;
		friend class impl::EdgeMarker<T, PointHandle, EdgeHandle, FaceHandle>;

		bool internalMarking( TEdge* iEdge );
		static bool deletePoint( TEdge* e);
		void setInternalMarking( TEdge* iEdge, bool iMark );
		void setInternalMarkingAroundVertex( typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* iEdge, bool iMark );
		void setInternalMarkingInFace( typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* iEdge, bool iMark );

		void  floodPolygon( TEdge* iStartEdge, const TSimplePolygon2D& iPolygon, FaceHandle iFaceHandle );
		
#ifndef NDEBUG
	public:
		static unsigned numSetOrientedEdgeHandleCalls;
		static unsigned numSetOrientedEdgeHandleSwaps;
#endif
	};

	namespace impl
	{
		/** fastIntersect.  Returns the intersection of line iAiB with iCiD.  This routine is aimed
			at speed and less at accuracy.  This routines assumes there is an intersection point and won't try to be more specific about the error 
			when there is not.  It is used for calculating intersections with walking paths in meshes
		*/
		template <typename T>
		lass::prim::Result fastIntersect(	const lass::prim::Point2D<T>& iA, const lass::prim::Point2D<T>& iB,
								const lass::prim::Point2D<T>& iC, const lass::prim::Point2D<T>& iD, lass::prim::Point2D<T>& oP)
		{
			typedef lass::num::NumTraits<T> TNumTraits;
			const lass::prim::Vector2D<T> dirA=iB-iA;
			const lass::prim::Vector2D<T> difference=iC-iA;
			const lass::prim::Vector2D<T> dirB=iD-iC;

			const T denominator = lass::prim::perpDot(dirA, dirB);
			const T denominator2= denominator*2.0;		// this is (and should be) optimised by adding denominator to itself
			oP = iA;									// interwoven instruction as we assume the zero equality is seldom encountered
			if (denominator == denominator2)
			{
				// we don't bother finding out which case, we only need to know that
				// the result is not what we are looking for
				return lass::prim::rInvalid;
			}
			else
			{
				const T oTa = perpDot(difference, dirB) / denominator;
				oP.x += dirA.x*oTa;
				oP.y += dirA.y*oTa;
				return lass::prim::rOne; // intersecting
			}
		}

		TEMPLATE_DEF
		class EdgeMarker
		{
			typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
			TPlanarMesh* mesh_;
			bool    marking_;
		public:
			EdgeMarker( TPlanarMesh* iMesh, bool iMark ) : mesh_(iMesh), marking_(iMark) {}
			/** internalMark.  This changes internal marking.  This is not user code, unless
			*   your really know what you are doing, use the mark instead */
			bool internalMark( typename TPlanarMesh::TEdge* e )
			{
				mesh_->setInternalMarking( e, marking_ );
				return true;
			}
			bool mark( typename TPlanarMesh::TEdge* e )
			{
				TPlanarMesh::setMarking( e, marking_ );
				return true;
			}

		};

		TEMPLATE_DEF
		class EdgeToMatlab
		{
		public:
			typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
		private:
			TPlanarMesh* mesh_;
			lass::io::MatlabOStream& stream_;
		public:
			EdgeToMatlab( TPlanarMesh* iMesh, lass::io::MatlabOStream& ioOStream ) : mesh_(iMesh), stream_( ioOStream ) {}
			bool edgeToMatlab( typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* iEdge )
			{
				if (!mesh_->internalMarking( iEdge ))
				{
					if (iEdge->isConstrained())
						stream_.setColor(lass::io::mcRed);
					else
						stream_.setColor(lass::io::mcBlack);
					stream_ << typename TPlanarMesh::TLineSegment2D( 
						TPlanarMesh::org(iEdge), TPlanarMesh::dest(iEdge) ) 
						<< std::endl;
				}
				else
					return true;
				mesh_->setInternalMarking( iEdge, true );
				mesh_->setInternalMarking( iEdge->sym(), true );
				return true;
			}
			bool vertexToMatlab( typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* iEdge )
			{
				typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
				if ( !mesh_->internalMarking( iEdge ) )
					stream_ << TPlanarMesh::org(iEdge) << std::endl;
				else
					return true;
				mesh_->setInternalMarkingAroundVertex( iEdge, true );
				return true;
			}
		};

		TEMPLATE_DEF
		class EdgeGatherer
		{
			typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
			TPlanarMesh* mesh_;
			T   lastArea_;
		public:
			typedef std::list< typename TPlanarMesh::TEdge* > TEdgeList;
			TEdgeList   edgeList;

			EdgeGatherer(  TPlanarMesh* iMesh ) : mesh_(iMesh), lastArea_(0) {}
			virtual ~EdgeGatherer() {}

			bool makeConvexPolygon( typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* e )
			{
				LASS_ASSERT( TPlanarMesh::inPrimaryMesh( e ) );
				if ( mesh_->internalMarking( e ) || e->isConstrained())
					return true;

				if (    prim::ccw( TPlanarMesh::org(e->dNext()), TPlanarMesh::dest(e), TPlanarMesh::dest(e->lNext()))
					&&  prim::ccw( TPlanarMesh::org(e->sym()->dNext()), TPlanarMesh::org(e), TPlanarMesh::dest(e->sym()->lNext())) )
				{
					T dArea = num::abs(prim::doubleTriangleArea( TPlanarMesh::org(e->dNext()), TPlanarMesh::dest(e), TPlanarMesh::dest(e->lNext())))
							+ num::abs(prim::doubleTriangleArea( TPlanarMesh::org(e->sym()->dNext()), TPlanarMesh::org(e), TPlanarMesh::dest(e->sym()->lNext())));
					if (lastArea_ == T(0))
					{
						lastArea_ = dArea;
						edgeList.push_back( e );
					}
					else
					{
						if (dArea<lastArea_)
						{
							edgeList.push_front( e );
							lastArea_ = dArea;
						}
						else
							edgeList.push_back( e );
					}
					return false;
				}
				return true;
			}
		};

		TEMPLATE_DEF
		class BrutePointExactLocator
		{
			typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
			typename TPlanarMesh::TPoint2D  point_;
		public:
			typename TPlanarMesh::TEdge*    edge;
			BrutePointExactLocator( const typename TPlanarMesh::TPoint2D& iPoint ) : point_(iPoint), edge(NULL) {}
			bool findPoint( typename TPlanarMesh::TEdge* e )
			{
				if (TPlanarMesh::org(e)==point_)
				{	
					edge = e;
					return false;
				}
				return true;
			}
		};

		TEMPLATE_DEF
		class BrutePointLocator
		{
			typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
			typename TPlanarMesh::TPoint2D  point_;
		public:
			typename TPlanarMesh::TEdge*    edge;
			BrutePointLocator( const typename TPlanarMesh::TPoint2D& iPoint ) : point_(iPoint), edge(NULL) {}
			bool findEdge( typename TPlanarMesh::TEdge* e )
			{
				typename TPlanarMesh::TEdge* ce = e;
				for (int i=0;i<3;++i)
				{
					if (TPlanarMesh::rightOf(point_,ce))
						return true;
					ce = ce->lNext();
				}
				// now search the closest edge
				ce = e;

				if (point_==TPlanarMesh::org(e))
				{
					edge = e;
					return false;
				}
				if (point_==TPlanarMesh::dest(e))
				{
					edge = e->sym();
					return false;
				}
				if (point_==TPlanarMesh::dest(e->lNext()))
				{
					edge = e->lNext()->sym();
					return false;
				}

				TPlanarMesh::TRay2D  R1(TPlanarMesh::org(e), TPlanarMesh::dest(e));
				TPlanarMesh::TPoint2D    p1 = R1.point( R1.t( point_ ) );
				TPlanarMesh::TRay2D  R2(TPlanarMesh::dest(e), TPlanarMesh::org(e->lPrev()));
				TPlanarMesh::TPoint2D    p2 = R2.point( R2.t( point_ ) );
				TPlanarMesh::TRay2D  R3(TPlanarMesh::org(e->lPrev()), TPlanarMesh::org(e));
				TPlanarMesh::TPoint2D    p3 = R3.point( R3.t( point_ ) );

				typename TPlanarMesh::TPoint2D::TValue d1 = squaredDistance(point_,p1);
				typename TPlanarMesh::TPoint2D::TValue d2 = squaredDistance(point_,p2);
				typename TPlanarMesh::TPoint2D::TValue d3 = squaredDistance(point_,p3);

				if ((d1<d2) && (d1<d3))
				{
					edge = e;
					return false;
				}
				if ((d2<d3) && (d2<=d1))
				{
					edge = e->lNext();
					return false;
				}
				edge = e->lPrev();
				return false;

				/*
				T d1 = squaredDistance(TPlanarMesh::org(e),point_);
				T d2 = squaredDistance(TPlanarMesh::dest(e),point_);
				T d3 = squaredDistance(TPlanarMesh::dest(e->lNext()),point_);

				if (d1<d2)
				{
					if (d1<d3)
						edge=e;
					else 
						edge=e->lNext()->sym();
				}
				else
				{
					if (d2<d3)
						edge=e->sym();
					else 
						edge=e->lNext()->sym();
				}
				return false;
				*/
			}
		};

		TEMPLATE_DEF
		class BrutePointLocatorVerbose
		{
			typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
			typename TPlanarMesh::TPoint2D  point_;
		public:
			typename TPlanarMesh::TEdge*    edge;
			std::ofstream stream;
			BrutePointLocatorVerbose( const typename TPlanarMesh::TPoint2D& iPoint ) : point_(iPoint), edge(NULL) {}
			bool findEdge( typename TPlanarMesh::TEdge* e )
			{
				typename TPlanarMesh::TEdge* ce = e;
#if DEBUG_MESH
				stream << "---\n";
				stream << std::setprecision(20);
#endif
				for (int i=0;i<3;++i)
				{
#if DEBUG_MESH
					stream << TPlanarMesh::org(ce) << "-->" << TPlanarMesh::dest(ce) << ":" << prim::doubleTriangleArea(point_,TPlanarMesh::dest(ce),TPlanarMesh::org(ce)) << "\n";
#endif
#pragma LASS_TODO("Get rid of the epsilon!")
					if (prim::doubleTriangleArea(point_,TPlanarMesh::dest(ce),TPlanarMesh::org(ce))>1e-12)
						return true;

					//if (TPlanarMesh::rightOf(point_,ce))
					//	return true;
					ce = ce->lNext();
				}
				// now search the closest point
				ce = e;
				T d1 = squaredDistance(TPlanarMesh::org(e),point_);
				T d2 = squaredDistance(TPlanarMesh::dest(e),point_);
				T d3 = squaredDistance(TPlanarMesh::dest(e->lNext()),point_);

				if (d1<d2)
				{
					if (d1<d3)
						edge=e;
					else 
						edge=e->lNext()->sym();
				}
				else
				{
					if (d2<d3)
						edge=e->sym();
					else 
						edge=e->lNext()->sym();
				}
				return false;
			}
		};

	}

	TEMPLATE_DEF
	lass::io::MatlabOStream& operator<<( lass::io::MatlabOStream& ioOStream, const typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge& iEdge )
	{
		typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
		ioOStream << typename TPlanarMesh::TLineSegment2D( 
					TPlanarMesh::org(iEdge), TPlanarMesh::dest(iEdge) ) 
					<< std::endl;
		return ioOStream;
	}

	TEMPLATE_DEF
	lass::io::MatlabOStream& operator<<( lass::io::MatlabOStream& ioOStream, PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>& iMesh )
	{
		typedef impl::EdgeMarker<T, PointHandle, EdgeHandle, FaceHandle> TEdgeMarker;
		typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
		typedef typename TPlanarMesh::TEdgeCallback TEdgeCallback;
		impl::EdgeToMatlab<T,PointHandle,EdgeHandle,FaceHandle> matlabWriter(&iMesh, ioOStream);
		TEdgeMarker edgeMarker(  &iMesh, false );
		iMesh.forAllPrimaryEdges( TEdgeCallback( &edgeMarker, &TEdgeMarker::internalMark ) );
		iMesh.forAllPrimaryEdges( TEdgeCallback( &matlabWriter, 
			&impl::EdgeToMatlab<T,PointHandle,EdgeHandle,FaceHandle>::edgeToMatlab ) );
		iMesh.forAllPrimaryEdges( TEdgeCallback( &edgeMarker, &TEdgeMarker::internalMark ) );
		iMesh.forAllPrimaryEdges( TEdgeCallback( &matlabWriter, 
			&impl::EdgeToMatlab<T,PointHandle,EdgeHandle,FaceHandle>::vertexToMatlab ) );
		return ioOStream;
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::makeMaximalConvexPolygon()
	{
		typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
		typedef impl::EdgeGatherer<T, PointHandle, EdgeHandle, FaceHandle> TEdgeGatherer;
		typedef impl::EdgeMarker<T, PointHandle, EdgeHandle, FaceHandle> TEdgeMarker;

		while (true)
		{
			TEdgeGatherer edgeGatherer( this );
			TEdgeMarker edgeMarker( this, false );
			forAllPrimaryEdges( TEdgeCallback( &edgeMarker, &TEdgeMarker::internalMark ) );
			edgeGatherer.edgeList.clear();
			forAllPrimaryEdges( TEdgeCallback( &edgeGatherer, &TEdgeGatherer::makeConvexPolygon) );
			if (edgeGatherer.edgeList.size()>0)
				deleteEdge( edgeGatherer.edgeList.front() );
			else
				return;
		}
	}



	TEMPLATE_DEF
	PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::PlanarMesh( const TPoint2D& a, const TPoint2D& b, const TPoint2D& c)
	{
		tolerance_ = 1.0e-8;
		pointDistanceTolerance_ = 1.0e-8;
		TEdge* ea = makeEdge(a, b, true);
		TEdge* eb = makeEdge(b, c, true);
		TEdge* ec = makeEdge(c, a, true);


		TQuadEdge::splice(ea->sym(), eb);
		TQuadEdge::splice(eb->sym(), ec);
		TQuadEdge::splice(ec->sym(), ea);

		startEdge_ = ec;
		lastLocateEdge_ = startEdge_;
		edgeCount_ = 6;
		stackDepth_ = 0;

		boundingPoints_.push_back(a);
		boundingPoints_.push_back(b);
		boundingPoints_.push_back(c);
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::init4( const TPoint2D& a, const TPoint2D& b, const TPoint2D& c, const TPoint2D& d)
	{
		tolerance_ = 1.0e-8;
		pointDistanceTolerance_ = 1.0e-8;

		TEdge* ea = makeEdge(a, b, true);
		TEdge* eb = makeEdge(b, c, true);
		TEdge* ec = makeEdge(c, d, true);
		TEdge* ed = makeEdge(d, a, true);

		TQuadEdge::splice(ea->sym(), eb);
		TQuadEdge::splice(eb->sym(), ec);
		TQuadEdge::splice(ec->sym(), ed);
		TQuadEdge::splice(ed->sym(), ea);

		if (prim::inCircle( c,d,a,b ))
			connect( ec, eb );
		else
			connect( eb, ea );

		startEdge_ = ed;
		lastLocateEdge_ = startEdge_;
		edgeCount_ = 10;
		stackDepth_ = 0;

		boundingPoints_.push_back(a);
		boundingPoints_.push_back(b);
		boundingPoints_.push_back(c);
		boundingPoints_.push_back(d);
	}

	TEMPLATE_DEF
	PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::PlanarMesh( const prim::Aabb2D<T>& iAabb )
	{
		TPoint2D    topleft(iAabb.min().x, iAabb.max().y);
		TPoint2D    topright(iAabb.max().x, iAabb.max().y);
		TPoint2D    bottomleft(iAabb.min().x, iAabb.min().y);
		TPoint2D    bottomright(iAabb.max().x, iAabb.min().y);
		init4(topleft, bottomleft, bottomright, topright);
	}

	TEMPLATE_DEF
	PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::PlanarMesh( const TPoint2D& a, const TPoint2D& b, const TPoint2D& c, const TPoint2D& d)
	{
		init4(a, b, c, d);
	}


	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::deletePoint( typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* iEdge)
	{
		if (iEdge->handle()->point_)
			delete iEdge->handle()->point_;
		TEdge*  currentEdge = iEdge;
		do
		{
			(LASS_ENFORCE(currentEdge->handle()))->point_ = NULL;
			currentEdge = currentEdge->oNext();
		} while ( currentEdge != iEdge );
		return true;
	}

	TEMPLATE_DEF
	PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::~PlanarMesh( )
	{
		forAllVertices( deletePoint );
		typename TQuadEdgeList::iterator qIt;
		for (qIt = quadEdgeList_.begin(); qIt != quadEdgeList_.end();++qIt)
		{
			(*qIt)->edgeDeconstrain();
			(*qIt)->faceDeconstrain();
			(*qIt)->dispose();
			delete *qIt;
		}
	}

	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::makeEmptyEdge( bool makeConstrained )
	{
		TQuadEdge* qe = new TQuadEdge(makeConstrained);
		quadEdgeList_.push_back( qe );
		edgeCount_ += 2;
		
		TEdge*  e = qe->edges();
		/*
		for (int i=0;i<4;++i)
		{
			ProxyHandle*    pHandle = new ProxyHandle;
			e->handle() = pHandle;
			e = e->rot();
		}
		*/
		return e;
	}


	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::connect( TEdge* a, TEdge* b)
	{
		FaceHandle fa = faceHandle(a);
		FaceHandle fb = faceHandle(b);
		if ( fa != fb )
		{
           	LASS_THROW("connect of edges would violate face constraint");
		}
		PointHandle hADest = pointHandle( a->sym() );
		PointHandle hB = pointHandle( b );
		TEdge* e = makeEdge( dest(a), org(b), false);
		TQuadEdge::splice( e, a->lNext() );
		TQuadEdge::splice( e->sym(), b );

		setFaceHandle( e, fa );
		setPointHandle( a->sym(), hADest );
		setPointHandle( b, hB );
		return e;
	}
	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::fixEdge(TEdge *e)
	// A simple, but recursive way of doing things.
	// Flips e, if necessary, in which case calls itself
	// recursively on the two edges that were to the right
	// of e before it was flipped, to propagate the change.
	{
		if (e->isConstrained())
			return;
		TEdge *f = e->oPrev();
		TEdge *g = e->dNext();

		if ( prim::inCircle( dest(e),dest(e->oNext()),org(e),dest(f)) )
		{
			swap(e);
			fixEdge(f);
			fixEdge(g);
		}
	}


	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::safeSplitEdge(TEdge *e, const TPoint2D& iPoint )
	// checks that when the point iPoint is on the edge e that is 
	// really between the org and the dest of e
	// due to numerical roundoff this could be otherwise
	//
	// only call this routine when you are splitting an edge e in two 
	// collinear (or at least expected collinear within numerical roundoff) parts
	{
		TRay2D	testRay1(org(e),dest(e));
		if (testRay1.t(dest(e))<=testRay1.t(iPoint))
		{
			LASS_THROW("Inconsistent splitting of constrained edge");
		}
		TRay2D	testRay2(dest(e),org(e));
		if (testRay2.t(org(e))<=testRay2.t(iPoint))
		{
			LASS_THROW("Inconsistent splitting of constrained edge");
		}
		splitEdge(e,iPoint);
	}


	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::splitEdge(TEdge *e, const TPoint2D& iPoint )
	// Shorten edge e s.t. its destination becomes x. Connect
	// x to the previous destination of e by a new edge. If e
	// is constrained, x is snapped onto the edge, and the new
	// edge is also marked as constrained.

	// remember that this function is rather dumb
	// no snapping is performed to constrained edges
	// all these operations have to be performed _before_ entering this routine
	// the edge e is readily available so it is the responsible of the caller to 
	// make meaningful splits
	{
		const TVector2D dir = direction(e);
		PointHandle    iE = pointHandle(e);
		PointHandle    iES= pointHandle(e->sym());
		EdgeHandle		iLE = edgeHandle(e);
		EdgeHandle		iRE = edgeHandle(e->sym());

		TPoint2D thePoint = iPoint;
		TEdge* t = e->lNext();
		TQuadEdge::splice( e->sym(), t );
		setDest( thePoint, e );
		setPointHandle(e, iE);
		TEdge* ne = connect( e, t );
		if (e->isConstrained())
		{
			//setEdgeHandle(ne, iLE);
			//setEdgeHandle(ne->sym(), iRE);
			setOrientedEdgeHandle( e, iLE, iRE, dir );
			setOrientedEdgeHandle( ne, iLE, iRE, dir );
			ne->quadEdge()->edgeConstrain();
		}

		// make sure the inserted edge also points to correct point information
		LASS_ASSERT( pointHandle(e) == iE );
		LASS_ASSERT( pointHandle(e->lNext()->sym()) == iES );
	}

	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TPoint2D PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::snap(const TPoint2D& x, const TPoint2D& a, const TPoint2D& b)
	{
//#pragma LASS_TODO("This can be more efficient")
		/*
		if (distance(x,a)<tolerance_)
			return a;
		if (distance(x,b)<tolerance_)
			return b;
		*/
		if (x==a)
			return a;
		if (x==b)
			return b;
		T t1 = dot(x-a,b-a);
		T t2 = dot(x-b,a-b);

		T t = std::max(t1,t2) / (t1 + t2);

		if (prim::doubleTriangleArea(x,a,b)==0.0)
			return x;

		if (t1>t2)
		{
			T rx1 = (T(1)-t)*a.x + t*b.x;
			T ry1 = (T(1)-t)*a.y + t*b.y;
			return TPoint2D(rx1,ry1);
		}

		T rx2 = (T(1)-t)*b.x + t*a.x;
		T ry2 = (T(1)-t)*b.y + t*a.y;
		
		return TPoint2D(rx2,ry2);
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::swap( TEdge* iEdge )
	{
		if ( iEdge->isConstrained() )
		{
			LASS_THROW( "cannot swap constrained edge" );
		}
		TEdge* e = iEdge;
		TEdge* a = e->oPrev();
		TEdge* b = e->sym()->oPrev();

		/* following handles do not represent the edges, they represent the points
				a
			   / \
			  b-e-c
			   \ /
				d

		*/
		PointHandle hA = pointHandle(e->oNext()->sym());
		PointHandle hB = pointHandle(e);
		PointHandle hC = pointHandle(e->sym());
		PointHandle hD = pointHandle(e->oPrev()->sym());

		TQuadEdge::splice( e, a );
		TQuadEdge::splice( e->sym(), b );
		TQuadEdge::splice( e, a->lNext() );
		TQuadEdge::splice( e->sym(), b->lNext() );
		setOrg( dest(a), e );       // this is point a here!
		setDest( dest(b), e );      // this is point b here!

		setPointHandle( e, hD );
		setPointHandle( e->sym(), hA );

		LASS_ASSERT( leftOf( dest(e->lNext()), e ) );
		LASS_ASSERT( leftOf( dest(e->lNext()->lNext()), e->lNext() ) );
		LASS_ASSERT( leftOf( dest(e->lNext()->lNext()->lNext()), e->lNext()->lNext() ) );
		LASS_ASSERT( leftOf( dest(e->sym()->lNext()), e->sym() ) );
		LASS_ASSERT( leftOf( dest(e->sym()->lNext()->lNext()), e->sym()->lNext() ) );
		LASS_ASSERT( leftOf( dest(e->sym()->lNext()->lNext()->lNext()), e->sym()->lNext()->lNext() ) );

		LASS_ASSERT( pointHandle( e->oNext()->sym() ) == hB );
		LASS_ASSERT( pointHandle( e->oPrev()->sym() ) == hC );
	}

	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::deleteEdge( TEdge* e )
	{
		if ( e->isConstrained() )
		{
			return false;
		}

		while (startEdge_->quadEdge()==e->quadEdge())
			startEdge_=e->lNext();
		if (lastLocateEdge_->quadEdge()==e->quadEdge())
			lastLocateEdge_ = startEdge_;

		TQuadEdge::splice( e, e->oPrev() );
		TQuadEdge::splice( e->sym(), e->sym()->oPrev() );

		typename TQuadEdgeList::iterator it = std::find(quadEdgeList_.begin(),quadEdgeList_.end(),e->quadEdge());
		std::swap(*it, quadEdgeList_.back());

		TQuadEdge* qe = quadEdgeList_.back();
		qe->edgeDeconstrain();
		qe->faceDeconstrain();
		qe->dispose();
		quadEdgeList_.pop_back();
		delete qe;

		edgeCount_ -= 2;
		return true;
	}

	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::bruteForceLocate( const TPoint2D& iPoint ) const
	{
		typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
		impl::BrutePointLocator<T, PointHandle, EdgeHandle, FaceHandle> bruteLocator( iPoint );
		const_cast<TPlanarMesh*>(this)->forAllFaces( TEdgeCallback( &bruteLocator, &impl::BrutePointLocator<T, PointHandle, EdgeHandle, FaceHandle>::findEdge ) );
		if (bruteLocator.edge==NULL)
		{
			impl::BrutePointLocator<T, PointHandle, EdgeHandle, FaceHandle> bruteLocator( iPoint );
			const_cast<TPlanarMesh*>(this)->forAllPrimaryEdges( TEdgeCallback( &bruteLocator, &impl::BrutePointLocator<T, PointHandle, EdgeHandle, FaceHandle>::findEdge ) );
#if DEBUG_MESH
			if (bruteLocator.edge==NULL)
			{
				impl::BrutePointLocatorVerbose<T, PointHandle, EdgeHandle, FaceHandle> bruteLocatorVerbose( iPoint );
				bruteLocatorVerbose.stream.open("bruteforcelocation.txt");
				const_cast<TPlanarMesh*>(this)->forAllPrimaryEdges( TEdgeCallback( &bruteLocatorVerbose, &impl::BrutePointLocatorVerbose<T, PointHandle, EdgeHandle, FaceHandle>::findEdge ) );
				bruteLocatorVerbose.stream.close();
				return bruteLocatorVerbose.edge;
			}
#endif
			return bruteLocator.edge;
		}
		return bruteLocator.edge;
	}

	// searches for an exact vertex
	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::bruteForceExactLocate( const TPoint2D& iPoint ) const
	{
		typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
		impl::BrutePointExactLocator<T, PointHandle, EdgeHandle, FaceHandle> brutePointLocator( iPoint );
		const_cast<TPlanarMesh*>(this)->forAllVertices( TEdgeCallback( &brutePointLocator, &impl::BrutePointExactLocator<T, PointHandle, EdgeHandle, FaceHandle>::findPoint ) );
		return brutePointLocator.edge;
	}

	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::isBoundingPoint( const TPoint2D& iPoint ) const
	{
		for (int i=0;i<boundingPoints_.size();++i)
		{
			if (boundingPoints_[i]==iPoint)
				return true;
		}
		return false;
	}



	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::locate( const TPoint2D& iPoint ) const
	{
		long edgesPassed = 0;
		TEdge* e = lastLocateEdge_;

		while (edgesPassed<(edgeCount_+2))
		{
continueSearch:
			++edgesPassed;
			if ( org( e ) == iPoint )
				return e;
			if ( dest( e ) == iPoint )
				return e->sym();
			if ( rightOf( iPoint, e ) )
				e = e->sym();

			if (hasLeftFace(e))
			{
				if ( iPoint == org(e->lPrev()) )
					return e->lPrev();
				if ( leftOf( iPoint, e->oNext()) )
				{
					e = e->oNext();
					continue;
				}
				TEdge* ce = e;
#pragma LASS_TODO("Optimize")
				// this for loop is introduced for point location in non-triangular, general
				// convex cells
				for (int i=0;i<chainOrder(e)-2;++i)
				{
					if ( leftOf( iPoint, ce->dPrev()) )
					{
						e = ce->dPrev();
						goto continueSearch;
					}
					ce = ce->lNext();
				}

				TRay2D  R1(org(e), dest(e));
				TPoint2D    p1 = R1.point( R1.t( iPoint ) );
				TRay2D  R2(dest(e), org(e->lPrev()));
				TPoint2D    p2 = R2.point( R2.t( iPoint ) );
				TRay2D  R3(org(e->lPrev()), org(e));
				TPoint2D    p3 = R3.point( R3.t( iPoint ) );

				typename TPoint2D::TValue d1 = squaredDistance(iPoint,p1);
				typename TPoint2D::TValue d2 = squaredDistance(iPoint,p2);
				typename TPoint2D::TValue d3 = squaredDistance(iPoint,p3);

				if (d1*d2*d3 == T(0))
				{
					// yes, we are unlucky
					int a = 0;
				}

				if ((d1<d2) && (d1<d3))
				{
					lastLocateEdge_ = e;
					return lastLocateEdge_;
				}
				if ((d2<d3) && (d2<=d1))
				{
					lastLocateEdge_ = e->lNext();
					return lastLocateEdge_;
				}
				lastLocateEdge_ = e->lPrev();
				return lastLocateEdge_;
			}

			if (onEdge(iPoint,e))
			{
				lastLocateEdge_ = e->sym();
				return e->sym();
			}

			if (rightOf(iPoint, e->oPrev()))
			{
				e = e->oPrev()->sym();
				continue;
			}
			if (rightOf(iPoint,e->dNext()))
			{
				e = e->dNext()->sym();
				continue;
			}
		}
		//throw std::runtime_error("could not locate edge for point, probably point on edge which is not supported yet!");
		
		return bruteForceLocate(iPoint);
	}

	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::pointLocate( const TPoint2D& iPoint ) const
	{
		long edgesPassed = 0;
		TEdge* e = lastLocateEdge_;

		while (edgesPassed<(edgeCount_+2))
		{
			++edgesPassed;
			if ( org( e ) == iPoint )
				return e;
			if ( dest( e ) == iPoint )
				return e->sym();
			if ( rightOf( iPoint, e ) )
				e = e->sym();

			if (hasLeftFace(e))
			{
				if ( iPoint == org(e->lPrev()) )
					return e->lPrev();
				if ( leftOf( iPoint, e->oNext()) )
				{
					e = e->oNext();
					continue;
				}
				if ( leftOf( iPoint, e->dPrev()) )
				{
					e = e->dPrev();
					continue;
				}
			}

			if (rightOf(iPoint, e->oPrev()))
			{
				e = e->oPrev()->sym();
				continue;
			}
			if (rightOf(iPoint,e->dNext()))
			{
				e = e->dNext()->sym();
				continue;
			}
		}
		//throw std::runtime_error("could not locate edge for point, probably point on edge which is not supported yet!");
		e = bruteForceExactLocate(iPoint);
		LASS_ASSERT(e!=NULL);
		return e;
		LASS_THROW( "pointLocate: could not find point");
	}


	/** method is polygon-safe */
	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::shoot( const TRay2D& iRay ) const
	{
		TEdge* locateEdge = locate(iRay.support());
		TEdge* startEdge = locateEdge;
		if (!startEdge)
			return NULL;
		
		if (org(startEdge)==iRay.support())
		{
			TEdge* e(startEdge);
			lass::prim::Side lastSide = iRay.classify(org(e));
			int vOrder = vertexOrder(e);
			for (int i=0;i<vOrder+1;++i)
			{
				/*
				lass::prim::Side currentSide = iRay.classify(dest(e));
				if (lastSide==lass::prim::sRight && lastSide!=currentSide)
					return e;
				lastSide = currentSide;
				*/
				if (	num::abs(prim::doubleTriangleArea(iRay.support(),iRay.point(T(1)),dest(e)))<tolerance_
					&&	iRay.t(dest(e)) > T(0) )
					return e;
				e = e->oNext();
			}
			e = e;
		}

		do	
		{
			TEdge* e(startEdge);
			lass::prim::Side lastSide = iRay.classify(org(e));
			do
			{
				lass::prim::Side currentSide = iRay.classify(dest(e));
				if (currentSide==lass::prim::sSurface)
					return e->sym();
				if (lastSide==lass::prim::sRight && lastSide!=currentSide)
					return e;
				lastSide = currentSide;
				e = e->lNext();
			}
			while (e!=startEdge);
			startEdge = startEdge->oNext();
		} while (startEdge!=locateEdge);

#if DEBUG_MESH
		// recreate the same situation for debugging purposes
		{
			lass::io::MatlabOStream bugMesh;
			bugMesh.open( "shoot_ray.m" );
			bugMesh << std::setprecision(15);
			bugMesh << *const_cast<PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>*>(this);
			bugMesh.close();
			bugMesh.open( "shoot_ray_edges.m" );
			bugMesh.setColor(lass::io::mcBlue);
			bugMesh << TLineSegment2D(iRay.support(),iRay.point(1.0));
			do	
			{
				TEdge* e(startEdge);
				lass::prim::Side lastSide = iRay.classify(org(e));
				bugMesh.setColor(lass::io::mcRed);
				bugMesh << TLineSegment2D(org(startEdge),dest(startEdge));
				do
				{
					bugMesh.setColor(lass::io::mcGreen);
					bugMesh << TLineSegment2D(org(e),dest(e));
					lass::prim::Side currentSide = iRay.classify(dest(e));
					if (lastSide==lass::prim::sRight && lastSide!=currentSide)
						return e;
					lastSide = currentSide;
					e = e->lNext();
				}
				while (e!=startEdge);
				startEdge = startEdge->oNext();
			} while (startEdge!=locateEdge);
			bugMesh.close();
		}
#endif

		return NULL;	// we give up
	}

	/** method is polygon-safe */
	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::pointShoot( const TRay2D& iRay ) const
	{
		TEdge* locateEdge = pointLocate(iRay.support());
		LASS_ASSERT(isBoundingPoint(org(locateEdge)) || allEqualChainOrder(locateEdge));
		TEdge* startEdge = locateEdge;
		if (!startEdge)
			return NULL;
		
		TEdge* e(startEdge);
		lass::prim::Side lastSide = iRay.classify(org(e));
		int vOrder = vertexOrder(e);
		for (int i=0;i<vOrder+1;++i)
		{
			/*
			lass::prim::Side currentSide = iRay.classify(dest(e));
			if (lastSide==lass::prim::sRight && lastSide!=currentSide)
				return e;
			lastSide = currentSide;
			*/
			if (	num::abs(prim::doubleTriangleArea(iRay.support(),iRay.point(T(1)),dest(e)))<tolerance_
				&&	iRay.t(dest(e)) > T(0) )
				return e;
			e = e->oNext();
		}
		e = e;

		do	
		{
			TEdge* e(startEdge);
			lass::prim::Side lastSide = iRay.classify(org(e));
			do
			{
				lass::prim::Side currentSide = iRay.classify(dest(e));
				if (lastSide==lass::prim::sRight && lastSide!=currentSide)
					return e;
				lastSide = currentSide;
				e = e->lNext();
			}
			while (e!=startEdge);
			startEdge = startEdge->oNext();
		} while (startEdge!=locateEdge);

		LASS_THROW("pointShoot, could not find suitable triangle");
		return NULL;
	}


	/* _Adds_ the intersection points of the crossed edges to the output vector, method is polygon safe */
	TEMPLATE_DEF
	template <typename OutputIterator> 
	OutputIterator PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::walkIntersections( const TLineSegment2D& iSegment, OutputIterator oIntersections) const
	{
		std::vector<TEdge*> crossedEdges;
		walk(iSegment,std::back_inserter(crossedEdges));
		for (int i=0;i<crossedEdges.size();++i)
		{
			TPoint2D intersection;
			if (impl::fastIntersect(iSegment.tail(),iSegment.head(),fastOrg(crossedEdges[i]),fastDest(crossedEdges[i]),intersection)!=lass::prim::rOne)
			{
				// we make the bold assumption that we have parallel coinciding lines
				(*oIntersections++) = fastOrg(crossedEdges[i]);
				if (squaredDistance(fastOrg(crossedEdges[i]),fastDest(crossedEdges[i]))<squaredDistance(iSegment.tail(),iSegment.head()))
					(*oIntersections++) = fastDest(crossedEdges[i]);
				else
					(*oIntersections++) = iSegment.head();
			}
			else
			{
				(*oIntersections++) = intersection;
			}
		}
		return oIntersections;
	}


	/* _Adds_ the crossed edges to the output vector, method is polygon safe */
	TEMPLATE_DEF
	template <typename OutputIterator> 
	OutputIterator PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::walk( const TLineSegment2D& iSegment, OutputIterator crossedEdges ) const
	{
		TEdge* e=NULL;
		e = shoot(TRay2D(iSegment.tail(),iSegment.vector()));
		//LASS_ASSERT(isBoundingPoint(org(e)) || allEqualChainOrder(e));
		if (!e)
		{
			LASS_THROW("Could not shoot initial ray in walk");
		}
		if (dest(e)==iSegment.head())
		{
			(*crossedEdges++) = e;
			return crossedEdges;
		}

		while (	!leftOf(iSegment.head(),e) 
			||	(	(num::abs(prim::doubleTriangleArea(dest(e),iSegment.head(),iSegment.tail()))<tolerance_ )
			&&		(num::abs(prim::doubleTriangleArea(org(e),iSegment.head(),iSegment.tail()))<tolerance_ ) ) )
		{
			(*crossedEdges++) = e;
			/*
			TEdge* ne1 = e->sym()->lNext();
			if (cw(iSegment.tail(),iSegment.head(),dest(ne1)))
				e = ne1->lNext();
			else
				e = ne1;
			*/
			TEdge* ce = e->sym()->lNext();
#pragma LASS_TODO("Optimize")
			// this for loop is introduced for point location in non-triangular, general
			// convex cells
			for (int i=0;i<chainOrder(e)-1;++i)
			{
				if ( weakCcw( iSegment.tail(), iSegment.head(), dest(ce) ) )
				{
					e = ce;
					break;
				}
				ce = ce->lNext();
			}
		}
		return crossedEdges;
	}

	/* _Adds_ the crossed edges to the output vector, method is polygon safe */
	TEMPLATE_DEF
	template <typename OutputIterator> 
	OutputIterator PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::pointWalk( const TLineSegment2D& iSegment, OutputIterator crossedEdges ) const
	{
		TEdge* e=NULL;
		e = pointShoot(TRay2D(iSegment.tail(),iSegment.vector()));
		LASS_ASSERT(isBoundingPoint(org(e)) || allEqualChainOrder(e));
		if (!e)
		{
			LASS_THROW("Could not shoot initial ray in pointWalk");
		}
		if (dest(e)==iSegment.head())
		{
			(*crossedEdges++) = e;
			return crossedEdges;
		}

		while (	!leftOf(iSegment.head(),e) 
			||	(	(num::abs(prim::doubleTriangleArea(dest(e),iSegment.head(),iSegment.tail()))<tolerance_ )
			&&		(num::abs(prim::doubleTriangleArea(org(e),iSegment.head(),iSegment.tail()))<tolerance_ ) ) )
		{
			(*crossedEdges++) = e;
			/*
			TEdge* ne1 = e->sym()->lNext();
			if (cw(iSegment.tail(),iSegment.head(),dest(ne1)))
				e = ne1->lNext();
			else
				e = ne1;
			*/
			TEdge* ce = e->sym()->lNext();
#pragma LASS_TODO("Optimize")
			// this for loop is introduced for point location in non-triangular, general
			// convex cells
			for (int i=0;i<chainOrder(e)-1;++i)
			{
				if ( weakCcw( iSegment.tail(), iSegment.head(), dest(ce) ) )
				{
					e = ce;
					break;
				}
				ce = ce->lNext();
			}
		}
		return crossedEdges;
	}


	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::triangulate( TEdge* iEdge )
	// Triangulates the left face of first, which is assumed to be closed.
	// It is also assumed that all the vertices of that face lie to the
	// left of last (the edge preceeding first).
	// This is NOT intended as a general simple polygon triangulation
	// routine. It is called by InsertEdge in order to restore a
	// triangulation after an edge has been inserted.
	// The routine consists of two loops: the outer loop continues as
	// long as the left face is not a triangle. The inner loop examines
	// successive triplets of vertices, creating a triangle whenever the
	// triplet is counterclockwise.
	{
		TEdge *first = iEdge;
		TEdge *a, *b, *e, *t1, *t2, *last = first->lPrev();

		while (first->lNext()->lNext() != last)
		{
			e = first->lNext();
			t1 = first;
			while (e != last)
			{
				t2 = e->lNext();
				if (t2 == last && t1 == first)
					break;
				if (leftOf(dest(e),t1))
				{
					if (t1 == first)
						t1 = first = connect(e, t1)->sym();
					else
						t1 = connect(e, t1)->sym();
					a = t1->oPrev(), b = t1->dNext();
					fixEdge(a);
					fixEdge(b);
					e = t2;
				}
				else
				{
					t1 = e;
					e = t2;
				}
			}
		}
		a = last->lNext(), b = last->lPrev();
		fixEdge(a);
		fixEdge(b);
		fixEdge(last);
	}


	/** makeDelaunay: after insertion of the site force a _Delaunay_ triangulation
	*	forceOnEdge : points are considered on the edge although due to numerical roundoff they
	*				  maybe not, as user you should let this default to false or you'd better
	*				  be knowning damn well what you are doing :-D
	*/
	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::insertSite( const TPoint2D& iPoint, bool makeDelaunay, bool forceOnEdge )
	{
		TEdge* e = locate( iPoint );
		if (e==NULL)
		{
			locate( iPoint );
			LASS_ASSERT( e!=NULL);
		}
		T sqDistOrg	= prim::squaredDistance(org(e),iPoint);
		T sqDistDest = prim::squaredDistance(dest(e),iPoint);

		if (sqDistOrg<pointDistanceTolerance_*pointDistanceTolerance_)
			return e;
		if (sqDistDest<pointDistanceTolerance_*pointDistanceTolerance_)
			return e->sym();

		bool hasLeft = hasLeftFace(e);
		bool hasRight = hasRightFace(e);

		if (!hasLeft && !hasRight)
		{
#if DEBUG_MESH
			lass::io::MatlabOStream bugMesh;
			bugMesh.open( "bugMesh.m" );
			bugMesh << *this;
			bugMesh.close();
#endif
			throw std::runtime_error("insertSite: edge does not have any faces");
		}

		bool isOnEdge = onEdge(iPoint, e) || forceOnEdge;
		// the distance to lines is enlarged a bit for greater stability towards distances to line calculations
		if (!isOnEdge && TLine2D(org(e),dest(e)).squaredDistance(iPoint)<1.05*pointDistanceTolerance_*pointDistanceTolerance_)
		{
			// snap the point to the line, this is a precaution to avoid numerical 
			// instability later on
			isOnEdge = true;
			TPoint2D x = snap(iPoint, org(e), dest(e));
			return insertSite(x,makeDelaunay,true);
		}

		bool insideLeft  = hasLeft && (isOnEdge || leftOf(iPoint, e)) &&
						   rightOf(iPoint, e->oNext()) && rightOf(iPoint, e->dPrev());
		bool insideRight = hasRight && (isOnEdge || rightOf(iPoint, e)) &&
						   leftOf(iPoint, e->oPrev()) && leftOf(iPoint, e->dNext());

		// check for numerical stability
		// we check if upon insertion the orientation tests still give meaningful results
		{
			TPoint2D p1 = org(e);
			TPoint2D p2 = dest(e);
			TPoint2D p3 = dest(e->lNext());

			T s1 = prim::doubleTriangleArea(iPoint,p1,p2);
			T s2 = prim::doubleTriangleArea(iPoint,p2,p3);
			T s3 = prim::doubleTriangleArea(iPoint,p3,p1);
			
			if (isOnEdge)
			{
				if ( s1*s2 < T(0) || s2*s3 < T(0) || s1*s3 < T(0) )
				{
					if (squaredDistance(org(e),iPoint)<squaredDistance(dest(e),iPoint))
					{
						if (squaredDistance(org(e),iPoint)<pointDistanceTolerance_*pointDistanceTolerance_)
							return insertSite(org(e),makeDelaunay,true);
						else
						{
							// we have come across a situation which is combinatorially not possible except for
							// numerical issues, we just continue as the algorithm can cope with this
						}
					}
					else
					{
						if (squaredDistance(dest(e),iPoint)<pointDistanceTolerance_*pointDistanceTolerance_)
							return insertSite(dest(e),makeDelaunay,true);
						else
						{
							// we have come across a situation which is combinatorially not possible except for
							// numerical issues, we just continue as the algorithm can cope with this
						}
					}
				}
			}
			else
			{
				LASS_ASSERT( s1*s2 > T(0) );
				LASS_ASSERT( s2*s3 > T(0) );
				LASS_ASSERT( s1*s3 > T(0) );
			}
		}


		if (insideLeft  && iPoint == dest(e->oNext()))
			return e->lPrev();
		if (insideRight && iPoint == dest(e->oPrev()))
			return e->dNext();

		if ( isOnEdge )
		{
			static int passCount2 = 0;
			++passCount2;
			// snap x to e, and check for coincidence:
			TPoint2D x = snap(iPoint, org(e), dest(e));
			T sqDistOrg	= prim::squaredDistance(org(e),x);
			T sqDistDest = prim::squaredDistance(dest(e),x);

			if (sqDistOrg<pointDistanceTolerance_*pointDistanceTolerance_)
				return e;
			if (sqDistDest<pointDistanceTolerance_*pointDistanceTolerance_)
				return e->sym();

			// bummer
			if (hasRight && hasLeft)
			{
				// has two faces
				TEdge *a, *b, *c, *d;
				a = e->oPrev();
				b = e->dNext();
				c = e->lNext();
				d = e->lPrev();
				safeSplitEdge(e, x);
				connect(e, e->lPrev());
				connect(e->oPrev(), e->sym());
				fixEdge(a);
				fixEdge(b);
				fixEdge(c);
				fixEdge(d);
			}
			else
			{
				// has only one face
				if (hasRight)
					e = e->sym();
				TEdge *c = e->lNext();
				TEdge *d = e->lPrev();
				safeSplitEdge(e, x);
				connect(e, e->lPrev());
				fixEdge(c);
				fixEdge(d);
			}
			int vOrder = vertexOrder(e->sym());
			LASS_ASSERT(vOrder>2);
			LASS_ASSERT(x==dest(e));
			LASS_ASSERT(allEqualChainOrder(e->sym()));
			return e->sym();
		}

		// unused?  [Bramz]
		// int chOrder = chainOrder(e);

		PointHandle pH = pointHandle(e);
		TEdge* base = makeEdge(org(e),iPoint,false);
		TQuadEdge::splice(base,e);
		setPointHandle(e, pH);
		TEdge* startBase = base->sym();

		do
		{
			base = connect( e, base->sym() );
			e = base->oPrev();
		} while (e->dPrev()!=startBase);

		if ( makeDelaunay )
		{
			// try to build a delaunay mesh
			while (true)
			{
				TEdge* t=e->oPrev();
				if ( !e->isConstrained() && prim::inCircle(org(e),dest(t),dest(e),iPoint))
				{
					swap(e);
					e=e->oPrev();
				}
				else
				{
					if (e->lPrev()==startBase)
						return base->sym();
					else
						e = e->oNext()->lPrev();
				}
			}
		}
		int vOrder = vertexOrder(base->sym());
		LASS_ASSERT(vOrder>2);
		LASS_ASSERT(iPoint==dest(base));
		LASS_ASSERT(allEqualChainOrder(base->sym()));
		return base->sym();
	}


	/** insertEdge.  Inserts a constrained edge into the planar mesh.  Edges on the lefthand
	*	side of the edge will be assigned iLeftHandle, others, iRightHandle.  All inserted points
	*	will be assigned the point handle, in case the iPointHandle is different from the NullType.
	*/
	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::insertEdge( const TLineSegment2D& iSegment, EdgeHandle iLeftHandle, EdgeHandle iRightHandle, PointHandle iPointHandle, bool iForcePointHandle, bool makeDelaunay )
	{
#if DEBUG_MESH
		std::cout << "Inserting : " << iSegment << "\n";
		static int passCountMesh = 0;
		++passCountMesh;
		char buf[128];
		sprintf(buf,"debug_mesh_%d.m",passCountMesh);
		lass::io::MatlabOStream debugMesh;
		debugMesh.open(buf);
		debugMesh << *this;
		debugMesh.close();
#endif
		TEdge *ea, *eb;
		TPoint2D aa, bb, fbb, faa;
		if (ea = insertSite(iSegment.tail()),makeDelaunay)
		{
			aa = org(ea);
			if (iForcePointHandle)
				setPointHandle(ea, iPointHandle);

		}
		if (eb = insertSite(iSegment.head()),makeDelaunay)
		{
			bb = org(eb);
			if (iForcePointHandle)
				setPointHandle(eb, iPointHandle);
		}
		fbb = bb;
		faa = aa;

		if (ea == NULL || eb == NULL)
		{
			LASS_THROW("insertEdge: could not insert endpoints of edge");
		}

		if (org(ea)!=aa)
		{
			ea=locate(aa);
			if (ea==NULL)
			{
				LASS_THROW("insertEdge: could not locate an endpoint");
			}

			if (!(aa == org(ea)))
				ea = ea->sym();
			if (!(aa == org(ea)))
			{
				LASS_THROW("insertEdge: point a is not an endpoint of ea");
			}
		}

		if (aa==bb)
		{
			return ea;
		}
		
		// first case : edge is present, constrain it
		TEdge* ce = ea;
		int vOrder = vertexOrder(ea);
		for (int i=0;i<vOrder;++i)
		{
			if (dest(ce)==fbb)
			{
				ce->quadEdge()->edgeConstrain();
				setOrientedEdgeHandle( ce, iLeftHandle, iRightHandle, iSegment.vector() );
				return ce;
			}
			ce = ce->oNext();
		}
		if (distance(aa,bb)<pointDistanceTolerance_ )
		{
			LASS_THROW("insertEdge: both ends map to same vertex within requested numerical precision");
		}
		ce = ea;
		for (int i=0;i<vOrder;++i)
		{
			// if is almost in line we also take it
			if (	prim::dot(direction(ce), iSegment.vector()) > T(0)	
				&&	num::abs(lass::prim::doubleTriangleArea(dest(ce),faa,fbb))<tolerance_) 
			{
				ce->quadEdge()->edgeConstrain();
				setOrientedEdgeHandle( ce, iLeftHandle, iRightHandle, iSegment.vector() );

				return insertEdge( TLineSegment2D( dest(ce), fbb), iLeftHandle, iRightHandle, iPointHandle, iForcePointHandle, makeDelaunay );
			}

			ce = ce->oNext();
		}
		
		TVector2D segmentDirection(fbb-faa);
		std::vector< TEdge* >	insertedEdges;		// edges having as origin newly inserted points
		std::vector< TPoint2D > insertedPoints;		// newly inserted points
		std::vector< TPoint2D > finalInsertedPoints;		// newly inserted points
		std::vector< TEdge* >	crossedEdges;			// crossed edges by new segment
		std::vector< TEdge* >	filteredEdges;			// crossed edges by new segment
		bool tookAction = false;
		static int passCount=0;
		++passCount;

		LASS_ASSERT(allEqualChainOrder(ea));
		LASS_ASSERT(allEqualChainOrder(eb));
		pointWalk(TLineSegment2D(faa,fbb),std::back_inserter(crossedEdges));
#if DEBUG_MESH
		std::cout << "Walk returned segments " << crossedEdges.size() << "\n";
		std::cout << std::setprecision(15);
#endif 

		// search all edges to split
		insertedPoints.push_back(faa);
		for (int i=0;i<crossedEdges.size();++i)
		{
			bool computeIntersection = (crossedEdges[i]->isConstrained());
			TPoint2D eorg = org(crossedEdges[i]);
			TPoint2D edest= dest(crossedEdges[i]);
			bool noCommonPoints = eorg!=faa && eorg!=fbb &&  edest!=faa && edest!=fbb;
			if (	computeIntersection	
				&&	noCommonPoints)
			{
				TLine2D other(eorg,edest);
				TPoint2D x;

				if (prim::intersect(TLine2D(aa,bb),other,x)==prim::rOne)
				{
					TEdge* npe = insertSite(x,true,true);
					TPoint2D nx = org(npe);

					// the reverse test now for degeneration avoidance
					// this avoids that the crossed edges is only touching the walked path
					// noCommonPoints =  nx != eorg && nx != edest;

					if (nx!=insertedPoints.back())
					{
						insertedPoints.push_back(nx);
#if DEBUG_MESH
						std::cout << "-> Intersection " << x << " filtered to " << nx << "\n";
#endif
						break;
					}
				}
			}
			if (noCommonPoints)
				filteredEdges.push_back(crossedEdges[i]);
		}
		if (fbb!=insertedPoints.back())
			insertedPoints.push_back(fbb);
		
		// we found some intersections, recursively insert the subparts of the line segments
		if (insertedPoints.size()>2)
		{
			TEdge* ce = NULL;
			TEdge* bce = NULL;
			for (int i=0;i<insertedPoints.size()-1;++i)
			{
				ce = insertEdge( TLineSegment2D(insertedPoints[i],insertedPoints[i+1]), iLeftHandle, iRightHandle, iPointHandle, iForcePointHandle, makeDelaunay );
				if (i==0)
					bce = ce;
			}
			if ( org(bce) != faa )
			{
				bce = pointLocate( faa );
			}
			LASS_ASSERT( org(bce)==faa);

			return bce;
		}

		std::vector< TEdge* >	toSwapEdges;		// edges which are not swappable yet will be hold back till the end
													// this will only happen once, if not we have a problem and we retry
													// clearing the path recursively
		bool madeConstraint = false;
		// no intersections found, we clear the path by swapping edges
		for (int i=0;i<filteredEdges.size();++i)
		{
			// is the edge swappable without creating an inside-out triangle?
			TPoint2D a = org(filteredEdges[i]);
			TPoint2D b = dest(filteredEdges[i]);
			TPoint2D c = dest(filteredEdges[i]->lNext());
			TPoint2D d = dest(filteredEdges[i]->sym()->lNext());
			bool swappedCcw1 = prim::ccw(d,b,c);
			bool swappedCcw2 = prim::ccw(a,d,c);
			if (!swappedCcw1 || !swappedCcw2)
			{
#if DEBUG_MESH
				lass::io::MatlabOStream bugMesh;
				bugMesh.open( "swap_constrained.m" );
				bugMesh << *this;
				for (int j=0;j<filteredEdges.size();++j)
				{
					bugMesh.setColor( lass::io::mcBlue);
					bugMesh << TLineSegment2D(org(filteredEdges[j]),dest(filteredEdges[j]));
				}
				bugMesh.setColor( lass::io::mcRed );
				bugMesh << TLineSegment2D(a,b);
				bugMesh.setColor( lass::io::mcGreen );
				bugMesh << TLineSegment2D(faa,fbb);
				bugMesh.close();
#endif
				toSwapEdges.push_back(filteredEdges[i]);

				//LASS_THROW("could not swap edge without creating inside-out triangle");
			}
			else
			{
				if (filteredEdges[i]->quadEdge()->isConstrained())
				{
					LASS_THROW("found constrained edge where I should not find one!");
				}
				swap(filteredEdges[i]);
				TPoint2D eorg = org(filteredEdges[i]);
				TPoint2D edest =  dest(filteredEdges[i]);
				T onEdgeOrg = num::abs(prim::doubleTriangleArea(eorg,edest,faa));
				T onEdgeDest = num::abs(prim::doubleTriangleArea(eorg,edest,fbb));
				T reverseTest1 = num::abs(prim::doubleTriangleArea(eorg,faa,fbb));
				T reverseTest2 = num::abs(prim::doubleTriangleArea(edest,faa,fbb));
				if (onEdgeOrg<tolerance_ && onEdgeDest<tolerance_)
				{
					filteredEdges[i]->quadEdge()->edgeConstrain();
	#if DEBUG_MESH
					std::cout << "@ Constraining " << org(filteredEdges[i]) << "-->" << dest(filteredEdges[i]) << "\n";
	#endif
					setOrientedEdgeHandle( filteredEdges[i], iLeftHandle, iRightHandle, iSegment.vector() );
					if (toSwapEdges.size()>0)
					{
						LASS_THROW("edges for delayed swapping found");
					}
					madeConstraint = true;
					break;
				}
			}
		}

		if (toSwapEdges.size()>0)
			return insertEdge( iSegment, iLeftHandle, iRightHandle, iPointHandle, iForcePointHandle, makeDelaunay );
		if (!madeConstraint)
		{
			LASS_THROW("could not force constrained edge");
		}
		LASS_ASSERT( org(ea) == faa );
		return ea;
	}


	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge*  PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::insertPolygon( const TSimplePolygon2D& iPolygon, EdgeHandle iLeftHandle, EdgeHandle iRightHandle, bool makeDelaunay)
	{
		TEdge* e = NULL;
		for (size_t i=1;i<iPolygon.size();++i)
		{
			e=insertEdge(TLineSegment2D(iPolygon[i-1],iPolygon[i]),iLeftHandle,iRightHandle);
		}
		e=insertEdge(TLineSegment2D(iPolygon[iPolygon.size()-1],iPolygon[0]),iLeftHandle,iRightHandle);
		return e;
	}

	/** marks all faces which have their barycentrum inside the given polygon with the provided handle */
	TEMPLATE_DEF
	void  PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::markPolygon( TEdge* iStartEdge, const TSimplePolygon2D& iPolygon, FaceHandle iFaceHandle = FaceHandle())
	{	
		typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
		typedef impl::EdgeMarker<T, PointHandle, EdgeHandle, FaceHandle> TEdgeMarker;
		StackIncrementer( &stackDepth_, PLANAR_MESH_STACK_DEPTH );
		TEdgeMarker edgeMarker( this, false );
		forAllPrimaryEdges( TEdgeCallback( &edgeMarker, &TEdgeMarker::internalMark ) );
#pragma LASS_TODO("do a more efficient marking or allow for multiple marking")

		floodPolygon( iStartEdge->sym(), iPolygon, iFaceHandle );
		floodPolygon( iStartEdge->lNext()->sym(), iPolygon, iFaceHandle );
		floodPolygon( iStartEdge->lNext()->lNext()->sym(), iPolygon, iFaceHandle );
	}

	/** marks all faces which have their barycentrum inside the given polygon with the provided handle */
	TEMPLATE_DEF
	template <typename InputPolygonIterator, typename InputFaceHandleIterator>
	void  PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::markPolygons( InputPolygonIterator iFirstPolygon, InputPolygonIterator iLastPolygon, InputFaceHandleIterator iFirstFaceHandle )
	{
		typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
		typedef impl::EdgeMarker<T, PointHandle, EdgeHandle, FaceHandle> TEdgeMarker;
		StackIncrementer( &stackDepth_, PLANAR_MESH_STACK_DEPTH );
		TEdgeMarker edgeMarker( this, false );
		forAllPrimaryEdges( TEdgeCallback( &edgeMarker, &TEdgeMarker::internalMark ) );

		while (iFirstPolygon != iLastPolygon)
		{
			TEdge* iStartEdge = locate(*iFirstPolygon[0]);
			floodPolygon( iStartEdge->sym(), *iFirstPolygon, *iFirstFaceHandle );
			floodPolygon( iStartEdge->lNext()->sym(), *iFirstPolygon, *iFirstFaceHandle );
			floodPolygon( iStartEdge->lNext()->lNext()->sym(), *iFirstPolygon, *iFirstFaceHandle );
			++iFirstPolygon;
			++iFirstFaceHandle;
		}
	}

	/** marks all faces which have their barycentrum inside the given polygon with the provided handle */
	TEMPLATE_DEF
	void  PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::markPolygons( const std::vector<TSimplePolygon2D>& iPolygons, const std::vector<FaceHandle>& iFaceHandles)
	{	
		if (iPolygons.size()!=iFaceHandles.size())
		{
			LASS_THROW("markPolygons: list of polygons must fit list of face handles");
		}
		markPolygons(iPolygons.begin(), iPolygons.end(), iFaceHandles.begin());
	}


	TEMPLATE_DEF
	void  PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::floodPolygon( TEdge* iStartEdge, const TSimplePolygon2D& iPolygon, FaceHandle iFaceHandle = FaceHandle())
	{	
		TPoint2D bary = triangle(iStartEdge).surfaceCentroid().affine();
		if (iPolygon.contains(bary) && !internalMarking(iStartEdge))
		{
			setInternalMarking( iStartEdge, true );
			setFaceHandle( iStartEdge, iFaceHandle );
			floodPolygon( iStartEdge->sym(), iPolygon, iFaceHandle );
			floodPolygon( iStartEdge->lNext()->sym(), iPolygon, iFaceHandle );
			floodPolygon( iStartEdge->lNext()->lNext()->sym(), iPolygon, iFaceHandle );
		}
	}

	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::makeEdge( const TPoint2D& a, const TPoint2D& b, bool makeConstrained )
	{
		TEdge* e(makeEmptyEdge( makeConstrained ));

		TPoint2D*   na(new TPoint2D( a ));
		TPoint2D*   nb(new TPoint2D( b ));
		e->handle()->point_ = na;
		e->sym()->handle()->point_ = nb;

		return e;
	}


	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::startEdge() const
	{
		return startEdge_;
	}


	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::inPrimaryMesh( TEdge* iEdge )
	{
		return (iEdge->index() == 2) || (iEdge->index() == 0);
	}

	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::inDualMesh( TEdge* iEdge )
	{
		return !inPrimaryMesh( iEdge );
	}

	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TTriangle2D PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::triangle( TEdge* iEdge)
	{
		if (!inPrimaryMesh(iEdge))
		{
			LASS_THROW("PlanarMesh::triangle: edge not in primary mesh");
		}
		return TTriangle2D(org(iEdge),dest(iEdge),dest(iEdge->lNext()));
	}

	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TSimplePolygon2D PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::polygon( TEdge* iEdge)
	{
		if (!inPrimaryMesh(iEdge))
		{
			LASS_THROW("PlanarMesh::polygon: edge not in primary mesh");
		}
		TSimplePolygon2D poly;
		TEdge* cEdge = iEdge;
		do
		{
			poly.add(org(cEdge));
			cEdge = cEdge->lNext();
		} while (cEdge!=iEdge);
		return poly;
	}

	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::inConvexCell( const TPoint2D& iPoint, TEdge* iEdge)
	{
		if (!inPrimaryMesh(iEdge))
		{
			LASS_THROW("PlanarMesh::polygon: edge not in primary mesh");
		}
		TEdge* cEdge = iEdge;
		do
		{
			if (rightOf(iPoint, cEdge))
				return false;
			cEdge = cEdge->lNext();
		} while (cEdge!=iEdge);
		return true;
	}

	TEMPLATE_DEF
	const typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TPoint2D& PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::org( TEdge* iEdge )
	{
		if (!inPrimaryMesh( iEdge ))
		{
			LASS_THROW("PlanarMesh::org: edge not in primary mesh");
		}
		return *iEdge->handle()->point_;
	}

	TEMPLATE_DEF
	const typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TPoint2D& PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::fastOrg( TEdge* iEdge )
	{
		return *iEdge->handle()->point_;
	}

	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TPoint2D PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::along( TEdge* iEdge, const T& iParam )
	{
		if (!inPrimaryMesh( iEdge ))
		{
			LASS_THROW("PlanarMesh::along: edge not in primary mesh");
		}
		const TPoint2D& eOrg = *iEdge->handle()->point_;
		const TPoint2D& eDest = *iEdge->sym()->handle()->point_;
		const T oX = eDest.x*iParam + (1-iParam)*eOrg.x;
		const T oY = eDest.y*iParam + (1-iParam)*eOrg.y;
		return TPoint2D(oX,oY);
	}

	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TPoint2D PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::fastAlong( TEdge* iEdge, const T& iParam )
	{
		const TPoint2D& eOrg = *iEdge->handle()->point_;
		const TPoint2D& eDest = *iEdge->sym()->handle()->point_;
		const T oX = eDest.x*iParam + (1-iParam)*eOrg.x;
		const T oY = eDest.y*iParam + (1-iParam)*eOrg.y;
		return TPoint2D(oX,oY);
	}


	TEMPLATE_DEF inline
	const typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TPoint2D& PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::dest( TEdge* iEdge )
	{
		return org( iEdge->sym() );
	}

	TEMPLATE_DEF inline
	const typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TPoint2D& PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::fastDest( TEdge* iEdge )
	{
		return fastOrg( iEdge->sym() );
	}

	TEMPLATE_DEF inline
	const typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TVector2D PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::direction( TEdge* iEdge )
	{
		return dest( iEdge ) - org( iEdge );
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::setOrg( const TPoint2D& iPoint, TEdge* iEdge )
	{
		LASS_ASSERT( inPrimaryMesh( iEdge ) );
		*iEdge->handle()->point_ = iPoint;
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::setDest( const TPoint2D& iPoint, TEdge* iEdge )
	{
		LASS_ASSERT( inPrimaryMesh( iEdge ) );
		*iEdge->sym()->handle()->point_ = iPoint;
	}

	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::rightOf( const TPoint2D& iPoint, TEdge* iEdge )
	{
		return lass::prim::ccw( iPoint, dest(iEdge), org(iEdge) );
	}

	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::leftOf( const TPoint2D& iPoint, TEdge* iEdge )
	{
		return lass::prim::ccw( iPoint, org(iEdge), dest(iEdge) );
	}

	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::onEdge( const TPoint2D& iPoint, TEdge* iEdge )
	{
		if (prim::doubleTriangleArea( iPoint, org(iEdge), dest(iEdge)) == T(0) )
		{
			TLineSegment2D eSeg(org(iEdge), dest(iEdge) );
			T rt = eSeg.t(iPoint);
			return (rt>=0.0) && (rt<=1.0);
		}
		return false;
	}
	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::hasLeftFace( TEdge* e )
	{
//#pragma LASS_TODO("This only works for triangular faces... update to general convex polygonal faces")
//		return ( org(e->lPrev()) == dest(e->lNext()) &&
//				 leftOf(org(e->lPrev()), e));
		return leftOf(dest(e->lNext()), e);
	}
	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::hasRightFace( TEdge* iEdge )
	{
		return hasLeftFace( iEdge->sym() );
	}

	/** chainOrder.  returns the order of the polygonal chain starting from iEdge and
	*	walking around the left-face of iEdge.  Or in other words: the number of vertices
	*	in the polygon on the left of iEdge.
	*/
	TEMPLATE_DEF
	int PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::chainOrder( TEdge* iEdge )
	{
		int order = 0;
		TEdge* currentEdge = iEdge;
		do
		{
			++order;
			currentEdge = currentEdge->lNext();
		} while (currentEdge!=iEdge);
		return order;
	}

	/** allEqualchainOrder.  returns true when the chainorder of all edges which share org(iEdge)
	*	is equal, f.i. when they are all triangles/quadriliterals
	*
	*/
	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::allEqualChainOrder( TEdge* iEdge ) 
	{
		int chOrd = chainOrder(iEdge);
		for (int i=1;i<vertexOrder(iEdge);++i)
		{
			iEdge = iEdge->oNext();
			if (chainOrder(iEdge)!=chOrd)
				return false;
		}
		return true;
	}


	/** vertexOrder.  returns the order of the vertex defined by the origin of iEdge
	*	This is the number of undirected edges connected to this origin.
	*/
	TEMPLATE_DEF
	int PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::vertexOrder( TEdge* iEdge )
	{
		int order = 0;
		TEdge* currentEdge = iEdge;
		do
		{
			++order;
			currentEdge = currentEdge->oNext();
		} while (currentEdge!=iEdge);
		return order;
	}


	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::marking( TEdge* iEdge )
	{
		LASS_ENFORCE( iEdge->handle() );
		return iEdge->handle()->mark_;
	}

	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::internalMarking( TEdge* iEdge )
	{
		LASS_ENFORCE( iEdge->handle() );
		return iEdge->handle()->internalMark_[stackDepth_];
	}


	TEMPLATE_DEF
	PointHandle PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::pointHandle( TEdge* iEdge )
	{
		return inPrimaryMesh( iEdge ) ? iEdge->handle()->pointHandle() : PointHandle();
	}

	TEMPLATE_DEF
	EdgeHandle PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::edgeHandle( TEdge* iEdge )
	{
		return inPrimaryMesh( iEdge ) ? iEdge->handle()->edgeHandle() : EdgeHandle();
	}

	TEMPLATE_DEF
	FaceHandle PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::faceHandle( TEdge* iEdge )
	{
		if ( inPrimaryMesh( iEdge ) )
		{
			ProxyHandle* temp = iEdge->rot()->handle();
			if (temp)
				return temp->faceHandle();
		}
		return FaceHandle();
	}


	TEMPLATE_DEF
	PointHandle& PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::pointHandleRef( TEdge* iEdge )
	{
		if (inPrimaryMesh(iEdge))
			return iEdge->handle()->pointHandle();
		LASS_THROW("no point handle available");
	}

	TEMPLATE_DEF
	EdgeHandle& PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::edgeHandleRef( TEdge* iEdge )
	{
		if (inPrimaryMesh(iEdge))
			return iEdge->handle()->edgeHandle();
		LASS_THROW("no edge handle available");
	}

	TEMPLATE_DEF
	FaceHandle& PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::faceHandleRef( TEdge* iEdge )
	{
		if ( inPrimaryMesh( iEdge ) )
		{
			ProxyHandle* temp = iEdge->rot()->handle();
			if (temp)
				return temp->faceHandle();
		}
		LASS_THROW("no face handle available");
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::setMarking( TEdge* iEdge, bool iMark )
	{
		LASS_ENFORCE( iEdge->handle() );
		iEdge->handle()->mark_ = iMark;
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::setInternalMarking( TEdge* iEdge, bool iMark )
	{
		LASS_ENFORCE( iEdge->handle() );
		iEdge->handle()->internalMark_[stackDepth_] = iMark;
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::setInternalMarkingAroundVertex( TEdge* iEdge, bool iMark )
	{
		TEdge* e = iEdge;
		do
		{
			setInternalMarking( e, iMark );
			e = e->oNext();
		} while (e!=iEdge);
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::setInternalMarkingInFace( TEdge* iEdge, bool iMark )
	{
		TEdge* e = iEdge;
		do
		{
			setInternalMarking( e, iMark );
			e = e->lNext();
		} while (e!=iEdge);
	}


	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::setPointHandle( TEdge* iEdge, PointHandle iHandle )
	{
		if (! inPrimaryMesh( iEdge ) )
			throw std::runtime_error("setPointHandle : edge not in primary mesh");

		TEdge*  currentEdge = iEdge;
		do
		{
			(LASS_ENFORCE(currentEdge->handle()))->pointHandle() = iHandle;
			currentEdge = currentEdge->oNext();
		} while ( currentEdge != iEdge );
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::setEdgeHandle( TEdge* iEdge, EdgeHandle iHandle )
	{
		if (! inPrimaryMesh( iEdge ) )
			throw std::runtime_error("setEdgeHandle : edge not in primary mesh");
		iEdge->handle()->edgeHandle() = iHandle;
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::setFaceHandle( TEdge* iEdge, FaceHandle iHandle )
	{
		if (! inPrimaryMesh( iEdge ) )
			throw std::runtime_error("setFaceHandle : edge not in dual mesh");

		TEdge*  currentEdge = iEdge;
		do
		{
			(LASS_ENFORCE(currentEdge->rot()->handle()))->faceHandle() = iHandle;
			if ( faceHandle( currentEdge ) != faceHandle( currentEdge->sym() ) )
				currentEdge->quadEdge()->faceConstrain();
			else
				currentEdge->quadEdge()->faceDeconstrain();
			currentEdge = currentEdge->lNext();
		} while ( currentEdge != iEdge );
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::setOrientedEdgeHandle( 
			TEdge* iEdge, EdgeHandle iLeftHandle, EdgeHandle iRightHandle, const TVector2D& iOrientation )
	{
#ifndef NDEBUG
		++numSetOrientedEdgeHandleCalls;
#endif
		if (prim::dot(direction(iEdge), iOrientation) < T(0))
		{
#ifndef NDEBUG
			++numSetOrientedEdgeHandleSwaps;
#endif
			std::swap(iLeftHandle, iRightHandle);
		}
		setEdgeHandle(iEdge, iLeftHandle);
		setEdgeHandle(iEdge->sym(), iRightHandle);
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::forAllPrimaryEdges( const TEdgeCallback& iCallback )
	{
		typename TQuadEdgeList::iterator qIt;
		for (qIt = quadEdgeList_.begin(); qIt != quadEdgeList_.end();++qIt)
		{
			if (inPrimaryMesh( (*qIt)->edges() ) )
			{
				if (!iCallback( (*qIt)->edges() )) return;
				if (!iCallback( (*qIt)->edges()->sym() )) return;
			}
			else
			{
				if (!iCallback( (*qIt)->edges()->rot() )) return ;
				if (!iCallback( (*qIt)->edges()->invRot() )) return;
			}
		}
	}
	/** forAllPrimaryUndirectedEdges.  Edges sharing endpoints are only listed once */
	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::forAllPrimaryUndirectedEdges( const TEdgeCallback& iCallback )
	{
		typename TQuadEdgeList::iterator qIt;
		for (qIt = quadEdgeList_.begin(); qIt != quadEdgeList_.end();++qIt)
		{
			if (inPrimaryMesh( (*qIt)->edges() ) )
			{
				if (!iCallback( (*qIt)->edges() )) return;
			}
			else
			{
				if (!iCallback( (*qIt)->edges()->rot() )) return ;
			}
		}
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::forAllDualEdges( const TEdgeCallback& iCallback )
	{
		typename TQuadEdgeList::iterator qIt;
		for (qIt = quadEdgeList_.begin(); qIt != quadEdgeList_.end();++qIt)
		{
			if (inDualMesh( (*qIt)->edges() ) )
			{
				if (!iCallback( (*qIt)->edges() )) return;
				if (!iCallback( (*qIt)->edges()->sym() )) return;
			}
			else
			{
				if (!iCallback( (*qIt)->edges()->rot() )) return;
				if (!iCallback( (*qIt)->edges()->invRot() )) return;
			}
		}
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::forAllEdges( const TEdgeCallback& iCallback )
	{
		typename TQuadEdgeList::iterator qIt;
		for (qIt = quadEdgeList_.begin(); qIt != quadEdgeList_.end();++qIt)
		{
			if (!iCallback( (*qIt)->edges() )) return;
			if (!iCallback( (*qIt)->edges()->sym() )) return;
			if (!iCallback( (*qIt)->edges()->rot() )) return;
			if (!iCallback( (*qIt)->edges()->invRot() )) return;
		}
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::forAllVertices( const TEdgeCallback& iCallback )
	{
		typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
		StackIncrementer( &stackDepth_, PLANAR_MESH_STACK_DEPTH );
		typedef impl::EdgeMarker<T, PointHandle, EdgeHandle, FaceHandle> TEdgeMarker;
		TEdgeMarker edgeMarker( this, false );
		forAllPrimaryEdges( TEdgeCallback( &edgeMarker, &TEdgeMarker::internalMark ) );

		typename TQuadEdgeList::iterator qIt;
		for (qIt = quadEdgeList_.begin(); qIt != quadEdgeList_.end();++qIt)
		{
			TEdge* baseEdge = (*qIt)->edges();
			if (inDualMesh( baseEdge ) )
				baseEdge = baseEdge->rot();

			TEdge* e = baseEdge;
			for (int i=0;i<2;++i)
			{
				if ( !internalMarking( e ) )
				{
					if (!iCallback( e ))
						return;
					setInternalMarkingAroundVertex( e, true );
				}
				e = e->sym();
			}
		}
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::forAllFaces( const TEdgeCallback& iCallback )
	{
		StackIncrementer( &stackDepth_, PLANAR_MESH_STACK_DEPTH );
		typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
		typedef impl::EdgeMarker<T, PointHandle, EdgeHandle, FaceHandle> TEdgeMarker;
		TEdgeMarker edgeMarker( this, false );
		forAllPrimaryEdges( TEdgeCallback( &edgeMarker, &TEdgeMarker::internalMark ) );

		typename TQuadEdgeList::iterator qIt;
		for (qIt = quadEdgeList_.begin(); qIt != quadEdgeList_.end();++qIt)
		{
			TEdge* baseEdge = (*qIt)->edges();
			if (inDualMesh( baseEdge ) )
				baseEdge = baseEdge->rot();

			TEdge* e = baseEdge;
			for (int i=0;i<2;++i)
			{
				if ( !internalMarking( e ) )
				{
					if (!iCallback( e ))
						return;
					setInternalMarkingInFace( e, true );
				}
				e = e->sym();
			}
		}
	}

#ifndef NDEBUG
	TEMPLATE_DEF unsigned PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::numSetOrientedEdgeHandleCalls = 0;
	TEMPLATE_DEF unsigned PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::numSetOrientedEdgeHandleSwaps = 0;
#endif


}

}

#endif
