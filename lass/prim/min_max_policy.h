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



/** @defgroup MinMaxPolicy MinMaxPolicy
 *  @brief Policies to specify how an AABB traits misbehaving minima and maxima.
 *  @author Bram de Greve
 *  @date 2003
 *
 *  The way an AABB handles its minima and maxima can be set by the template parameter
 *  @c MinMaxPolicy.  One policy StrictMinMax will enforce you to use correct
 *  minima and maxima, and on any suspicious behaviour, it will throw an exception.
 *  And other policy AutoMinMax will try to correct misbehaviour without your
 *  notice.  The third one is UncheckedMinMax and will ignore all errors in favour
 *  of faster code, but leading to undefined behaviour if you misbehave.
 *
 *  For more information on these policies, I refer to the documentation
 *  compagning these policies.
 *
 *  The content of the policies are rather implementation details, clients shouldn't
 *  worry about it.
 *
 *  @note these policies can be used for points of any dimension.
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_MIN_MAX_POLICY_H
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_MIN_MAX_POLICY_H

namespace lass
{
namespace prim
{

/** Exception thrown by StrictMinMax
 *  @ingroup MinMaxPolicy
 */
class MinMaxError: public util::Exception
{
public:
	MinMaxError(const std::string& msg, const std::string& loc): util::Exception(msg, loc) {}
private:
	LASS_UTIL_EXCEPTION_PRIVATE_IMPL(MinMaxError)
};



/** @ingroup MinMaxPolicy
 *  @brief MinMaxPolicy enforcing strict rules for the minima and maxima
 *  @author Bram de Greve
 *  @date 2003
 *
 *  The strict policy will throw exceptions if the client misbehaves to the AABB.
 *  This policy demands that you set correct minima and maxima, it demands that the
 *  minimum is indeed a minimum.  An very good example of misbehaviour would be to
 *  swap minimum and maximum.  But not all component values of the minimum have to
 *  be greater than the maximum.  To throw an exception, it's sufficient to have only
 *  one component value of the minimum to be greater than the corresponding component
 *  value of the maximum.  This can lead to subtle errors.  an example:
 *
 *  @code
 *  Aabb3D<int, StrictMinMax> aabb(Point3D<int>(1, 2, 3), Point3D<int>(4, 5, 6));
 *  aabb.min() = Point3D<int>(5, 5, 5);
 *  @endcode
 *
 *  The above example will throw an exception.  The first line is correct, it constructs
 *  an AABB of which the maximum is indeed larger than the minimum.  It's in the second
 *  line.  Here, we try to adjust the minimum of the AABB by setting it to 3D point
 *  (5, 5, 5).  The @a z coordinate is no problem: 5 is indeed less that 6.  The @a y
 *  coordinate isn't a problem either: 5 is not greater than 5.  It's the @a x coordinate
 *  that causes the trouble: 5 is greater than 4.  Bang!  an exception is thrown.
 *
 *  The rest of the class are implementation details for the AABB and should not be
 *  called by the client (you :).  In short we can say of this policy: make sure that
 *  minima are minima and maxima maxima.
 */
struct StrictMinMax
{
	/** set value @a oMin to @a iMin
	 *  @exception throw error if @a iMin it is in conflict with @a iMax.
	 */
	template <typename PointType>
	static void setMin(PointType& oMin, const PointType& iMax, const PointType& iMin)
	{
		if (pointwiseMin(iMin, iMax) != iMin)
		{
			LASS_THROW_EX(MinMaxError, "client tries to set invalid minimum '" << iMin
				<< "' with maximum '" << iMax << "'");
		}
		oMin = iMin;
	}

	/** set value @a oMax to @a iMax
	 *  @exception throw error if @a iMax it is in conflict with @a iMin.
	 */
	template <typename PointType>
	static void setMax(const PointType& iMin, PointType& oMax, const PointType& iMax)
	{
		if (pointwiseMin(iMin, iMax) != iMin)
		{
			LASS_THROW_EX(MinMaxError, "client tries to set invalid maximum '" << iMax
				<< "' with minimum '" << iMin << "'");
		}
		oMax = iMax;
	}

	/** check if @a iMin and @a iMax are valid minima and maxima.
	 *  @exception throw error if @a iMin and @a iMax are inconsistent.
	 */
	template <typename PointType>
	static void checkMinMax(const PointType& iMin, const PointType& iMax)
	{
		if (pointwiseMin(iMin, iMax) != iMin)
		{
			LASS_THROW_EX(MinMaxError, "inconsistent mininum '" << iMin << "' and maximum '" << iMax << "'");
		}
	}
};






/** @ingroup MinMaxPolicy
 *  @brief MinMaxPolicy automatically correcting wrong minima and maxima.
 *  @author Bram de Greve
 *  @date 2003
 *
 *  The auto policy will @e silently correct wrong min/max situations by @e squeezing or
 *  @e swapping the values until a valid situation is achieved.  How this is done, depends
 *  on the situation.  We must consider two different possibities:
 *
 *  - The easiest thing to explain is when you set both the minimum and the maximum at the
 *  same time.  It can happen that you have swapped them by accident: you provided
 *  the maximum as minimum and vice versa.  This policy will detect this and swap it again.
 *  Now the good thing is: it does this per component!  So, you can have swapped @a x and
 *  @a y coordinates and coorect @a z coordinates, it will distill a correct minimum and
 *  maximum out of this.  This is illustrated in the following example.
 *
 *  @code
 *  Aabb3D<int, AutoMinMax> aabb(Point3D<int>(4, 5, 3), Point3D<int>(1, 2, 6));
 *  LASS_EVAL(aabb);
 *
 *  // output: aabb == '(1, 2, 3)-(4, 5, 6)'
 *  @endcode
 *
 *  - The other situation is when you only set the minimumm or maximum, and you want
 *    to keep to current maximum or minimum.  In this case, we've opted for squeezing
 *    the values, rather than swapping them.  The philosophy behind it is: when you
 *    set the minimum, you want to enforce a minimum.  You don't want to be the maximum
 *    become a lesser minimum than the one you're setting, because it doesn't quite fit.
 *    No, instead of this, we squeeze or move the maximum along with the minimum.  i.e.
 *    A new maximum is chooses so that it is not less than the current maximum, but
 *    not less than the new minimum either.  It's hard to explain, but an example might
 *    illustrate this beautifully: for the @a y and @a z coordinate, there's nothing
 *    special to it: you set minimum values that are not greater than the maximum value,
 *    no misbehaviour here.  On the other hand, for @a x coordinate the new minimum 5
 *    is greater than the current maximum 4.  Instead of swapping this values so that 4
 *    becomes the new minimum, we enforce 5 as the minimum.  All that is left is to
 *    choose a new value for the maximum @a x coordinate that is greater than 4 or not
 *    less than 5.  Hmmm ... Would that be 5? :)
 *
 *  @code
 *  Aabb3D<int, AutoMinMax> aabb(Point3D<int>(1, 2, 3), Point3D<int>(4, 5, 6));
 *  aabb.min() = Point3D<int>(5, 5, 5);
 *  LASS_EVAL(aabb);
 *
 *  // output: aabb == '(5, 5, 5)-(5, 5, 6)'.
 *  @endcode
 *
 *  @note this policy will @e not correct the empty state of an AABB.  After all, in
 *        in contrary to other primitives, the empty state @e is a valid state for the
 *        AABB, so it does not have to be corrected.
 *
 *  The rest of the class are implementation details for the AABB and should not be
 *  called by the client (you :).  In short we can say of this policy: if you try to
 *  do something stupid, we try to correct it for you.
 */
struct AutoMinMax
{
	/** set value @a oMin to @a iMin and move @a ioMax (if necessary) so it stays correct.
	 */
	template <typename PointType>
	static void setMin(PointType& oMin, PointType& ioMax, const PointType& iMin)
	{
		ioMax = pointwiseMax(ioMax, iMin);
		oMin = iMin;
	}

	/** set value @a oMax to @a iMax and move @a ioMin (if necessary) so it stays correct.
	 */
	template <typename PointType>
	static void setMax(PointType& ioMin, PointType& oMax, const PointType& iMax)
	{
		ioMin = pointwiseMin(ioMin, iMax);
		oMax = iMax;
	}

	/** correct @a ioMin and @a ioMax so that they are indeed then minimum and maximum.
	 */
	template <typename PointType>
	static void checkMinMax(PointType& ioMin, PointType& ioMax)
	{
		const PointType min = pointwiseMin(ioMin, ioMax);
		ioMax = pointwiseMax(ioMin, ioMax);
		ioMin = min;
	}
};



/** @ingroup MinMaxPolicy
 *  @brief MinMaxPolicy that makes it your responsibility to behave well.
 *  @author Bram de Greve
 *  @date 2003
 *
 *  The unchecked policy is the fastest policy of the pool.  It will @e silently expects you to
 *  provide correct min/max values and ignores all invalid states caused by you.  That way it
 *  can avoid all checks and it will be faster than the other to in speed ciritical situations.
 *
 *  If you're using this policy, it's your responsibility to give the AABB correct values so it
 *  stays valid.  If you don't, you'll get what we call undefined behaviour.  Have fun :)
 *
 *  The rest of the class are implementation details for the AABB and should not be
 *  called by the client (you :).  In short we can say of this policy: if you try to
 *  do something stupid, you get undefined behaviour.
 */
struct UncheckedMinMax
{
	/** set value @a oMin to @a iMin and move @a ioMax (if necessary) so it stays correct.
	 */
	template <typename PointType>
	static void setMin(PointType& oMin, PointType& ioMax, const PointType& iMin)
	{
		oMin = iMin;
	}

	/** set value @a oMax to @a iMax and move @a ioMin (if necessary) so it stays correct.
	 */
	template <typename PointType>
	static void setMax(PointType& ioMin, PointType& oMax, const PointType& iMax)
	{
		oMax = iMax;
	}

	/** correct @a ioMin and @a ioMax so that they are indeed then minimum and maximum.
	 */
	template <typename PointType>
	static void checkMinMax(PointType& /*ioMin*/, PointType& /*ioMax*/)
	{
	}
};

}

}

#endif
