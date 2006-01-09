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
#include "../prim/side.h"
#include "../util/callback_r_1.h"
#include "../util/small_object.h"
#include "../io/matlab_o_stream.h"


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
	}

	TEMPLATE_DEF
	class PlanarMesh
	{
	public:
		typedef lass::prim::Point2D<T> TPoint2D;
		typedef lass::prim::Vector2D<T> TVector2D;
		typedef lass::prim::Line2D<T> TLine2D;
		typedef lass::prim::Ray2D<T, prim::Unnormalized, prim::Unbounded > TRay2D;
		typedef lass::prim::LineSegment2D<T> TLineSegment2D;

		static const int PLANAR_MESH_STACK_DEPTH = 8;   /**< this determines the maximum nesting depth of forAllVertices and forAllFaces */
	private:
		struct ProxyHandle : public lass::util::SmallObject<16384,64,unsigned char>
		{
			TPoint2D*       point_;
			PointHandle*    pointHandle_;
			EdgeHandle*     edgeHandle_;
			FaceHandle*     faceHandle_;
			bool            mark_;
			std::vector<bool>   internalMark_;

			ProxyHandle() : point_(NULL), pointHandle_(NULL), edgeHandle_(NULL), faceHandle_(NULL), mark_(false) { internalMark_ = std::vector<bool>(PLANAR_MESH_STACK_DEPTH,false); }
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
		virtual ~PlanarMesh();

		TEdge*  startEdge() const;

		void    forAllPrimaryEdges( const TEdgeCallback& iCallback );
		void    forAllPrimaryUndirectedEdges( const TEdgeCallback& iCallback );
		void    forAllDualEdges( const TEdgeCallback& iCallback );
		void    forAllEdges( const TEdgeCallback& iCallback );
		void    forAllVertices( const TEdgeCallback& iCallback );
		void    forAllFaces( const TEdgeCallback& iCallback );

		TEdge*  locate( const TPoint2D& iPoint ) const;
		TEdge*  insertSite( const TPoint2D& iPoint, bool makeDelaunay = true);
		TEdge*  insertEdge( const TLineSegment2D& iSegment, bool makeDelaunay = true);
		bool    deleteEdge( TEdge* iEdge );
		long    edgeCount() const;
		void    makeMaximalConvexPolygon();

		static  const TPoint2D& org( TEdge* iEdge );
		static  const TPoint2D& dest( TEdge* iEdge );
		static  bool  rightOf( const TPoint2D& iPoint, TEdge* iEdge );
		static  bool  leftOf( const TPoint2D& iPoint, TEdge* iEdge );
		static  bool  onEdge( const TPoint2D& iPoint, TEdge* iEdge );
		static  bool  hasLeftFace( TEdge* iEdge );
		static  bool  hasRightFace( TEdge* iEdge );
		static  int   chainOrder( TEdge* iEdge );
		static  int   vertexOrder( TEdge* iEdge );

		static  bool inPrimaryMesh( TEdge* iEdge );
		static  bool inDualMesh( TEdge* iEdge );
		static  bool marking( TEdge* iEdge );
		static  void setMarking( TEdge* iEdge, bool iMark );
		static  PointHandle*    pointHandle( TEdge* iEdge );
		static  EdgeHandle*     edgeHandle( TEdge* iEdge );
		static  FaceHandle*     faceHandle( TEdge* iEdge );
		static  void setPointHandle( TEdge* iEdge, PointHandle* iHandle );
		static  void setEdgeHandle( TEdge* iEdge, EdgeHandle* iHandle );
		static  void setFaceHandle( TEdge* iEdge, FaceHandle* iHandle );
	private:
		TEdge*  startEdge_;
		TQuadEdgeList       quadEdgeList_;

		mutable TEdge*  lastLocateEdge_;
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
		TPoint2D snap(const TPoint2D& a, const TPoint2D& b, const TPoint2D& c);

		TEdge*  bruteForceLocate( const TPoint2D& iPoint ) const;

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
	};

	namespace impl
	{
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
			lass::io::MatlabOStream& stream_;
			TPlanarMesh* mesh_;
		public:
			EdgeToMatlab( TPlanarMesh* iMesh, lass::io::MatlabOStream& ioOStream ) : mesh_(iMesh), stream_( ioOStream ) {}
			bool edgeToMatlab( typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* iEdge )
			{
				if (!mesh_->internalMarking( iEdge ))
					stream_ << typename TPlanarMesh::TLineSegment2D( 
						TPlanarMesh::org(iEdge), TPlanarMesh::dest(iEdge) ) 
						<< std::endl;
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
					if (lastArea_ == 0)
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
		class BrutePointLocator
		{
			typedef PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle> TPlanarMesh;
			typename TPlanarMesh::TPoint2D  point_;
		public:
			typename TPlanarMesh::TEdge*    edge;
			BrutePointLocator( const typename TPlanarMesh::TPoint2D& iPoint ) : point_(iPoint), edge(NULL) {}
			bool findEdge( typename TPlanarMesh::TEdge* e )
			{
				if (    ( prim::doubleTriangleArea( TPlanarMesh::org(e), TPlanarMesh::dest(e), TPlanarMesh::org(e->lNext()) ) == 0 )
					&&  ( TPlanarMesh::leftOf( point_, e ) ) )
				{
					edge = e;
					return false;
				}
				typename TPlanarMesh::TEdge* currentEdge = e;
				do
				{
					if (!prim::ccw( TPlanarMesh::org(e), TPlanarMesh::dest(e), TPlanarMesh::org(e->lNext()) ))
						return true;
				} while (currentEdge!=e);
				edge = e;
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
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::init4( const TPoint2D& a, const TPoint2D& b, const TPoint2D& c, const TPoint2D& d)
	{
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
			LASS_ENFORCE(currentEdge->handle())->point_ = NULL;
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
		if ( faceHandle( a ) != faceHandle( b ) )
		{
			throw std::runtime_error("connect of edges would violate face constraint");
		}
		PointHandle* hADest = pointHandle( a->sym() );
		PointHandle* hB = pointHandle( b );
		TEdge* e = makeEdge( dest(a), org(b), false);
		TQuadEdge::splice( e, a->lNext() );
		TQuadEdge::splice( e->sym(), b );

		setFaceHandle( e, faceHandle( a ) );
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
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::splitEdge(TEdge *e, const TPoint2D& iPoint )
	// Shorten edge e s.t. its destination becomes x. Connect
	// x to the previous destination of e by a new edge. If e
	// is constrained, x is snapped onto the edge, and the new
	// edge is also marked as constrained.
	{
		PointHandle*    iE = pointHandle(e);
		PointHandle*    iES= pointHandle(e->sym());

		TPoint2D thePoint = iPoint;
		if (e->isConstrained())
			thePoint = snap( iPoint, org(e), dest(e));
		TEdge* t = e->lNext();
		TQuadEdge::splice( e->sym(), t );
		setDest( thePoint, e );
		setPointHandle(e, iE);
		TEdge* ne = connect( e, t );
		if (e->isConstrained())
			ne->quadEdge()->edgeConstrain();

		// make sure the inserted edge also points to correct point information
		LASS_ASSERT( pointHandle(e) == iE );
		LASS_ASSERT( pointHandle(e->lNext()->sym()) == iES );
	}

	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TPoint2D PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::snap(const TPoint2D& x, const TPoint2D& a, const TPoint2D& b)
	{
		if (x == a)
			return a;
		if (x == b)
			return b;
		T t1 = dot(x-a,b-a);
		T t2 = dot(x-b,a-b);

		T t = std::max(t1,t2) / (t1 + t2);

		if (t1>t2)
		{
			T rx = (T(1)-t)*a.x + t*b.x;
			T ry = (T(1)-t)*a.y + t*b.y;
			return TPoint2D(rx,ry);
		}

		T rx = (T(1)-t)*b.x + t*a.x;
		T ry = (T(1)-t)*b.y + t*a.y;
		return TPoint2D(rx,ry);
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
		PointHandle* hA = pointHandle(e->oNext()->sym());
		PointHandle* hB = pointHandle(e);
		PointHandle* hC = pointHandle(e->sym());
		PointHandle* hD = pointHandle(e->oPrev()->sym());

		TQuadEdge::splice( e, a );
		TQuadEdge::splice( e->sym(), b );
		TQuadEdge::splice( e, a->lNext() );
		TQuadEdge::splice( e->sym(), b->lNext() );
		setOrg( dest(a), e );       // this is point a here!
		setDest( dest(b), e );      // this is point b here!

		setPointHandle( e, hD );
		setPointHandle( e->sym(), hA );

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

		if (startEdge_==e)
			startEdge_=e->lNext();
		if (lastLocateEdge_==e)
			lastLocateEdge_ = startEdge_;

		TQuadEdge::splice( e, e->oPrev() );
		TQuadEdge::splice( e->sym(), e->sym()->oPrev() );

		TQuadEdgeList::iterator it = std::find(quadEdgeList_.begin(),quadEdgeList_.end(),e->quadEdge());
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
		return bruteLocator.edge;
	}



	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::locate( const TPoint2D& iPoint ) const
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

				TRay2D  R1(org(e), dest(e));
				TPoint2D    p1 = R1.point( R1.t( iPoint ) );
				TRay2D  R2(dest(e), org(e->lPrev()));
				TPoint2D    p2 = R2.point( R2.t( iPoint ) );
				TRay2D  R3(org(e->lPrev()), org(e));
				TPoint2D    p3 = R3.point( R3.t( iPoint ) );

				typename TPoint2D::TValue d1 = squaredDistance(iPoint,p1);
				typename TPoint2D::TValue d2 = squaredDistance(iPoint,p2);
				typename TPoint2D::TValue d3 = squaredDistance(iPoint,p3);

				int s = 0;
				if ((d1<d2) && (d1<d3))
				{
					return e;
				}
				if ((d2<d3) && (d2<d1))
				{
					return e->lNext();
				}
				return e->lPrev();
			}

			if (onEdge(iPoint,e))
				return e->sym();

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


	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::insertSite( const TPoint2D& iPoint, bool makeDelaunay )
	{
		TEdge* e = locate( iPoint );
		if ( org(e) == iPoint )
			return e;
		if ( dest(e) == iPoint )
			return e->sym();

		bool hasLeft = hasLeftFace(e);
		bool hasRight = hasRightFace(e);

		if (!hasLeft && !hasRight)
		{
			lass::io::MatlabOStream bugMesh;
			bugMesh.open( "bugMesh.m" );
			bugMesh << *this;
			bugMesh.close();
			throw std::runtime_error("insertSite: edge does not have any faces");
		}

		bool isOnEdge = onEdge(iPoint, e);
		bool insideLeft  = hasLeft && (isOnEdge || leftOf(iPoint, e)) &&
						   rightOf(iPoint, e->oNext()) && rightOf(iPoint, e->dPrev());
		bool insideRight = hasRight && (isOnEdge || rightOf(iPoint, e)) &&
						   leftOf(iPoint, e->oPrev()) && leftOf(iPoint, e->dNext());

		if (insideLeft  && iPoint == dest(e->oNext()))
			return e->lPrev();
		if (insideRight && iPoint == dest(e->oPrev()))
			return e->dNext();

		if ( isOnEdge )
		{
			// snap x to e, and check for coincidence:
			TPoint2D x = snap(iPoint, org(e), dest(e));
			if (x == org(e))
				return e;
			if (x == dest(e))
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
				splitEdge(e, iPoint);
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
				splitEdge(e, iPoint);
				connect(e, e->lPrev());
				fixEdge(c);
				fixEdge(d);
			}
			return e->sym();
		}

		int chOrder = chainOrder(e);

		PointHandle* pH = pointHandle(e);
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
		return base->sym();
	}


	TEMPLATE_DEF
	typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TEdge* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::insertEdge( const TLineSegment2D& iSegment, bool makeDelaunay )
	{
		TEdge *ea, *eb;
		TPoint2D aa, bb;
		if (ea = insertSite(iSegment.tail()),makeDelaunay)
			aa = org(ea);
		if (eb = insertSite(iSegment.head()),makeDelaunay)
			bb = org(eb);

		if (ea == NULL || eb == NULL)
			throw std::runtime_error("insertEdge: could not insert endpoints of edge");

		if (org(ea)!=aa)
		{
			ea=locate(aa);
			if (ea==NULL)
				throw std::runtime_error("insertEdge: could not locate an endpoint");

			if (!(aa == org(ea)))
				ea = ea->sym();
			if (!(aa == org(ea)))
				throw std::runtime_error("insertEdge: point a is not an endpoint of ea");
		}

		if (aa == bb)
			throw std::runtime_error("insertEdge: both ends map to same vertex");

		// aa and bb are vertices in the mesh; our goal is to connect
		// them by a sequence of one or more edges.

		TLine2D ab(aa,bb);
		TVector2D dd=bb-aa;
		TEdge *t,*ne;

		while (org(ea)!=iSegment.head())
		{
			t = ea;
			do
			{
				if ( ab.classify(dest(ea))==prim::sSurface && dot(dest(ea)-aa,dd)>0)
					break;
				if ( ab.classify(dest(ea->oNext()))==prim::sSurface
					&&  dot(dest(ea->oNext())-aa,dd)>0 )
				{
					ea = ea->oNext();
					break;
				}
				if (!cw(dest(ea),bb,aa) && cw(dest(ea->oNext()),bb,aa))
					break;
				ea = ea->oNext();
				if (ea==t)
					throw std::runtime_error("insertEdge: infinite loop");
			} while (true);

			// check to see if an edge is already there:
			if ( ab.classify(dest(ea))==prim::sSurface)
			{
				ea->quadEdge()->edgeConstrain();
				aa = dest(ea);
				if (aa==bb)
#pragma LASS_TODO("Check if returned edge doesn't violate postconditions")
					return ea;
				ab = TLine2D(aa,bb);
				dd = bb-aa;
				ea = ea->sym()->oNext();
				continue;
			}
			t = ea;
			while (true)
			{

				if (ab.classify(dest(t->lNext()))==prim::sSurface)
				{
#pragma LASS_TODO("does this only work for triangles?!")
					if (ea==t->lNext()->lNext()->lNext())
					{
						// edge is already there
						t->lNext()->lNext()->quadEdge()->edgeConstrain();
						ea = t->lNext()->sym();
						break;
					}
					else
					{
						// need a new edge
						ne = connect(t->lNext(), ea);
						ne->quadEdge()->edgeConstrain();
						ea = t->lNext()->sym();
						triangulate(ne->lNext());
						triangulate(ne->oPrev());
						break;
					}
				}

				if ( ccw(aa,bb, dest(t->lNext())) )
				{
					if (!t->lNext()->isConstrained())
						deleteEdge(t->lNext());
					else
					{
						TPoint2D x;
						TLine2D other(org(t->lNext()),dest(t->lNext()));
						if (prim::intersect(ab,other,x)!=prim::rOne)
							throw std::runtime_error("insertEdge: could not find intersection");
						splitEdge(t->lNext(),x);
						ne = connect(t->lNext(), ea);
						ne->quadEdge()->edgeConstrain();

						ea = t->lNext()->sym();
						triangulate(ne->lNext());
						triangulate(ne->oPrev());
						break;
					}
				}
				else
				{
					t = t->lNext();
				}
			}
		}

		return ea;
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
	const typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TPoint2D& PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::org( TEdge* iEdge )
	{
		if (!inPrimaryMesh( iEdge ))
			throw std::runtime_error("PlanarMesh::org: edge not in primary mesh");
		return *iEdge->handle()->point_;
	}

	TEMPLATE_DEF
	const typename PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::TPoint2D& PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::dest( TEdge* iEdge )
	{
		return org( iEdge->sym() );
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
		TRay2D eSeg( org(iEdge), dest(iEdge) );
		typename TRay2D::TValue t = eSeg.t( iPoint );
		if ( eSeg.point(t) == iPoint )
			return (t>=0.0) && (t<=1.0);
		else
			return false;
	}
	TEMPLATE_DEF
	bool PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::hasLeftFace( TEdge* e )
	{
		return ( org(e->lPrev()) == dest(e->lNext()) &&
				 leftOf(org(e->lPrev()), e));
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
	PointHandle* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::pointHandle( TEdge* iEdge )
	{
		return inPrimaryMesh( iEdge ) ? iEdge->handle()->pointHandle_ : NULL;
	}

	TEMPLATE_DEF
	EdgeHandle* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::edgeHandle( TEdge* iEdge )
	{
		return inPrimaryMesh( iEdge ) ? iEdge->handle()->edgeHandle_ : NULL;
	}

	TEMPLATE_DEF
	FaceHandle* PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::faceHandle( TEdge* iEdge )
	{
		if ( inPrimaryMesh( iEdge ) )
		{
			ProxyHandle* temp = iEdge->rot()->handle();
			if (temp)
				return temp->faceHandle_;
		}
		return NULL;
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
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::setPointHandle( TEdge* iEdge, PointHandle* iHandle )
	{
		if (! inPrimaryMesh( iEdge ) )
			throw std::runtime_error("setPointHandle : edge not in primary mesh");

		TEdge*  currentEdge = iEdge;
		do
		{
			LASS_ENFORCE(currentEdge->handle())->pointHandle_ = iHandle;
			currentEdge = currentEdge->oNext();
		} while ( currentEdge != iEdge );
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::setEdgeHandle( TEdge* iEdge, EdgeHandle* iHandle )
	{
		if (! inPrimaryMesh( iEdge ) )
			throw std::runtime_error("setEdgeHandle : edge not in primary mesh");

		iEdge->handle()->edgeHandle_ = iHandle;
	}

	TEMPLATE_DEF
	void PlanarMesh<T, PointHandle, EdgeHandle, FaceHandle>::setFaceHandle( TEdge* iEdge, FaceHandle* iHandle )
	{
		if (! inPrimaryMesh( iEdge ) )
			throw std::runtime_error("setFaceHandle : edge not in dual mesh");

		TEdge*  currentEdge = iEdge;
		do
		{
			LASS_ENFORCE(currentEdge->rot()->handle())->faceHandle_ = iHandle;
			if ( faceHandle( currentEdge ) != faceHandle( currentEdge->sym() ) )
				currentEdge->quadEdge()->faceConstrain();
			else
				currentEdge->quadEdge()->faceDeconstrain();
			currentEdge = currentEdge->lNext();
		} while ( currentEdge != iEdge );
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

}

}

#endif
