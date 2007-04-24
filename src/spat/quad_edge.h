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



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_QUAD_EDGE_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_QUAD_EDGE_H

#include "spat_common.h"

namespace lass
{
namespace spat
{
	template< typename EdgeHandle >
	class QuadEdge
	{
	public:
		class Edge
		{
			friend class QuadEdge<EdgeHandle>;
		public:
			Edge() : next_(NULL) {}

			Edge* rot()   { return (index_ < 3) ? this + 1 : this - 3; }
			Edge* invRot(){ return (index_ > 0) ? this - 1 : this + 3; }
			Edge* sym()   { return (index_ < 2) ? this + 2 : this - 2; }
			Edge* oNext() { return next_; }
			Edge* oPrev() { return rot()->oNext()->rot(); }
			Edge* dNext() { return sym()->oNext()->sym(); }
			Edge* dPrev() { return invRot()->oNext()->invRot(); }
			Edge* lNext() { return invRot()->oNext()->rot(); }
			Edge* lPrev() { return oNext()->sym(); }
			Edge* rNext() { return rot()->oNext()->invRot(); }
			Edge* rPrev() { return sym()->oNext(); }

			QuadEdge* quadEdge() const { return (QuadEdge*)(this - index_); }
			EdgeHandle* const handle() { return &edgeHandle_;}
			const EdgeHandle* const handle() const { return &edgeHandle_; }
			bool isConstrained() const { return quadEdge()->isConstrained(); }
			bool isEdgeConstrained() const { return quadEdge()->isEdgeConstrained(); }
			bool isFaceConstrained() const { return quadEdge()->isFaceConstrained(); }

			int index() const { return index_; }
		private:

			Edge* next_;
			EdgeHandle edgeHandle_;
			int index_;
		};

	public:
		QuadEdge(bool makeConstrained=false);
		QuadEdge(const QuadEdge& other);
		~QuadEdge();
		QuadEdge& operator=(const QuadEdge& other);

		void    edgeConstrain();
		void    edgeDeconstrain();
		void    faceConstrain();
		void    faceDeconstrain();
		bool    isConstrained() const;
		bool    isEdgeConstrained() const;
		bool    isFaceConstrained() const;
		Edge*   edges();

		static void splice( Edge* a, Edge* b);

	private:
		// don't touch the position of the edges_, they must be the first data type in the POD or else you will be crucified!

		void initEdges();
		void copyEdges(const QuadEdge& other);

		Edge    edges_[4];
		bool    edgeConstrained_;       /**< the edge is forced into the mesh, stay off! */
		bool    faceConstrained_;       /**< the faces adjacent the edge have their handles set differently, cannot do stuff with the edge! */

	};


	template< typename EdgeHandle > QuadEdge<EdgeHandle>::QuadEdge(bool makeEdgeConstrained)
	{
		edgeConstrained_ = makeEdgeConstrained;
		faceConstrained_ = false;
		initEdges();
	}

	template< typename EdgeHandle > QuadEdge<EdgeHandle>::QuadEdge(const QuadEdge& other):
		edgeConstrained_(other.edgeConstrained_),
		faceConstrained_(other.faceConstrained_)
	{
		initEdges();
		copyEdges(other);
	}

	template< typename EdgeHandle > QuadEdge<EdgeHandle>& QuadEdge<EdgeHandle>::operator=(const QuadEdge& other)
	{
		copyEdges(other);
		return *this;
	}

	template< typename EdgeHandle > QuadEdge<EdgeHandle>::~QuadEdge()
	{
		LASS_ASSERT( !faceConstrained_ );
		// detach edge from the subdivision:
		splice(edges_, edges_->oPrev());
		splice(edges_->sym(), edges_->sym()->oPrev());
	}

	template< typename EdgeHandle > void QuadEdge<EdgeHandle>::splice( Edge* a, Edge* b )
	{
		Edge* alpha = a->oNext()->rot();
		Edge* beta  = b->oNext()->rot();

		Edge* t1 = b->oNext();
		Edge* t2 = a->oNext();
		Edge* t3 = beta->oNext();
		Edge* t4 = alpha->oNext();

		a->next_ = t1;
		b->next_ = t2;
		alpha->next_ = t3;
		beta->next_ = t4;
	}

	template< typename EdgeHandle > void QuadEdge<EdgeHandle>::edgeConstrain()
	{
		edgeConstrained_ = true;
	}
	template< typename EdgeHandle > void QuadEdge<EdgeHandle>::edgeDeconstrain()
	{
		edgeConstrained_ = false;
	}
	template< typename EdgeHandle > void QuadEdge<EdgeHandle>::faceConstrain()
	{
		faceConstrained_ = true;
	}
	template< typename EdgeHandle > void QuadEdge<EdgeHandle>::faceDeconstrain()
	{
		faceConstrained_ = false;
	}
	template< typename EdgeHandle > bool QuadEdge<EdgeHandle>::isConstrained() const
	{
		return edgeConstrained_ || faceConstrained_;
	}
	template< typename EdgeHandle > bool QuadEdge<EdgeHandle>::isEdgeConstrained() const
	{
		return edgeConstrained_;
	}
	template< typename EdgeHandle > bool QuadEdge<EdgeHandle>::isFaceConstrained() const
	{
		return faceConstrained_;
	}
	template< typename EdgeHandle > typename QuadEdge<EdgeHandle>::Edge* QuadEdge<EdgeHandle>::edges()
	{
		return edges_;
	}
	/*
	template< typename EdgeHandle > QuadEdge<EdgeHandle>* QuadEdge<EdgeHandle>::Edge::quadEdge()
	{
		return this - index_;
	}
	template< typename EdgeHandle > QuadEdge<EdgeHandle>::Edge* QuadEdge<EdgeHandle>::Edge::rot()
	{
		return (index_ < 3) ? this + 1 : this - 3;
	}
	template< typename EdgeHandle > QuadEdge<EdgeHandle>::Edge* QuadEdge<EdgeHandle>::Edge::invRot()
	{
		return (index_ > 0) ? this - 1 : this + 3;
	}
	template< typename EdgeHandle > QuadEdge<EdgeHandle>::Edge* QuadEdge<EdgeHandle>::Edge::sym()
	{
		return (index_ < 2) ? this + 2 : this - 2;
	}
	template< typename EdgeHandle > QuadEdge<EdgeHandle>::Edge* QuadEdge<EdgeHandle>::Edge::oNext()
	{
		return next_;
	}
	template< typename EdgeHandle > QuadEdge<EdgeHandle>::Edge* QuadEdge<EdgeHandle>::Edge::oPrev()
	{
		return rot()->oNext()->rot();
	}
	template< typename EdgeHandle > QuadEdge<EdgeHandle>::Edge* QuadEdge<EdgeHandle>::Edge::dNext()
	{
		return sym()->oNext()->sym();
	}
	template< typename EdgeHandle > QuadEdge<EdgeHandle>::Edge* QuadEdge<EdgeHandle>::Edge::dPrev()
	{
		return invRot()->oNext()->invRot();
	}
	template< typename EdgeHandle > QuadEdge<EdgeHandle>::Edge* QuadEdge<EdgeHandle>::Edge::lNext()
	{
		return invRot()->oNext()->rot();
	}
	template< typename EdgeHandle > QuadEdge<EdgeHandle>::Edge* QuadEdge<EdgeHandle>::Edge::lPrev()
	{
		return oNext()->sym();
	}
	template< typename EdgeHandle > QuadEdge<EdgeHandle>::Edge* QuadEdge<EdgeHandle>::Edge::rNext()
	{
		return rot()->oNext()->invRot();
	}
	template< typename EdgeHandle > QuadEdge<EdgeHandle>::Edge* QuadEdge<EdgeHandle>::Edge::rPrev()
	{
		return sym()->oNext();
	}
	*/

	template< typename EdgeHandle > void QuadEdge<EdgeHandle>::initEdges()
	{
		for (int i=0;i<4;++i)
			edges_[i].index_ = i;
		edges_[0].next_ = &edges_[0];
		edges_[1].next_ = &edges_[3];
		edges_[2].next_ = &edges_[2];
		edges_[3].next_ = &edges_[1];
	}

	template< typename EdgeHandle > void QuadEdge<EdgeHandle>::copyEdges(const QuadEdge& other)
	{
		for (int i=0;i<4;++i)
			edges_[i].edgeHandle_ = other.edges_[i].edgeHandle_;
	}
}

}

#endif
