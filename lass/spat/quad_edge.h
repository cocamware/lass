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
		typedef EdgeHandle TEdgeHandle;

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

			const Edge* rot() const   { return (index_ < 3) ? this + 1 : this - 3; }
			const Edge* invRot() const{ return (index_ > 0) ? this - 1 : this + 3; }
			const Edge* sym() const   { return (index_ < 2) ? this + 2 : this - 2; }
			const Edge* oNext() const { return next_; }
			const Edge* oPrev() const { return rot()->oNext()->rot(); }
			const Edge* dNext() const { return sym()->oNext()->sym(); }
			const Edge* dPrev() const { return invRot()->oNext()->invRot(); }
			const Edge* lNext() const { return invRot()->oNext()->rot(); }
			const Edge* lPrev() const { return oNext()->sym(); }
			const Edge* rNext() const { return rot()->oNext()->invRot(); }
			const Edge* rPrev() const { return sym()->oNext(); }

			const QuadEdge* quadEdge() const { return (QuadEdge*)(this - index_); }
			QuadEdge* quadEdge() { return (QuadEdge*)(this - index_); }
			const EdgeHandle& handle() const { return edgeHandle_;}
			EdgeHandle& handle() { return edgeHandle_; }
			bool isConstrained() const { return quadEdge()->isConstrained(); }
			bool isEdgeConstrained() const { return quadEdge()->isEdgeConstrained(); }
			bool isFaceConstrained() const { return quadEdge()->isFaceConstrained(); }

			int index() const { return index_; }
		private:

			EdgeHandle edgeHandle_;
			Edge* next_;
			int index_;
		};

	public:
		QuadEdge(bool makeConstrained=false);
		QuadEdge(const QuadEdge& other);
		~QuadEdge();
		void detach();
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
	public:
		bool	deleted;
	};


	template< typename EdgeHandle > QuadEdge<EdgeHandle>::QuadEdge(bool makeEdgeConstrained)
	{
		edgeConstrained_ = makeEdgeConstrained;
		faceConstrained_ = false;
		deleted = false;
		initEdges();
	}

	template< typename EdgeHandle > QuadEdge<EdgeHandle>::QuadEdge(const QuadEdge& other):
		edgeConstrained_(other.edgeConstrained_),
		faceConstrained_(other.faceConstrained_),
		deleted(other.deleted)
	{
		initEdges();
		copyEdges(other);
	}

	template< typename EdgeHandle > QuadEdge<EdgeHandle>& QuadEdge<EdgeHandle>::operator=(const QuadEdge& other)
	{
		deleted = other.deleted;
		copyEdges(other);
		return *this;
	}

	template< typename EdgeHandle > QuadEdge<EdgeHandle>::~QuadEdge()
	{
		LASS_ASSERT( !faceConstrained_ );
		// detach edge from the subdivision:
		//detach();
	}
	template< typename EdgeHandle > void QuadEdge<EdgeHandle>::detach()
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
