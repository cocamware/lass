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
 *	Copyright (C) 2004-2010 the Initial Developer.
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


/** @struct lass::prim::Plane3D
 *  @brief A 3D hyper plane.
 *  @author BdG
 *  @date 2003
 *
 *  I assume if you look to this class, you'll wonder ... "where the heck did all the code go?"
 *  Hah, I'll tell ya.  There are more than one possible models for representing a plane.  We can
 *  use a cartesian equation N.P + S = 0, or we can use a parametric equation P = S + x*U + y*V.
 *  This class let's you choose what implementation you want: a pure partesian model, a pure
 *  parametric model, or a model that combines both.  This is done by moving all code to
 *  implemenations impl::Plane3DCartesian, impl::Plane3DParametric or impl::Plane3DCombined of
 *  which this class with inherent accoring the model you've chosen.  You can select the one you
 *  want by specifying the template parameter @c EquationPolicy.  You can either use Cartesian
 *  (which is the default), Parametric or Combined.  Each of them will model the plane
 *  differently.  They all provide the same interface, but might have different results.  They
 *  might have different memory footprints, different performances, and are optimized for
 *  different purposes.  Cartesian will select an implementation that only uses the cartesian
 *  equation and will be the smallest and the fastest for most purposes (that's why it is the
 *  default :).  For texture mapping, you might more like the Parametric model, because it has
 *  better support for direction vectors, but it'll have to calculate the normal on the spot if
 *  you need it.  Combined is the workhorse for heavy duties and implements both.  Plus, it
 *  buffers the reciprocal direction vectors, so finding the parameters (x, y) of a point on the
 *  plane can happen faster.
 *
 *  @section common_interface common interface
 *
 *  Anyway, let me give you some general info on this whole Plane3D thing.  Though there are
 *  three different implemntations, they all show the same interface.  We'll explore them by this
 *  common
 *  interface:
 *
 *  @subsection type_definitions type definitions
 *
 *  - @c TSelf: the type of @c this
 *  - @c TImpl: type of implemantion, can be impl::Plane3DCartesian, impl::Plane3DParametric or
 *              impl::Plane3DCombined.
 *  - @c TNormalizingPolicy: the type you've used as NormalizingPolicy.
 *  - @c TPoint: type of a afine point in space.
 *  - @c TVector: type of a vector in space.
 *  - @c TUV: a pair of values (u, v) for the parametric equation P = S + u*U + v*V.
 *  - @c TValue: same as util::CallTraits<T>::TValue.
 *  - @c TParam: same as util::CallTraits<T>::TParam.
 *  - @c TReference: same as util::CallTraits<T>::TReference.
 *  - @c TConstReference: same as util::CallTraits<T>::TConstReference.
 *  - @c TNumTraits: same as num::NumTraits<T>.
 *
 *  @subsection constructors
 *
 *  @par Plane3D():
 *  the default constructor brings the plane in an invalid state.  normal and/or direction vectors
 *  will have zero length, which indicates for this invalid state.
 *
 *  @par Plane3D(const TPoint& iSupport, const TPoint& iPointU, const TPoint& iPointV):
 *  construct a plane through three points.  The parametric equation P = S + u*U + v*V is going to
 *  be constructed as following: iSupport will be the support point S, and direction vectors U and
 *  V will be drawn from the support point to iPointU and iPointV: U = iPointU - iSupport and V =
 *  iPointV - iSupport.  The cartesian equation N.P + d == 0 is constructed as following: the normal
 *  N = U x V.  d is determined by demaning that N.iSupport + d == 0.
 *
 *  @par Plane3D(const TPoint& iSupport, const TVector& iDirU, const TVector& iDirV):
 *  constructs a plane through one support point and two direction vectors (the parametric equation,
 *  anyone? :).  For the parametric equation P = S + u*U + v*V, we have iSupport for S, iDirU for U
 *  and iDirV for V.  Again the cartesian equation N.P + d == 0 is given by N = U x V and
 *  N.iSupport + d == 0.
 *
 *  @par Plane3D(const TPoint& iSupport, const TVector& iNormal):
 *  constructs a plane through a support point and a normal vector.  For the parametric equation
 *  P = S + u*U + v*V, we have iSupport for S, but U and V we have to get a little dirty.  We have
 *  to find two vectors U and V so that iNormal = U x V.  See Plane3DCommonImpl::generateDirections
 *  on how we do that.  The cartesian N.P + d == 0 is simple, we have iNormal for N and d can be
 *  found by N.iSupport + d == 0.
 *
 *  @par Plane3D(const TVector& iNormal, TParam iD):
 *  construct a plane by a normal vector and fourth value (cartesian equation? :).  For the
 *  parametric equation P = S + u*U + v*V, all have to generated: for S we use the point -iD*iNormal
 *  that is the closest point of the plane to the origin, U and V we've generated again with our
 *  little trick Plane3DCommonImpl::generateDirections.  For the cartesian equation N.P + d == 0, we
 *  of course use N = iNormal and d = iD.
 *
 *  @subsection accessors
 *
 *  After constructions, all vectors are normalized depending on the NormalizingPolicy you've
 *  choosen.  if the normal vector N is scaled, then d is scaled as well, so it still represents
 *  the same plane.
 *
 *  Now, we have a series of accessors that give you access to the internal data of the plane,
 *  including support point, direction vectors, normal vector, ...  Only Plane3DCombined will be
 *  able to pull these directly from its internals, but the others don't have all data aboard, so
 *  they have to generate them.  Be carefull, because it's not always what you suspect.
 *
 *  @par const TPoint support():
 *  returns the support point of the plane.  If you used a support point for the construction of the
 *  plane, models Plane3DParametric and Plane3DCombined will give you back this original one,
 *  exactly till the last bit.  Plane3DCartesian doesn't keep track of support points, so it has to
 *  create one if you ask for it.  It uses -d*N, the point of the plane that is closest to the
 *  origin.  Mind you that this one probably will not be the same as the original one, totally
 *  different even!  Of course, if you used a cartesian equation to construct the plane (without any
 *  support point), then all three models need to generate a support point at some point: -d*N.  In*
 *  that case, all three models @e will return the same support point.
 *
 *  @par void getDirections(TVector& oDirU, TVector& oDirV):
 *  get the direction vectors of the plane.  almost same story as for support().  If you've created
 *  the plane by two direction vectors or through three points (which leads to two direction
 *  vectors), then models Plane3DParametric and Plane3DCombined will give you back these originals
 *  (for Unnormalized planes only!  but in case of Normalized planes, they still correspond with the
 *  original directions).  Plane3DCartesian has to regenerate them by using
 *  Plane3DCommonImpl::generateDirections, and hence it can end up with total different directions!
 *  (of the same plane of course).  In case you've feeded the plane with a normal vector instead of
 *  direction vectors, all have to generate them and end up with the same ones.
 *
 *  @par const TVector directionU():
 *  same story as getDirections(oDirU, oDirV), but only returns direction U.  In case of
 *  Plane3DCartesian, it calls getDirections(oDirU, oDirV) itselfs, but throws away half of the
 *  result.  Hence, it's pretty slow, certainly if you also call dirV(), cause it does the same
 *  thing.  And then you end up with two hidden calls to getDirections(oDirU, oDirV) (which means
 *  expensive generation of direction vectors), while you could have had the same result by calling
 *  getDirections(oDirU, oDirV) once yourself but at half the price.  So, the tip is: DO use
 *  getDirections(oDirU, oDirV) if you can and need them both, and certainly if you're dealing with
 *  the Plane3DCartesian model.
 *
 *  @par const TVector directionV():
 *  same story as directionU(), but returns V instead and throws away U.
 *
 *  @par void getReciprocals(TVector& oReciprocalU, TVector& oReciprocalV):
 *  gets the reciprocals for the direction vectors.  These reciprocals can be used to find back
 *  (u, v) in the parametric equation P = S + u*U + v*V for given P.  u = oReciprocalU * (P - S) and
 *  v = oReciprocalV * (P - S).  In fact, they are used by TUV uv(const TPoint& iPoint) which is
 *  described below :)  I'm not going to get in detail how get these reciprocals, but I will tell
 *  you that only Plane3DCombined has these stored on board, and can return them or use them without
 *  to generate them.  Plane3DParametric does has to generate them on the spot, and so does
 *  Plane3DCartesian.  However, for Plane3DCartesian it is even worse, because before it can create
 *  reciprocals, it has to create direction vectors first!
 *
 *  @par const TVector reciprocalU():
 *  same story as getReciprocals(oReciprocalU, oReciprocalV), but returns only oReciprocalU.  same
 *  remark as on const TVector dirU(): if it has to  generate it, it has to generate both
 *  reciprocals, so it might be faster to use getReciprocals(oReciprocalU, oReciprocalV).
 *
 *  @par const TVector reciprocalV():
 *  same story as reciprocalV(), but returns the other one.
 *
 *  @par void getCartesian(TVector& oNormal, TReference oD):
 *  gets normal vector and d-value for the cartesian equation.  Now, this is the first one
 *  Plane3DCartesian has aboard itself.  Plane3DCombined also has it, but now it's up to
 *  Plane3DParametric to generate some stuff :)  Though, it's less worse than the other way around.
 *  This is because the parametric equation contains more information that the normal vector, and
 *  doesn't have to 'invent' anything like Plane3DCartesian has to do to return support point or
 *  direction vectors.  Actually, in theory, all three models should return the same normal and
 *  d-value.  In practice however, we have to deal with numerical imprecisions.  so the result of
 *  Plane3DParametric can differ a little in the last bits.  not a big difference, but enough to be
 *  inequal.
 *
 *  @par const TVector normal():
 *  returns only the normal vector of the cartesian equation.  For Plane3DParametric we have again
 *  the same remark as for getDirections and getReciprocals: it uses getCartesian anyway, so that
 *  might be faster if you both need the normal and d-value.
 *
 *  @par TValue d():
 *  same as normal(), but returns only the value d instead of the normal vector.
 *
 *  @par const XYZ majorAxis() const
 *  Return the major axis of the normal vector.
 *  The major axis is the one with the largest (absolute) component value.  e.g. if the normal
 *  vector is (-1, 4, -8), this will be the @e z axis because abs(-8) > abs(4) > abs(-1).
 *  In case there's more than one major axis possible, the "highest" index is choosen.  e.g.
 *  if the normal vector is (1, 1, 0), then @e y axis will be choosen, because @e y has a higher
 *  index than @e x .

 *
 *  @subsection methods_cartesian cartesian methods
 *
 *  So far the accessors.  let's get to cooler stuff.  For most of this stuff, we need the cartesian
 *  equation.  Plane3DCartesian and Plane3DCombined have it on board, but Plane3DParametric will
 *  have to generate it each call.  Keep that in mind!
 *
 *  @par Side classify(const TPoint& iPoint):
 *  tells at which side of the plane we can find the point iPoint: the front (sFront), the back
 *  (sBack), or right on the surface (sSurface). the front is the side where the normal sticks or
 *  points to.  The back is the other one. iPoint is exactly one the surface if N.iPoint + d == 0.
 *  Since we need the parametric equation, Plane3DParametric might have a performance hit here.
 *
 *  @par TValue equation(const TPoint& iPoint):
 *  fills in the point iPoint in the cartesian equation and returns the resutl.  N.iPoint + d will
 *  usually not be equal to zero (it's only zero for points on the plane).  This method returns what
 *  it is equal to. i.e. it returns N.iPoint + d.  For Normalized planes this is the same as the
 *  distance of iPoint to the plane, but not for Unnormalized planes, keep that in mind!  If you
 *  need the distance, used signedDistances(iPoint) as described below.  Again you might have a
 *  performance hit for the Plane3DParametric model because of the need of the cartesian equation.
 *
 *  @par TValue signedDistance(const TPoint& iPoint):
 *  returns the distance of the point to the plane, but signed. it will be positive for points in
 *  front of the plane, and negative for the ones in the back (also see: classify(iPoint)).  The
 *  real (unsigned) distances is simply the absolute value of the result.  For Normalized planes
 *  signedDistances(iPoint) will be equal to equation(iPoint).  But for Unnormalized planes
 *  signedDistances still divides through the normal's length Again performance hit for
 *  Plane3DParametric because of the need of the cartesian equation.
 *
 *  @par TValue squaredDistance(const TPoint& iPoint):
 *  returns the distance of iPoint to the plane but squared.  Not much more to tell, realy :)  Again
 *  performance hit for Plane3DParametric because of the cartesian equation.
 *
 *  @par TVector reject(const TPoint& iPoint):
 *  returns rejection of iPoint by the plane.  This is a bit tricky to explain.  If you have a
 *  support point S, then this rejection is the part of (iPoint - S) that is parallel to the normal
 *  vector (or orthogonal to the plane).  This would be the same as the rejection of (iPoint - S)
 *  (given by reject(iPoint - S), see below). But more descriptive might be: it is the vector you
 *  have to add to the projection of this point on the plane (given by project(iPoint), see below),
 *  to get back iPoint: iPoint == project(iPoint) + reject(iPoint). Again performance hit for
 *  Plane3DParametric because of the cartesian equation.
 *
 *  @par TVector reject(const TVector& iVector):
 *  returns rejection of iVector by the plane.  This is already somewhat easier.  the rejection of
 *  iVector is that part of iVector that is parallel to the normal vector of the plane.  You can µ
 *  also say that it is the orthogonal projection of iVector on the normal vector.  Or the part of
 *  iVector that is orthogonal to the plane.   Again performance hit for Plane3DParametric because
 *  of the cartersian equation.
 *
 *  @par TPoint project(const TPoint& iPoint):
 *  return the orthogonal projection of iPoint on the plane.  It is the point on the plane that is
 *  the closest one to iPoint.  If you draw a line through iPoint parallel to the normal vector,
 *  this projection is the point where this line intersects the plane.  It is know that iPoint ==
 *  project(iPoint) + reject(iPoint).  Again performance hit for Plane3DParametric because of the
 *  cartesian equation.
 *
 *  @par TVector project(const TVector& iVector):
 *  return the orthogonal projection of iVector on the plane.  It is the part of iVector that is parallel to the plane, or orthogonal to
 *  the normal vector.  It is known that iVector == project(iVector) + reject(iVector).  Again
 *  performance hit for Plane3DParametric because of the cartesian equation.
 *
 *  @par TPoint reflect(const TPoint& iPoint):
 *  return the reflection of iPoint in the plane.  It is the point at the same distance of the
 *  plane, but at the exact opposite side of the plane.  If you draw a line through iPoint parallel
 *  to the normal vector, it's the only other point on that line that is at the same (absolute)
 *  distance of the plane. If you walk from iPoint to the intersection point of the line and the
 *  plane, and you walk further the same distance again, you arrive at reflection of iPoint.  It is
 *  known that reflect(iPoint) == project(iPoint) - reject(iPoint).  Again performance hit for
 *  Plane3DParametric because of the cartesian equation.
 *
 *  @par TVector reflect(const TVector& iVector):
 *  return the reflection of iVector in the plane. It's the vector of which the orthogonal part to
 *  the plane is flipped.  It is know that reflect(iVector) == project(iVector) - reject(iVector).
 *  Again performance hit for Plane3DParametric because of the cartesian equation.
 *
 *  @subsection cartesian_methods cartesian methods
 *
 *  So far functions for the cartesian boys.  Now some stuff for parametric fellows. It's about how
 *  we can get a point of the plane if we now its two parametrs u and v, and how we can find u and v
 *  if we know a point of the plane.
 *
 *  @par TPoint point(TValue iU, TValue iV):
 *  return a point of the parametric equation P = S + iU * dirU + iV * dirV.  In case of
 *  Plane3DCartesian, we have the same remarks as for getDirections(oDirU, oDirV): not only we have
 *  a performance hit, we probably also have to deal with totally different direction vectors than
 *  the ones we have put in the constructor.
 *
 *  @par TPoint point(TUV iUV):
 *  return a point of the parametric equation P = S + iUV.x * dirU + iUV.y * dirV.  Pretty much the
 *  same thing as the one above, but packs both values in one pair iUV.  Same remarks on
 *  Plane3DCartesian.
 *
 *  @par TUV uv(const TPoint& iPoint):
 *  returns a pair a values (u, v) so that iPoint == S + u * dirU + v * dirV.  In theory, if you put
 *  these back in point(u, v), you should end up with iPoint again: point(uv(iPoint)) == iPoint ?
 *  Well, this is not totally true First of all, even in theory, it can only be true if iPoint is a
 *  point of the plane. if iPoint is not on the plane, then uv(iPoint) will give you the parameters
 *  of the point on the plane that is closest to iPoint.  And that point is the projection of
 *  iPoint.  So, the most you can say is that point(uv(iPoint)) == project(iPoint) ?  Yes, but IN
 *  THEORY!  In practice, however, numerical imprecisions will probably give you a slightly
 *  different result.  You'll be very close, but the last bits will differ enough to make the them
 *  inequal.  But with some epsilons, you'll be alright.  Some performance remarks:  Only
 *  Plane3DCombined has the necessary reciprocals for the direction vectors on board (remember the
 *  getReciprocals() discussion?  well, here it we have it :) Plane3DParametric does not, and will
 *  have to recreate them on each call.  This _might_ and probably will be quite costly.  For
 *  Plane3DCartesian, you're totally screwed: not only do we have to genereate the reciprocals, but
 *  we also have to generate the direction vectors we want to create reciprocals of!  So, you'll pay
 *  twice.  And above all, it won't be the reciprocals of the direction vectors you've put it.
 *
 *  @subsection misc_methods misc. methods
 *
 *  @par void flip():
 *  flips the plane so that the front becomes the back and the back becomes the front.  For the
 *  cartesian equation, this is done by negating the normal and d: N = -N and d = -d.  Of the parametric
 *  equation, direction vector U is flipped:
 *  U = -U.
 *
 *  @par bool isValid():
 *  returns true if the interal data represents a valid plane, and false if not.  A plane is valid
 *  if none of the direction vectors nor the normal vector are zero vectors.  And above of that,
 *  the direction vectors may not be colinear. Of course, we only test internal data.  We will not
 *  generate direction vectors (in case of Plane3DCartesian), just to test the if they are valid.
 *  We can safely do that, because we know that if the normal vector is valid, then the generated
 *  directions will be too.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_PLANE_3D_H

#include "prim_common.h"
#include "equation_policy.h"
#include "normalizing_policy.h"
#include "point_2d.h"
#include "point_3dh.h"
#include "xyz.h"
#include "impl/plane_3d_impl.h"
#include "../num/floating_point_comparison.h"



namespace lass
{
namespace prim
{

template <typename T, class EquationPolicy = Cartesian, class NormalizingPolicy = Normalized>
class Plane3D: public impl::Plane3DImpl<T, EquationPolicy, NormalizingPolicy>::Type
{
public:

	typedef Plane3D<T, EquationPolicy, NormalizingPolicy> TSelf;
	typedef typename impl::Plane3DImpl<T, EquationPolicy, NormalizingPolicy>::Type TImpl;

	typedef typename TImpl::TPoint  TPoint;
	typedef typename TImpl::TVector TVector;
	typedef typename TImpl::TParam  TParam;
	typedef typename TImpl::TValue  TValue;
	typedef typename TImpl::TReference  TReference;
	typedef typename TImpl::TConstReference TConstReference;
	typedef typename TImpl::TNumTraits TNumTraits;

	template <typename U>
	struct Rebind
	{
		typedef Plane3D<U, EquationPolicy, NormalizingPolicy> Type;
	};

	Plane3D();
	Plane3D(const TPoint& iSupport, const TPoint& iPointU, const TPoint& iPointV);
	Plane3D(const TPoint& iSupport, const TVector& iDirU, const TVector& iDirV);
	Plane3D(const TVector& iNormal, const TPoint& iSupport);
	Plane3D(const TVector& iNormal, TParam iD);

	Side classify(const TPoint& iPoint) const;
	const TValue signedDistance(const TPoint& iPoint) const;
	const TValue squaredDistance(const TPoint& iPoint) const;

	Side classify(const TPoint& iPoint, TParam iRelativeTolerance) const;
	const TValue signedDistance(const TPoint& iPoint, TParam iRelativeTolerance) const;
	const TValue squaredDistance(const TPoint& iPoint, TParam iRelativeTolerance) const;

	const XYZ majorAxis() const;

	/*
	typedef NormalizingPolicy TNormalizingPolicy;

	typedef Point3D<T> TPoint;
	typedef TPoint::TVector TVector;
	typedef Point2D<T> TUV;

	typedef TPoint::TValue TValue;
	typedef TPoint::TParam TParam;
	typedef TPoint::TReference TReference;
	typedef TPoint::TConstReference TConstReference;
	typedef TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension };

	const TPoint support() const;
	void getDirections(TVector& oDirU, TVector& oDirV) const;
	const TVector directionU() const;
	const TVector directionV() const;

	void getReciprocals(TVector& oReciprocalU, TVector& oReciprocalV) const;
	const TVector reciprocalU() const;
	const TVector reciprocalV() const;

	void getCartesian(TVector& oNormal, TReference oD) const;
	const TVector& normal() const;
	TConstReference d() const;
	const XYZ majorAxis() const;

	Side classify(const TPoint& iPoint) const;
	TValue equation(const TPoint& iPoint) const;
	TValue signedDistance(const TPoint& iPoint) const;
	TValue squaredDistance(const TPoint& iPoint) const;

	TVector reject(const TPoint& iPoint) const;
	TVector reject(const TVector& iVector) const;
	TPoint project(const TPoint& iPoint) const;
	TVector project(const TVector& iVector) const;
	TPoint reflect(const TPoint& iPoint) const;
	TVector reflect(const TVector& iVector) const;

	TPoint point(TParam iU, TParam iV) const;
	TPoint point(const TUV& iUV) const;
	TUV uv(const TPoint& iPoint) const;

	void flip();
	bool isValid() const;
	*/
};

template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Plane3D<T, Cartesian>& iPlane);
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Plane3D<T, Parametric>& iPlane);
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Plane3D<T, Combined>& iPlane);

}

}

#include "plane_3d.inl"

//#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_3D_H
//#include "line_3d_plane_3d.h"
//#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_AABB_3D_H
#	include "aabb_3d_plane_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_SEGMENT_3D_H
#	include "line_segment_3d_plane_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_3D_H
#	include "plane_3d_ray_3d.h"
#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_H
#	include "plane_3d_transformation_3d.h"
#endif

#endif

// --- END OF FILE ------------------------------------------------------------------------------
