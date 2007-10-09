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
 *      static bool contains( const TSimplePolygon3D& iP, const TPoint& point)
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
#include "impl/quad_tree_helper.h"

namespace lass
{
namespace spat
{

template
<
	class ObjectType,
	class ObjectTraits = DefaultObjectTraits<ObjectType>
>
class QuadTree: private impl::QuadTreeHelper<ObjectTraits, ObjectTraits::dimension>
{
public:

	typedef QuadTree<ObjectType, ObjectTraits> TSelf;
	typedef ObjectType TObjectType;
	typedef ObjectTraits TObjectTraits;

	typedef typename TObjectTraits::TObjectIterator TObjectIterator;
	typedef typename TObjectTraits::TObjectReference TObjectReference;
	typedef typename TObjectTraits::TAabb TAabb;
	typedef typename TObjectTraits::TRay TRay;
	typedef typename TObjectTraits::TPoint TPoint;
	typedef typename TObjectTraits::TVector TVector;
	typedef typename TObjectTraits::TValue TValue;
	typedef typename TObjectTraits::TParam TParam;
	typedef typename TObjectTraits::TReference TReference;
	typedef typename TObjectTraits::TConstReference TConstReference;
	typedef typename TObjectTraits::TInfo TInfo;

	enum { dimension = TObjectTraits::dimension };

	class Neighbour
	{
	public:
		Neighbour() {}
		Neighbour(TObjectIterator object, TValue squaredDistance): 
			object_(object), squaredDistance_(squaredDistance) {}
		TObjectIterator object() const { return object_; }
		TValue squaredDistance() const { return squaredDistance_; }
		TObjectIterator operator->() const { return object_; }
		TObjectReference operator*() const { return TObjectTraits::object(object_); }
		bool operator<(const Neighbour& other) const { return squaredDistance_ < other.squaredDistance_; }
		bool operator==(const Neighbour& other) const { return object_ == other.object_; }
	private:
		TObjectIterator object_;
		TValue squaredDistance_;
	};

	//typedef typename TObjectTraits::TSeparator TSeparator; // ???

	/** constructor that computes bounding box automatically from objects
	 */
	QuadTree(size_t maxSize = 100, size_t maxLevel = 10);

	/** constructor that computes bounding box automatically from objects
	 */
	QuadTree(TObjectIterator first, TObjectIterator last, size_t maxSize = 10, size_t maxLevel = 10);

	/** constructor.
	*  @param center       the center of the spatial tree
	*  @param extents      the extents of the spatial tree in each direction,
	*                       thus the total width of the tree is twice the extents
	*/
	//QuadTree(TObjectIterator first, TObjectIterator last, const TPoint& center, const TVector& extents, size_t maxSize = 100, size_t maxLevel = 10);

	/** constructor.
	*  @param box          the bounding box of the spatial tree
	*/
	//QuadTree(TObjectIterator first, TObjectIterator last, const TAabb& box, size_t maxSize = 100, size_t maxLevel = 10);

	~QuadTree();

	void reset();
	void reset(TObjectIterator first, TObjectIterator last);

	/** return true if any of the objects contains point.
	*
	* Required :
	* <tt>static bool ObjectTypeTraits::contains( const Object& object, const TPoint& point, const TInfo* info );</tt>
	*/
	const bool contains(const TPoint& p, const TInfo* info = 0) const;

	/** find objects containing point and write them to the output iterator.
	*
	* Required :
	* <tt>static bool ObjectTypeTraits::contains( const Object& object, const TPoint& point, const TInfo* info );</tt>
	*/
	template <typename OutputIterator>
	OutputIterator find(const TPoint& p, OutputIterator result, const TInfo* info = 0) const;

	const TObjectIterator intersect(const TRay& ray, TReference t, TParam tMin = 0, 
		const TInfo* info = 0) const;
	
	const bool intersects(const TRay& ray, TParam tMin = 0, 
		TParam maxT = std::numeric_limits<TValue>::infinity(), const TInfo* info = 0) const;

	const Neighbour nearestNeighbour(const TPoint& point, const TInfo* info = 0) const;	

	template <typename RandomIterator>
	RandomIterator rangeSearch(const TPoint& center, TParam maxRadius, size_t maxCount,
			RandomIterator first, const TInfo* info = 0) const;	

	/** contains.  Returns the number of object that returned sFront on all the lines
	*   provided in the iFrustum vector and _adds_ them to the vector oObjects.  An example
	*   of use is frustum culling.
	*
	* Required :
	*  <tt>static bool ObjectTypeTraits::contains( const Object& object, const TPoint& point );</tt>
	*/
	//size_t visible( const std::vector<TSeparator>& iFrustum, std::vector<ObjectType*>& oObjects ) const;

	void add(TObjectIterator object);
	size_t objectCount() const;

	/** depth. Returns the depth of the tree */
	const size_t depth() const;
	const TValue averageDepth() const;

	void swap(QuadTree& other);
	const TObjectIterator end() const;

private:
	enum { subNodeCount = 1 << dimension };
	typedef std::vector<TObjectIterator> TObjectIterators;
	struct QuadNode
	{
		QuadNode *node[subNodeCount];   /**< 0 = NW, 1 = NE, 2 = SE, 3 = SW for quadtrees*/
		TPoint center;                  /**< center of quadnode */
		TVector extents;                /**< x = half of widht, y = half of height */
		TObjectIterators data;          /**< the list containing the data */
		bool leaf;                      /**< true for leaf nodes */

		QuadNode(const TPoint& aCenter, const TVector& aExtents);
		~QuadNode();

		void add(TObjectIterator object, size_t maxSize, size_t maxLevel, size_t level = 0, bool mayDecompose = true);

		size_t objectCount() const;            /**< number of objects in this node and all its children */
		size_t depth() const;					/**< depth of the child tree */
		const TValue averageDepth() const;
		void decompose(size_t maxSize, size_t maxLevel, size_t level = 0);	/**< split the current node into 4 children */
		void absorb();                      /**< absorb all children into the current node */

		TAabb aabb() const;
	};
	
	const TObjectIterator doIntersect(const QuadNode* node, const TRay& ray, TReference t, TParam tMin, 
		const TInfo* info, const TVector& tNear, const TVector& tFar, size_t flipMask) const;
	const bool doIntersects(const QuadNode* node, const TRay& ray, TParam tMin, TParam tMax, 
		const TInfo* info, const TVector& tNear, const TVector& tFar, size_t flipMask) const;
	void doNearestNeighbour(const QuadNode* node, const TPoint& point, const TInfo* info,
		Neighbour& best) const;
	template <typename RandomIterator>
	RandomIterator doRangeSearch(const QuadNode* node, const TPoint& target, TReference squaredRadius, 
		size_t maxCount, RandomIterator first, RandomIterator last, const TInfo* info) const;

	TAabb aabb_;
	QuadNode*   root_;
	TObjectIterator end_;
	size_t maxSize_;
	size_t maxLevel_;
};



}
}

#include "quad_tree.inl"

#endif

// EOF
