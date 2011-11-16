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



/** @class lass::prim::Line2D
 *  @brief 2D Line
 *  @author Bram de Greve [BdG]
 *  @date 2003
 *
 *  I assume if you look to this class, you'll wonder ... "where the heck did all the code go?"
 *  Hah, I'll tell ya.  There is more than one possible model to represent a line.  We can
 *  use a cartesian equation @c N.P+S=0, or we can use a parametric equation @c P=S+t*U.
 *  This class lets you choose what implementation you want: a pure cartesian model, a pure
 *  parametric model, or a model that combines both.  This is done by moving all code to
 *  implemenations impl::Line2DCartesian, impl::Line2DParametric or impl::Line2DCombined.  Line2D
 *  will inherit its implementation of the model you've choosen.  You can select the one you
 *  want by specifying the template parameter @e EquationPolicy.  You can either use Cartesian
 *  (which is the default), Parametric or Combined.  Each of them will model the line
 *  differently.  They all provide the same interface, but might have different results.  They
 *  might have different memory footprints, different performances, and are optimized for
 *  different purposes.  Cartesian will select an implementation that only uses the cartesian
 *  equation and will be the smallest and the fastest for most purposes (that's why it is the
 *  default :).  But sometimes, you might more like the Parametric model, because it has
 *  better support for direction vectors, but it'll have to calculate the normal on the spot if
 *  you need it.  Combined is the workhorse for heavy duties and implements both..
 *
 *  @section template arguments
 *
 *  - @a T: the underlying data type, the proton.
 *  - @a @ref EquationPolicy: chooses the model: Cartesian, Parametric or Combined
 *  - @a @ref NormalizingPolicy: enables automatic normalizing or not: Normalized or Unnormalized
 *
 *  @section common_interface common interface
 *
 *  Anyway, let me give you some general info on this whole Line2D thing.  Though there are
 *  three different implentations, they all have the same interface.  We'll explore them by this
 *  common interface:
 *
 *  @subsection type_definitions type definitions
 *
 *  - @c TSelf: the type of @a this
 *  - @c TImpl: type of implemantion, can be impl::Line2DCartesian, impl::Line2DParametric or
 *              impl::Line2DCombined.
 *  - @c TNormalizingPolicy: the type you've used as NormalizingPolicy.
 *  - @c TPoint: type of a afine point in space.
 *  - @c TVector: type of a vector in space.
 *  - @c TValue: same as util::CallTraits<T>::TValue.
 *  - @c TParam: same as util::CallTraits<T>::TParam.
 *  - @c TReference: same as util::CallTraits<T>::TReference.
 *  - @c TConstReference: same as util::CallTraits<T>::TConstReference.
 *  - @c TNumTraits: same as num::NumTraits<T>.
 *
 *  @subsection constructors
 *
 *  @par Line2D():
 *  the default constructor brings the line in an invalid state.  normal and/or direction vector
 *  will have zero length, which indicates for this invalid state.
 *
 *  @par Line2D(const TPoint& iSupport, const TPoint& iPoint):
 *  construct a line through two points.  The parametric equation @a P=S+t*U is going to
 *  be constructed as following: iSupport will be the support point @a S, and direction vector @a U
 *  will be drawn from the support point to @a iPoint: @a U = @a iPointU - @a iSupport.  The cartesian
 *  equation @a N.P+d==0 is constructed as following: the normal @a N=perpDot(U) and @a d is determined
 *  by demaning that @a N.S+d==0.
 *
 *  @par Line2D(const TPoint& iSupport, const TVector& iDirection):
 *  constructs a line through one support point and one direction vector (the parametric equation,
 *  anyone? :).  For the parametric equation @a P=S+t*U, we have @a iSupport for @a S, @a iDirection
 *  for @a U.  Again the cartesian equation @a N.P+d==0 is given by @a N=perpDot(U) and
 *  @a N.S+d==0.
 *
 *  @par Line2D(const TPoint& iSupport, const TVector& iNormal):
 *  constructs a line through a support point and a normal vector.  For the parametric equation
 *  @a P=S+t*U, we have @a iSupport for @a S, and @a U=-perpDot(N).  The cartesian @a N.P+d==0 is
 *  simple, we have @a iNormal for @a N and @a d can be found by @a N.S+d==0.
 *
 *  @par Line2D(const TVector& iNormal, TParam iD):
 *  construct a line by a normal vector and fourth value (cartesian equation? :).  For the
 *  parametric equation @a P=S+t*U, all have to generated: for @a S we use the point @a -iD*iNormal
 *  that is the closest point of the line to the origin, @a U is given by @a U=-perpDot(N).
 *  For the cartesian equation @a N.P+d==0, we of course use @a N = @a iNormal and @a d = @a iD.
 *
 *  @subsection accessors
 *
 *  After construction, all vectors are normalized depending on the @a NormalizingPolicy you've
 *  choosen as template argument.  if the normal vector @a N is scaled, then @a d is scaled as well, so
 *  it still represents the same line.
 *
 *  Now, we have a series of accessors that give you access to the internal data of the line,
 *  including support point, direction vectors, normal vector, ...  Only Line2DCombined will be
 *  able to pull these directly from its internals, but the others don't have all data aboard, so
 *  they have to generate them.  Be carefull, because it's not always what you suspect.
 *
 *  @par const TPoint support():
 *  returns the support point of the line.  If you used a support point for the construction of the
 *  line, models Line2DParametric and Line2DCombined will give you back this original one,
 *  exactly till the last bit.  Line2DCartesian doesn't keep track of support points, so it has to
 *  create one if you ask for it.  It uses @a -d*N, the point of the line that is closest to the
 *  origin.  Mind you that this one probably will not be the same as the original one, totally
 *  different even!  Of course, if you used a cartesian equation to construct the line (without any
 *  support point), then all three models need to generate a support point at some point: @a -d*N.
 *  In that case, all three models @e will return the same support point.
 *
 *  @par const TVector direction():
 *  get the direction vector of the line.  almost same story as for @c support().  If you've created
 *  the line by a direction vector ot two points (which leads to the direction vector), then models
 *  Line2DParametric and Line2DCombined will give you back these originals
 *  (for Unnormalized lines only!  but in case of Normalized lines, they still correspond with the
 *  original directions).  Line2DCartesian has to regenerate them, but it will result in the same
 *  direction vector as the others.
 *
 *  @par void getCartesian(TVector& oNormal, TReference oD):
 *  gets normal vector and d-value for the cartesian equation.  Now, this is the first one
 *  Line2DCartesian has aboard itself.  Line2DCombined also has it, but now it's up to
 *  Line2DParametric to generate some stuff :)  Though, it's less worse than the other way around.
 *  This is because the parametric equation contains more information on the support point, and
 *  doesn't have to 'invent' anything like Line2DCartesian has to.  Actually, in theory, all three
 *  models should return the same normal and d-value.  In practice however, we have to deal with
 *  numerical imprecisions.  so the result of Line2DParametric can differ a little in the last bits.
 *  not a big difference, but enough to be inequal.
 *
 *  @par const TVector normal():
 *  returns only the normal vector of the cartesian equation.  For Line2DParametric we have again
 *  the same remark as for getDirections and getReciprocals: it uses getCartesian anyway, so that
 *  might be faster if you both need the normal and d-value.
 *
 *  @par TValue d():
 *  same as normal(), but returns only the value @a d instead of the normal vector.
 *
 *  @subsection methods_cartesian cartesian methods
 *
 *  So far the accessors.  let's get to cooler stuff.  For most of this stuff, we need the cartesian
 *  equation.  Line2DCartesian and Line2DCombined have it on board, but Line2DParametric will
 *  have to generate it each call.  Keep that in mind!
 *
 *  @par Side classify(const TPoint& iPoint):
 *  tells at which side of the line we can find the point iPoint: the front (sFront), the back
 *  (sBack), or right on the surface (sSurface). the front is the side where the normal sticks or
 *  points to.  The back is the other one. iPoint is exactly one the surface if @a N.iPoint+d==0.
 *  Since we need the parametric equation, Line2DParametric might have a performance hit here.
 *
 *  @par TValue equation(const TPoint& iPoint):
 *  fills in the point @a iPoint in the cartesian equation and returns the resutl.  @a N.iPoint+d will
 *  usually not be equal to zero (it's only zero for points on the line).  This method returns what
 *  it is equal to. i.e. it returns @a N.iPoint+d.  For Normalized lines this is the same as the
 *  distance of @a iPoint to the line, but not for Unnormalized lines, keep that in mind!  If you
 *  need the distance, used @c signedDistances() as described below.  Again you might have a
 *  performance hit for the Line2DParametric model because of the need of the cartesian equation.
 *
 *  @par TValue signedDistance(const TPoint& iPoint):
 *  returns the distance of the point to the line, but signed. it will be positive for points in
 *  front of the line, and negative for the ones in the back (also see: @c classify()).  The
 *  real (absolute) distances is simply the absolute value of the result.  For Normalized lines
 *  @c signedDistances() will be equal to @c equation().  But for Unnormalized lines
 *  signedDistances still divides through the normal's length Again performance hit for
 *  Line2DParametric because of the need of the cartesian equation.
 *
 *  @par TVector reject(const TPoint& iPoint):
 *  returns rejection of @a iPoint by the line.  This is a bit tricky to explain.  If you have a
 *  support point @a S, then this rejection is the part of @a iPoint-S that is parallel to the normal
 *  vector (or orthogonal to the line).  This would be the same as the rejection of @a iPoint-S
 *  (given by @c reject(iPoint-S), see below). But more descriptive might be: it is the vector you
 *  have to add to the projection of this point on the line (given by @c project(iPoint), see below),
 *  to get back @a iPoint: @c iPoint==project(iPoint)+reject(iPoint). Again performance hit for
 *  Line2DParametric because of the cartesian equation.
 *
 *  @par TVector reject(const TVector& iVector):
 *  returns rejection of @a iVector by the line.  This is already somewhat easier.  the rejection of
 *  @a iVector is that part of @a iVector that is parallel to the normal vector of the line.  You can
 *  also say that it is the orthogonal projection of @a iVector on the normal vector.  Or the part of
 *  iVector that is orthogonal to the line.   Again performance hit for Line2DParametric because
 *  of the cartersian equation.
 *
 *  @par TPoint project(const TPoint& iPoint):
 *  return the orthogonal projection of @a iPoint on the line.  It is the point on the line that is
 *  the closest one to @a iPoint.  If you draw a line through @a iPoint parallel to the normal vector,
 *  this projection is the point where this line intersects the line.  It is know that in theory
 *  @c iPoint==project(iPoint)+reject(iPoint).  Again performance hit for Line2DParametric because
 *  of the cartesian equation.
 *
 *  @par TVector project(const TVector& iVector):
 *  return the orthogonal projection of @a iVector on the line.  It is the part of @a iVector that is
 *  parallel to the line, or orthogonal to the normal vector.  It is known that in theory
 *  @c iVector==project(iVector)+reject(iVector).  Again performance hit for Line2DParametric because
 *  of the cartesian equation.
 *
 *  @par TPoint reflect(const TPoint& iPoint):
 *  return the reflection of @a iPoint in the line.  It is the point at the same distance of the
 *  line, but at the exact opposite side of the line.  If you draw a line through iPoint parallel
 *  to the normal vector, it's the only other point on that line that is at the same (absolute)
 *  distance of the line. If you walk from iPoint to the intersection point of the line and the
 *  line, and you walk further the same distance again, you arrive at reflection of iPoint.  It is
 *  known that in theory @c reflect(iPoint)==project(iPoint)-reject(iPoint).  Again performance hit
 *  for Line2DParametric because of the cartesian equation.
 *
 *  @par TVector reflect(const TVector& iVector):
 *  return the reflection of @a iVector in the line. It's the vector of which the orthogonal part to
 *  the line is flipped.  It is know that @c reflect(iVector)==project(iVector)-reject(iVector).
 *  Again performance hit for Line2DParametric because of the cartesian equation.
 *
 *  @subsection methods_parametric parametric methods
 *
 *  So far functions for the cartesian boys.  Now some stuff for parametric fellows. It's about how
 *  we can get a point of the line if we now its parametr @a t, and how we can find @t if we know
 *  the point on the line.
 *
 *  @par TPoint point(TParam iT):
 *  returns a point of the parametric equation @c P=S+iT*U.  In case of
 *  Line2DCartesian, we have the same remarks as for @a direction(): not only we have
 *  a performance hit, we probably also have to deal with totally different direction vectors than
 *  the ones we have put in the constructor.
 *
 *  @par TValue t(const TPoint& iPoint):
 *  returns a value @a t so that @c iPoint==S+t*U.  In theory, if you put this back in @c point(),
 *  you should end up with the projection of @a iPoint: @c point(t(iPoint))==project(iPoint) ?
 *  Well, this is not totally true. In practice, numerical imprecisions will probably give you a
 *  slightly different result.  You'll be very close, but the last bits will differ enough to make the
 *  them inequal.  But with some epsilons, you'll be alright.
 *
 *  @subsection misc_methods misc. methods
 *
 *  @par void flip():
 *  flips the line so that the front becomes the back and the back becomes the front.  For the
 *  cartesian equation, this is done by negating the normal and d: @c N=-N and @c d=-d.
 *  Of the parametric equation, direction vector U is flipped: @c U=-U.
 *
 *  @par bool isValid():
 *  returns true if the interal data represents a valid line, and false if not.  A line is valid
 *  if none of the direction vectors nor the normal vector are zero vectors.  And above of that,
 *  the direction vectors may not be colinear. Of course, we only test internal data.  We will not
 *  generate direction vectors (in case of Line2DCartesian), just to test the if they are valid.
 *  We can safely do that, because we know that if the normal vector is valid, then the generated
 *  directions will be too.
 *
 *  @subsection free functions
 *
 *  There are some free functions listed below: distances and intersections.
 *  They are common for all lines and can handle mixed equation policies (two lines with different
 *  equation policies).
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_2D_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_LINE_2D_H

#include "prim_common.h"
#include "point_2d.h"
#include "equation_policy.h"
#include "normalizing_policy.h"
#include "impl/line_2d_impl.h"


namespace lass
{
namespace prim
{

template
<
	typename T,
	class EquationPolicy = Cartesian,
	class NormalizingPolicy = Normalized
>
class Line2D: public impl::Line2DImpl<T, EquationPolicy, NormalizingPolicy>::Type
{
public:
	typedef Line2D<T, EquationPolicy, NormalizingPolicy> TSelf;
	typedef typename impl::Line2DImpl<T, EquationPolicy, NormalizingPolicy>::Type TImpl;

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
		typedef Line2D<U, EquationPolicy, NormalizingPolicy> Type;
	};

	Line2D();
	Line2D(const TPoint& iSupport, const TPoint& iPoint);
	Line2D(const TPoint& iSupport, const TVector& iDir);
	Line2D(const TVector& iNormal, const TPoint& iSupport);
	Line2D(const TVector& iNormal, TParam iD);

	Side classify(const TPoint& iPoint) const;
	const TValue signedDistance(const TPoint& iPoint) const;
	const TValue squaredDistance(const TPoint& iPoint) const;
	Side classify(const TPoint& iPoint, TParam iRelativeTolerance) const;
	const TValue signedDistance(const TPoint& iPoint, TParam iRelativeTolerance) const;
	const TValue squaredDistance(const TPoint& iPoint, TParam iRelativeTolerance) const;

	/*
	typedef NormalizingPolicy TNormalizingPolicy;

	typedef Point2D<T> TPoint;
	typedef TPoint::TVector TVector;

	typedef TPoint::TValue TValue;
	typedef TPoint::TParam TParam;
	typedef TPoint::TReference TReference;
	typedef TPoint::TConstReference TConstReference;
	typedef TPoint::TNumTraits TNumTraits;

	enum { dimension = TPoint::dimension };

	const TPoint support() const;
	const TVector direction() const;

	void getCartesian(TVector& oNormal, TReference oD) const;
	const TVector& normal() const;
	TConstReference d() const;

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

	TPoint point(TParam iT) const;
	TValue t(const TPoint& iPoint) const;

	void flip();
	bool isValid() const;
	*/
};



template <typename T, class EP, class NP>
T signedDistance(const Point2D<T>& iA, const Line2D<T, EP, NP>& iB);

template <typename T, class EPa, class NPa, class EPb, class NPb>
T signedDistance(const Line2D<T, EPa, NPa>& iA, const Line2D<T, EPb, NPb>& iB);

template <typename T, class EPa, class NPa, class EPb, class NPb>
Result intersect(const Line2D<T, EPa, NPa>& iA, const Line2D<T, EPb, NPb>& iB,
				 T& oTa, T& oTb);

template <typename T, class EPa, class NPa, class EPb, class NPb>
Result intersect(const Line2D<T, EPa, NPa>& iA, const Line2D<T, EPb, NPb>& iB,
				 Point2D<T>& oPoint);

template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Line2D<T, Cartesian>& iLine);
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Line2D<T, Parametric>& iLine);
template <typename T>
io::XmlOStream& operator<<(io::XmlOStream& ioOStream, const Line2D<T, Combined>& iLine);



}

}

#include "line_2d.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_PRIM_RAY_2D_H
#	include "line_2d_ray_2d.h"
#endif

#endif

// --- END OF FILE ------------------------------------------------------------------------------
