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

#ifndef KD_TREE_GUARDIAN_OF_INCLUSION_AABB_TREE_2D_H
#define KD_TREE_GUARDIAN_OF_INCLUSION_AABB_TREE_2D_H

#include "pylass_common.h"
#include <lass/prim/aabb_2d.h>
#include <lass/prim/ray_2d.h>
#include <lass/spat/aabb_tree.h>

namespace pylass
{

struct ObjectTraits: 
	public spat::DefaultAabbRayTraits<
		prim::Aabb2D<double>,
		prim::Ray2D<double>
	>
{
	typedef python::PyObjectPtr<PyObject>::Type TObject;
	typedef std::vector<TObject>::const_iterator TObjectIterator;		/**< iterator to object */
	typedef const TObject& TObjectReference;	/**< reference to object */
	typedef void TInfo;

	static const TAabb objectAabb(TObjectIterator object) 
	{
		python::LockGIL LASS_UNUSED(lock);
		TObject method(python::pyBuildSimpleObject("aabb"));
		TObject result(PyObject_CallMethodObjArgs(object->get(), method.get(), 0));
		if (!result)
		{
			python::impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}

		TAabb temp;
		if (python::pyGetSimpleObject(result.get(), temp) != 0)
		{
			LASS_THROW("bad result");
		}
		return temp;
	}
	
	static bool objectContains(TObjectIterator object, const TPoint& point, const TInfo*) 
	{ 
		python::LockGIL LASS_UNUSED(lock);
		TObject method(python::pyBuildSimpleObject("contains"));
		TObject p(python::pyBuildSimpleObject(point));
		TObject result(PyObject_CallMethodObjArgs(object->get(), method.get(), p.get(), 0));
		if (!result)
		{
			python::impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}

		bool temp;
		if (python::pyGetSimpleObject(result.get(), temp) != 0)
		{
			LASS_THROW("bad result");
		}
		return temp;
	}

	static bool objectIntersect(TObjectIterator object, const TRay& ray, TReference t, TParam tMin, const TInfo*)
	{
		python::LockGIL LASS_UNUSED(lock);
		TObject method(python::pyBuildSimpleObject("intersect"));
		TObject support(python::pyBuildSimpleObject(ray.support()));
		TObject direction(python::pyBuildSimpleObject(ray.direction()));
		TObject minT(python::pyBuildSimpleObject(tMin));
		TObject result(PyObject_CallMethodObjArgs(object->get(), method.get(), support.get(), direction.get(), minT.get(), 0));
		if (!result)
		{
			python::impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}

		TValue tCandidate;
		if (python::pyGetSimpleObject(result.get(), tCandidate) != 0)
		{
			LASS_THROW("bad result");
		}
		if (tCandidate > tMin)
		{
			t = tCandidate;
			return true;
		}
		return false;
	}

	/** return true if object is intersected by ray
	 */
	static bool objectIntersects(TObjectIterator object, const TRay& ray, TParam tMin, TParam tMax, const TInfo*)
	{
		python::LockGIL LASS_UNUSED(lock);
		TObject method(python::pyBuildSimpleObject("intersects"));
		TObject support(python::pyBuildSimpleObject(ray.support()));
		TObject direction(python::pyBuildSimpleObject(ray.direction()));
		TObject minT(python::pyBuildSimpleObject(tMin));
		TObject maxT(python::pyBuildSimpleObject(tMax));
		TObject result(PyObject_CallMethodObjArgs(object->get(), method.get(), support.get(), direction.get(), minT.get(), maxT.get(), 0));
		if (!result)
		{
			python::impl::fetchAndThrowPythonException(LASS_PRETTY_FUNCTION);
		}

		bool temp;
		if (python::pyGetSimpleObject(result.get(), temp) != 0)
		{
			LASS_THROW("bad result");
		}
		return temp;
	}
};


class AabbTree2D: public python::PyObjectPlus
{
	PY_HEADER(python::PyObjectPlus)
public:
	typedef python::PyObjectPtr<PyObject>::Type TObject;
	typedef std::vector<TObject> TObjects;
	typedef spat::AabbTree<TObject, ObjectTraits> TTree;

	typedef TTree::TObjectIterator TObjectIterator;
	typedef TTree::TAabb TAabb;
	typedef TTree::TRay TRay;
	typedef TTree::TPoint TPoint;
	typedef TTree::TVector TVector;
	typedef TTree::TValue TValue;
	typedef TTree::TParam TParam;

	typedef std::pair<TObject, TValue> TIntersectResult;

	AabbTree2D();
	AabbTree2D(const TObjects& iObjects);

	void reset(const TObjects& iObjects);

	const TAabb aabb() const;

	bool contains(const TPoint& point) const;
	TObjects find(const TPoint& point) const;
	TIntersectResult intersect(const TPoint& support, const TVector& direction) const;
	TIntersectResult intersect(const TPoint& support, const TVector& direction, TParam tMin) const;
	bool intersects(const TPoint& support, const TVector& direction) const;
	bool intersects(const TPoint& support, const TVector& direction, TParam tMin) const;
	bool intersects(const TPoint& support, const TVector& direction, TParam tMin, TParam tMax) const;

	void swap(AabbTree2D& other);
	bool isEmpty() const;
	void clear();

private:
	TObjects objects_;
	TTree tree_;
};

}

#endif

