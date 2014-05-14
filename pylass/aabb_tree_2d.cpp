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

#include "pylass_common.h"
#include "aabb_tree_2d.h"

namespace pylass
{

PY_DECLARE_CLASS(AabbTree2D)
PY_CLASS_CONSTRUCTOR_0(AabbTree2D)
PY_CLASS_CONSTRUCTOR_1(AabbTree2D, const AabbTree2D::TObjects&)
PY_CLASS_METHOD(AabbTree2D, reset)
PY_CLASS_METHOD(AabbTree2D, aabb)
PY_CLASS_METHOD(AabbTree2D, contains)
PY_CLASS_METHOD(AabbTree2D, find)
PY_CLASS_METHOD_QUALIFIED_2(AabbTree2D, intersect, AabbTree2D::TIntersectResult, const AabbTree2D::TPoint&, const AabbTree2D::TVector&)
PY_CLASS_METHOD_QUALIFIED_3(AabbTree2D, intersect, AabbTree2D::TIntersectResult, const AabbTree2D::TPoint&, const AabbTree2D::TVector&, AabbTree2D::TParam)
PY_CLASS_METHOD_QUALIFIED_2(AabbTree2D, intersects, bool, const AabbTree2D::TPoint&, const AabbTree2D::TVector&)
PY_CLASS_METHOD_QUALIFIED_3(AabbTree2D, intersects, bool, const AabbTree2D::TPoint&, const AabbTree2D::TVector&, AabbTree2D::TParam)
PY_CLASS_METHOD_QUALIFIED_4(AabbTree2D, intersects, bool, const AabbTree2D::TPoint&, const AabbTree2D::TVector&, AabbTree2D::TParam, AabbTree2D::TParam)
PY_CLASS_METHOD(AabbTree2D, swap)
PY_CLASS_METHOD(AabbTree2D, isEmpty)
PY_CLASS_METHOD(AabbTree2D, clear)

AabbTree2D::AabbTree2D():
	objects_(),
	tree_()
{
}

AabbTree2D::AabbTree2D(const TObjects& iObjects):
	objects_(iObjects),
	tree_()
{
	tree_.reset(objects_.begin(), objects_.end());
}

void AabbTree2D::reset(const TObjects& iObjects)
{
	AabbTree2D temp(iObjects);
	swap(temp);
}

const AabbTree2D::TAabb AabbTree2D::aabb() const
{
	return tree_.aabb();
}

bool AabbTree2D::contains(const TPoint& iPoint) const
{
	return tree_.contains(iPoint);
}

AabbTree2D::TObjects AabbTree2D::find(const TPoint& iPoint) const
{
	typedef std::vector<TObjectIterator> TObjectIterators;

	TObjectIterators temp;
	tree_.find(iPoint, std::back_inserter(temp));

	TObjects result;
	for (TObjectIterators::const_iterator i = temp.begin(); i != temp.end(); ++i)
	{
		result.push_back(**i);
	}

	return result;
}

AabbTree2D::TIntersectResult AabbTree2D::intersect(const TPoint& iSupport, const TVector& iDirection) const
{
	return intersect(iSupport, iDirection, 0);
}

AabbTree2D::TIntersectResult AabbTree2D::intersect(const TPoint& iSupport, const TVector& iDirection, TParam iMinT) const
{
	TValue t;
	const TObjectIterator hit = tree_.intersect(TRay(iSupport, iDirection), t, iMinT);
	if (hit != objects_.end())
	{
		return TIntersectResult(*hit, t);
	}
	else
	{
		return TIntersectResult(TObject(), iMinT);
	}
}

bool AabbTree2D::intersects(const TPoint& iSupport, const TVector& iDirection) const
{
	return tree_.intersects(TRay(iSupport, iDirection));
}

bool AabbTree2D::intersects(const TPoint& iSupport, const TVector& iDirection, TParam iMinT) const
{
	return tree_.intersects(TRay(iSupport, iDirection), iMinT);
}

bool AabbTree2D::intersects(const TPoint& iSupport, const TVector& iDirection, TParam iMinT, TParam iMaxT) const
{
	return tree_.intersects(TRay(iSupport, iDirection), iMinT, iMaxT);
}

void AabbTree2D::swap(AabbTree2D& iOther)
{
	objects_.swap(iOther.objects_);
	tree_.swap(iOther.tree_);
}

bool AabbTree2D::isEmpty() const
{
	LASS_ASSERT(objects_.empty() == tree_.isEmpty());
	return objects_.empty();
}

void AabbTree2D::clear()
{
	objects_.clear();
	tree_.reset();
}

}

// EOF
