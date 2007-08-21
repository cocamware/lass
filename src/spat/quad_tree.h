/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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



/** @class lass::spat::QuadTree
 *  A spatial container for generic objects.  The object needs a traits class which
 *  contains the necessary functions to perform the quad tree management for the
 *  particular ObjectType.  The traits class needs as a basis the following interface:
 *  <tt>
 *      static TAabb aabb(const TSimplePolygon3D& iP);
 *      static bool contains( const TSimplePolygon3D& iP, const TPoint& iPoint)
 *  </tt>
 *  The above functions are only examples.  The dimensionality of the primitives must
 *  match but can be of any order.  So the quad tree can be used to classify in
 *  2 and 3 dimensions.  In three dimensions the more common name is OctTree.
 *
 *  Higher level divisions can in theory be supported but the dimensional specific
 *  part must be reimplemented.  Altough this is only 2 functions and could be written
 *  generally this is not yet available.
 *
 *  @brief a Quad tree for general objects
 *  @author Tom De Muer [TDM]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_QUAD_TREE_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_QUAD_TREE_H

#include "spat_common.h"

namespace lass
{
namespace spat
{

namespace impl
{
	template<
		int dimension,
		class ObjectType,
		template <class> class ObjectTraits
	>
	struct
	TreeSpatializer;
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
class QuadTree
{
public:
	typedef ObjectType TObjectType;
	typedef ObjectTraits<ObjectType> TObjectTraits;

	typedef typename ObjectTraits<ObjectType>::TPoint TPoint;
	typedef typename ObjectTraits<ObjectType>::TPoint::TNumTraits::selfType TBaseType;
	typedef typename ObjectTraits<ObjectType>::TAabb TAabb;
	typedef typename ObjectTraits<ObjectType>::TSeparator TSeparator;

	typedef typename TPoint::TVector TVector;
	typedef typename TPoint::TValue TValue;

private:
	enum { dimension = TPoint::dimension };
	static const int subNodeCount=1<<dimension;
	typedef std::list< ObjectType > TListType;
	struct QuadNode
	{
		QuadNode *node[subNodeCount];   /**< 0 = NW, 1 = NE, 2 = SE, 3 = SW for quadtrees*/
		TPoint center;                  /**< center of quadnode */
		TVector extents;                /**< x = half of widht, y = half of height */
		TListType data;                 /**< the list containing the data */
		int listSize;                   /**< helping the std::list :) */
		int level;                      /**< level in tree */
		bool leaf;                      /**< true for leaf nodes */

		QuadNode();
		QuadNode(const TPoint& aCenter, const TVector& aExtents);
		~QuadNode();

		QuadNode* add(const TObjectType& an,int iMaxSize = 100,int iMaxLevel = 10);

		int objectCount() const;            /**< number of objects in this node and all its children */
		int maxDepth() const;               /**< maximum depth of the child tree */
		void decompose();                   /**< split the current node into 4 children */
		void absorb();                      /**< absorb all children into the current node */

		TAabb aabb() const;
		int contains(const TPoint& ap, std::vector<ObjectType*>& oObjects) const;
	};
	friend struct impl::TreeSpatializer<dimension, ObjectType, ObjectTraits>;

	QuadNode*   root_;
	QuadTree();
public:
	/** constructor.
	*  @param iCenter       the center of the spatial tree
	*  @param iExtents      the extents of the spatial tree in each direction,
	*                       thus the total width of the tree is twice the extents
	*/
	QuadTree(const TPoint& iCenter, const TVector& iExtents);
	/** constructor.
	*  @param iBox          the bounding box of the spatial tree
	*/
	QuadTree(const TAabb& iBox);
	~QuadTree();

	/** contains.  Returns the number of object that returned true on contains and _adds_ them
	*   to the vector oObjects.
	*
	* Required :
	* <tt>static bool ObjectTypeTraits::contains( const Object& iObject, const TPoint& iPoint );</tt>
	*/
	int contains( const TPoint& iPoint, std::vector<ObjectType*>& oObjects ) const;

	/** contains.  Returns the number of object that returned sFront on all the lines
	*   provided in the iFrustum vector and _adds_ them to the vector oObjects.  An example
	*   of use is frustum culling.
	*
	* Required :
	*  <tt>static bool ObjectTypeTraits::contains( const Object& iObject, const TPoint& iPoint );</tt>
	*/
	int visible( const std::vector<TSeparator>& iFrustum, std::vector<ObjectType*>& oObjects ) const;

	/** clear.  Clears the entire quadtree. */
	void clear();
	void add(const ObjectType& iObject,int iMaxSize = 100, int iMaxLevel = 10);
	int objectCount() const;

	/** maxDepth. Returns the maximum depth of the tree */
	int maxDepth() const;
};


namespace impl
{
	template<
		int dimension,
		class ObjectType,
		template <class> class ObjectTraits
	>
	struct
	TreeSpatializer
	{
	};

	template<
		class ObjectType,
		template <class> class ObjectTraits
	>
	struct
	TreeSpatializer<2, ObjectType, ObjectTraits>
	{
		typedef typename QuadTree<ObjectType,ObjectTraits>::TPoint TPoint;
		typedef typename QuadTree<ObjectType,ObjectTraits>::TVector TVector;
		typedef typename QuadTree<ObjectType,ObjectTraits>::TValue TValue;
		typedef typename QuadTree<ObjectType,ObjectTraits>::QuadNode QuadNode;

		static void buildSubNodes(QuadNode* ioParentNode)
		{
			TVector newExtents(ioParentNode->extents);
			newExtents*=0.5;

			const TValue cx = ioParentNode->center[0];
			const TValue cy = ioParentNode->center[1];
			const TValue ex = newExtents[0];
			const TValue ey = newExtents[1];
			ioParentNode->node[0] = new QuadNode(TPoint(cx-ex,cy+ey),newExtents);
			ioParentNode->node[1] = new QuadNode(TPoint(cx+ex,cy+ey),newExtents);
			ioParentNode->node[2] = new QuadNode(TPoint(cx+ex,cy-ey),newExtents);
			ioParentNode->node[3] = new QuadNode(TPoint(cx-ex,cy-ey),newExtents);
		}

		static int findSubNode(QuadNode const* iNode, const TPoint& iPoint)
		{
			const TPoint& center = iNode->center;
			if ( iPoint[1] >= center[1] )
				if ( iPoint[0] >= center[0] )
					return 1;
				else
					return 0;
			else
				if ( iPoint[0] >= center[0] )
					return 2;
				else
					return 3;
		}
	};


	template<
		class ObjectType,
		template <class> class ObjectTraits
	>
	struct
	TreeSpatializer<3, ObjectType, ObjectTraits>
	{
		typedef typename QuadTree<ObjectType,ObjectTraits>::TPoint TPoint;
		typedef typename QuadTree<ObjectType,ObjectTraits>::TVector TVector;
		typedef typename QuadTree<ObjectType,ObjectTraits>::TValue TValue;
		typedef typename QuadTree<ObjectType,ObjectTraits>::QuadNode QuadNode;

		static void buildSubNodes(QuadNode* ioParentNode)
		{
			TVector newExtents(ioParentNode->extents);
			newExtents*=0.5;

			const TValue cx = ioParentNode->center[0];
			const TValue cy = ioParentNode->center[1];
			const TValue cz = ioParentNode->center[2];
			const TValue ex = newExtents[0];
			const TValue ey = newExtents[1];
			const TValue ez = newExtents[2];

			ioParentNode->node[0] = new QuadNode(TPoint(cx-ex,cy+ey,cz+ez),newExtents);
			ioParentNode->node[1] = new QuadNode(TPoint(cx+ex,cy+ey,cz+ez),newExtents);
			ioParentNode->node[2] = new QuadNode(TPoint(cx+ex,cy-ey,cz+ez),newExtents);
			ioParentNode->node[3] = new QuadNode(TPoint(cx-ex,cy-ey,cz+ez),newExtents);

			ioParentNode->node[4] = new QuadNode(TPoint(cx-ex,cy+ey,cz-ez),newExtents);
			ioParentNode->node[5] = new QuadNode(TPoint(cx+ex,cy+ey,cz-ez),newExtents);
			ioParentNode->node[6] = new QuadNode(TPoint(cx+ex,cy-ey,cz-ez),newExtents);
			ioParentNode->node[7] = new QuadNode(TPoint(cx-ex,cy-ey,cz-ez),newExtents);
		}

		static int findSubNode(QuadNode const* iNode, const TPoint& iPoint)
		{
			const TPoint& center = iNode->center;
			int baseCell = 0;
			if ( iPoint[1] >= center[1] )
				if ( iPoint[0] >= center[0] )
					baseCell=1;
				else
					baseCell=0;
			else
				if ( iPoint[0] >= center[0] )
					baseCell=2;
				else
					baseCell=3;
			if (iPoint[2] < center[2])
				baseCell+=4;
			return baseCell;
		}
	};

};


template
<
	class ObjectType,
	template <class> class ObjectTraits
>
QuadTree< ObjectType, ObjectTraits >::QuadTree()
{
	root_ = NULL;
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
QuadTree< ObjectType, ObjectTraits>::~QuadTree()
{
	clear();
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
QuadTree< ObjectType, ObjectTraits>::QuadTree(const TPoint& iCenter,const TVector& iExtents)
{
	root_ = new QuadNode(iCenter, iExtents);
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
QuadTree< ObjectType, ObjectTraits >::QuadTree(const TAabb& iBox)
{
	TVector extents = iBox.max()-iBox.min();
	extents *= 0.5;
	root_ = new QuadNode(iBox.center(), extents);
}
template
<
	class ObjectType,
	template <class> class ObjectTraits
>
void QuadTree< ObjectType, ObjectTraits >::clear()
{
	delete root_;
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
int QuadTree< ObjectType, ObjectTraits >::objectCount() const
{
	return root_->objectCount();
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
QuadTree< ObjectType, ObjectTraits >::QuadNode::QuadNode() : leaf(true), level(0), listSize(0)
{
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
QuadTree< ObjectType, ObjectTraits >::QuadNode::QuadNode( const TPoint& iCenter, const TVector& iExtents) :
	center(iCenter), extents(iExtents), listSize(0), level(0), leaf(true)
{
	for (int i=subNodeCount-1;i>=0;--i)
		node[i] = NULL;
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
QuadTree< ObjectType, ObjectTraits >::QuadNode::~QuadNode()
{
	for (int i=0;i<subNodeCount;++i)
		delete node[i];
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
int QuadTree< ObjectType, ObjectTraits >::QuadNode::objectCount() const
{
	int cumulCount = listSize;
	for (int i=0;i<subNodeCount;++i)
	{
		if (node[i])
			cumulCount += node[i]->objectCount();
	}
	return cumulCount;
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
void QuadTree< ObjectType, ObjectTraits >::add(const ObjectType& iObject,int iMaxSize, int iMaxLevel)
{
	if (!root_->add(iObject, iMaxSize, iMaxLevel))
		LASS_THROW("Could not add object to QuadTree");
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
typename QuadTree< ObjectType, ObjectTraits >::QuadNode* QuadTree< ObjectType, ObjectTraits >::QuadNode::add(const ObjectType& iObject,int iMaxSize, int iMaxLevel)
{
	if (leaf)
	{
		data.push_back(iObject);
		++listSize;
		if ( (listSize>iMaxSize) && (level<iMaxLevel))
		{
			decompose();
		}
		return this;
	}
	else
	{
		// unused?  [Bramz]
		//QuadNode* lastNode = NULL;

		TAabb aBox = ObjectTraits<ObjectType>::aabb(iObject);
		int intersectionCount = 0;
		int lastIntersection = 0;
		for (int i=0;i<subNodeCount;++i)
		{
			if (aBox.intersects(node[i]->aabb()))
			{
				++intersectionCount;
				lastIntersection = i;
				if (intersectionCount>1)
					break;
			}
		}
		if (intersectionCount==1)
		{
			return node[lastIntersection]->add(iObject, iMaxSize, iMaxLevel);
		}
		if (intersectionCount>1)
		{
			data.push_back(iObject);
			++listSize;
			return this;
		}
		LASS_THROW("Object not placeable in node");
	}

}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
typename QuadTree< ObjectType, ObjectTraits >::TAabb QuadTree< ObjectType, ObjectTraits >::QuadNode::aabb() const
{
	/* Reconstruction of the aabb everytime an object gets inserted to the quadnode
	*  Altough this is not very efficient, it is only needed during construction.  Better
	*  storage via center and dx,dy can give more efficient lookup code which is the main
	*  interest.  Storing the aabb would give too much overhead in comparison with the gain
	*  in speed.
	*/
	//std::cout << "center:"<<center << std::endl;
	//std::cout << "extents:"<<extents << std::endl;
	TAabb result(center-extents,center+extents);
	//std::cout << "result:"<<result << std::endl;
	return result;
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
void QuadTree< ObjectType, ObjectTraits >::QuadNode::decompose()
{
	if (leaf) // only decompose leaf nodes, others are already decomposed
	{
		impl::TreeSpatializer<dimension, ObjectType, ObjectTraits>::buildSubNodes(this);

		for (int i=0;i<subNodeCount;++i)
			node[i]->level = level+1;
		leaf = false;

		typename TListType::iterator vit = data.begin();

		TAabb nodeAabb[subNodeCount];       // cache node aabb
		for (int i=0;i<subNodeCount;++i)
			nodeAabb[i] = node[i]->aabb();

		while (vit!=data.end())
		{
			typename TListType::iterator bit=vit;
			++bit;
			TAabb tempAabb = ObjectTraits<ObjectType>::aabb( *vit );
			/* for each object we test wether it is contained in one of the
			*  subnodes of the quadnode.  If it is completely in one then move
			*  the object down the tree, but only one level
			*/
			for (int i=0;i<subNodeCount;++i)
				if (nodeAabb[i].contains(tempAabb))
				{
					node[i]->add(*vit);
					--listSize;
					data.erase(vit);
					break;
				}
			vit=bit;
		}
	}
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
void QuadTree< ObjectType, ObjectTraits >::QuadNode::absorb()
{
	if (!leaf)
	{
		for (int i=0;i<subNodeCount;++i)
		{
			node[i].absorb();
			std::copy(node[i]->data.begin(), node[i]->data.end(), data.end());
			listSize += node[i]->listSize;
			delete node[i];
		}
		leaf = true;
	}
}

template
<
	class ObjectType,
	template <class> class ObjectTraits
>
int QuadTree< ObjectType, ObjectTraits >::QuadNode::maxDepth() const
{
	if (leaf)
		return level;
	else
	{
		int maxLevel=node[0]->maxDepth();
		for (int i=1;i<subNodeCount;++i)
			maxLevel = std::max(maxLevel,node[i]->maxDepth());
		return maxLevel;
	}
}


template
<
	class ObjectType,
	template <class> class ObjectTraits
>
int QuadTree< ObjectType, ObjectTraits >::maxDepth() const
{
	return root_->maxDepth();
}


template
<
	class ObjectType,
	template <class> class ObjectTraits
>
int QuadTree< ObjectType, ObjectTraits >::QuadNode::contains(const TPoint& iPoint, std::vector<ObjectType*>& oObjects) const
{
	int hitCount=0;
	typename TListType::const_iterator lIt = data.begin();
	for (;lIt!=data.end();++lIt)
	{
		if (ObjectTraits<ObjectType>::contains(*lIt,iPoint))
		{
			++hitCount;
			oObjects.push_back(const_cast<ObjectType*>(&(*lIt)));
		}
	}
	if (!leaf)
	{
		return   hitCount+
				 node[impl::TreeSpatializer<dimension, ObjectType, ObjectTraits>::findSubNode(this,iPoint)]->contains(iPoint,oObjects);
	}
	return hitCount;
}


template
<
	class ObjectType,
	template <class> class ObjectTraits
>
int QuadTree< ObjectType, ObjectTraits >::contains(const TPoint& iPoint, std::vector<ObjectType*>& oObjects) const
{
	return root_->contains(iPoint, oObjects);
}


}
}

#endif

// EOF
