/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2004
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_PAIR_3D_CONST_ITERATORS_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_PAIR_3D_CONST_ITERATORS_INL

#include "../spat_common.h"
#include "pair_3d_const_iterators.h"
#include "pair3.h"

namespace lass
{
namespace spat
{
namespace world_3d
{

// --- BASE ITERATOR -----------------------------------------------------------

/** Default constructor 
 */
template <typename T>
Pair3DConstIteratorBase<T>::Pair3DConstIteratorBase():
    beginPair_(0),
    currentPair_(0),
    polarity_(positive)
{
}



/** Constructor.
 */
template <typename T>
Pair3DConstIteratorBase<T>::Pair3DConstIteratorBase(const TPair* iBeginPair, 
                                                    Polarity iPolarity):
    polarity_(0)
{
    reset(iBeginPair, iPolarity);
}



/** Virtual Destructor.
 */
template <typename T>
Pair3DConstIteratorBase<T>::~Pair3DConstIteratorBase()
{
}



/** Reset iterator to iterate around a face starting at iBeginPair,
 *  with correct polarity by iPolarity.
 */
template <typename T>
void Pair3DConstIteratorBase<T>::reset(const TPair* iBeginPair, Polarity iPolarity)
{
    beginPair_ = iBeginPair;
    currentPair_ = iBeginPair;
    polarity_ = iPolarity;
}



/** Return pointer to current pair.
 */
template <typename T>
const TPair* Pair3DConstIteratorBase<T>::get() const
{
    return currentPair_;
}



/** Return true if iterator is done, beyond last pair.
 */
template <typename T>
bool Pair3DConstIteratorBase<T>::isEnd() const
{
    return currentPair_ == 0;
}



/** Return pointer to current pair.
 */
template <typename T>
const TPair* Pair3DConstIteratorBase<T>::operator->() const
{
    return get();
}



/** Return reference to current pair.
 */
template <typename T>
const TPair& Pair3DConstIteratorBase<T>::operator*() const
{
    LASS_ASSERT(get());
    return *get();
}



/** Return true if iterator points to iPair.
 */
template <typename T>
bool Pair3DConstIteratorBase<T>::operator==(const TPair* iPair) const
{
    return currentPair_ == iPair;
}



/** Return true if iterator points to iPair.
 */
template <typename T>
bool Pair3DConstIteratorBase<T>::operator!=(const TPair* iPair) const
{
    return currentPair_ != iPair;
}



// --- CLOCK ITERATOR ----------------------------------------------------------

/** Default constructor 
 */
template <typename T>
Pair3DConstClockIterator<T>::Pair3DConstClockIterator():
    Pair3DConstIteratorBase()
{
}



/** Constructor.
 */
template <typename T>
Pair3DConstClockIterator<T>::Pair3DConstClockIterator(const TPair* iBeginPair, 
                                                      Polarity iFacePolarity):
    Pair3DConstIteratorBase(iBeginPair, iFacePolarity)
{
}



/** Construct iterator to clock around a face starting at iBeginPair,
 *  with correct polarity by iCell.
 */
template <typename T>
Pair3DConstClockIterator<T>::Pair3DConstClockIterator(const TPair* iBeginPair, 
                                                      const Cell3* iCell)
{
    reset(iBeginPair, iCell);
}



/** Construct iterator to clock around a face starting at iBeginPair,
 *  with correct polarity by iPoint.
 */
template <typename T>
Pair3DConstClockIterator<T>::Pair3DConstClockIterator(const TPair* iBeginPair, 
                                                      const Point3& iPoint)
{
    reset(iBeginPair, iPoint);
}



/** Reset iterator to clock around a face starting at iBeginPair,
 *  with correct polarity by iCell.
 */
template <typename T>
void Pair3DConstClockIterator<T>::reset(const TPair* iBeginPair, const Cell3* iCell)
{
    Pair3DConstIteratorBase<T>::reset(iBeginPair, iBeginPair->facePolarity(iCell));
}



/** Reset iterator to clock around a face starting at iBeginPair,
 *  with correct polarity by iPoint.
 */
template <typename T>
void Pair3DConstClockIterator<T>::reset(const TPair* iBeginPair, const Point3& iPoint)
{
    Pair3DConstIteratorBase<T>::reset(iBeginPair, iBeginPair->facePolarity(iPoint));
}



/** Prefix clock.  clock and return result.
 */
template <typename T>
const Pair3DConstClockIterator& Pair3DConstClockIterator<T>::operator++()
{
    if (currentPair_)
    {
        currentPair_ = currentPair_->clock(polarity_);
        if (currentPair_ == beginPair_)
        {
            currentPair_ = 0;
        }
    }

    return *this;
}



/** Postfix clock.  clock and return situation before clock.
 */
template <typename T>
Pair3DConstClockIterator Pair3DConstClockIterator<T>::operator++(int)
{
    Pair3DConstClockIterator temp = *this;
    ++(*this);
    return temp;
}



// --- COUNTERCLOCK ITERATOR ---------------------------------------------------

/** Default constructor 
 */
template <typename T>
Pair3DConstCounterClockIterator<T>::Pair3DConstCounterClockIterator():
    Pair3DConstIteratorBase()
{
}



/** Constructor.
 */
template <typename T>
Pair3DConstCounterClockIterator<T>::Pair3DConstCounterClockIterator(const TPair* iBeginPair,
                                                                    Polarity iFacePolarity):
    Pair3DConstIteratorBase(iBeginPair, iFacePolarity)
{
}



/** Construct iterator to counter clock around a face starting at iBeginPair,
 *  with correct polarity by iCell.
 */
template <typename T>
Pair3DConstCounterClockIterator<T>::Pair3DConstCounterClockIterator(const TPair* iBeginPair, 
															        const Cell3* iCell)
{
    reset(iBeginPair, iCell);
}



/** Construct iterator to counter clock around a face starting at iBeginPair,
 *  with correct polarity by iPoint.
 */
template <typename T>
Pair3DConstCounterClockIterator<T>::Pair3DConstCounterClockIterator(const TPair* iBeginPair, 
															        const Point3& iPoint)
{
    reset(iBeginPair, iPoint);
}



/** Reset iterator to counter clock around a face starting at iBeginPair,
 *  with correct polarity by iCell.
 */
template <typename T>
void Pair3DConstCounterClockIterator<T>::reset(const TPair* iBeginPair, const Cell3* iCell)
{
    Pair3DConstIteratorBase<T>::reset(iBeginPair, iBeginPair->facePolarity(iCell));
}



/** Reset iterator to counter clock around a face starting at iBeginPair,
 *  with correct polarity by iPoint.
 */
template <typename T>
void Pair3DConstCounterClockIterator<T>::reset(const TPair* iBeginPair, const Point3& iPoint)
{
    Pair3DConstIteratorBase<T>::reset(iBeginPair, iBeginPair->facePolarity(iPoint));
}



/** Prefix counter clock.  counter clock and return result.
 */
template <typename T>
const Pair3DConstCounterClockIterator& 
Pair3DConstCounterClockIterator<T>::operator++()
{
    if (currentPair_)
    {
        currentPair_ = currentPair_->counterClock(polarity_);
        if (currentPair_ == beginPair_)
        {
            currentPair_ = 0;
        }
    }

    return *this;
}



/** Postfix counter clock.  counter clock and return situation before 
 *  counter clock.
 */
template <typename T>
Pair3DConstCounterClockIterator Pair3DConstCounterClockIterator<T>::operator++(int)
{
    Pair3DConstCounterClockIterator temp = *this;
    ++(*this);
    return temp;
}



// --- SPIN ITERATOR -----------------------------------------------------------

/** Default constructor 
 */
template <typename T>
Pair3DConstSpinIterator<T>::Pair3DConstSpinIterator():
    Pair3DConstIteratorBase()
{
}



/** Constructor.
 */
template <typename T>
Pair3DConstSpinIterator<T>::Pair3DConstSpinIterator(const TPair* iBeginPair,
                                                    Polarity a_edgePolarity):
    Pair3DConstIteratorBase(iBeginPair, a_edgePolarity)
{
}



/** Construct iterator to iterate around an edge starting at iBeginPair,
 *  with correct polarity by iCell.
 */
template <typename T>
Pair3DConstSpinIterator<T>::Pair3DConstSpinIterator(const TPair* iBeginPair, 
                                                    const Cell3* iCell)
{
    reset(iBeginPair, iCell);
}



/** Construct iterator to iterate around an edge starting at iBeginPair,
 *  with correct polarity by iPoint.
 */
template <typename T>
Pair3DConstSpinIterator<T>::Pair3DConstSpinIterator(const TPair* iBeginPair, 
                                                    const Point3& iPoint)
{
    reset(iBeginPair, iPoint);
}



/** Reset iterator to iterate around an edge starting at iBeginPair,
 *  with correct polarity by iCell
 */
template <typename T>
void Pair3DConstSpinIterator<T>::reset(const TPair* iBeginPair, const Cell3* iCell)
{
    Pair3DConstIteratorBase<T>::reset(iBeginPair, iBeginPair->edgePolarity(iCell));
}



/** Reset iterator to spin around a face starting at iBeginPair,
 *  with correct polarity by iPoint.
 */
template <typename T>
void Pair3DConstSpinIterator<T>::reset(const TPair* iBeginPair, const Point3& iPoint)
{
    Pair3DConstIteratorBase<T>::reset(iBeginPair, iBeginPair->edgePolarity(iPoint));
}



/** Prefix spin.  Spin and return result.
 */
template <typename T>
const Pair3DConstSpinIterator& Pair3DConstSpinIterator<T>::operator++()
{
    if (currentPair_)
    {
        currentPair_ = currentPair_->spin(polarity_);
        if (currentPair_ == beginPair_)
        {
            currentPair_ = 0;
        }
    }

    return *this;
}



/** Postfix spin.  Spin and return situation before spin.
 */
template <typename T>
Pair3DConstSpinIterator Pair3DConstSpinIterator<T>::operator++(int)
{
    Pair3DConstSpinIterator temp = *this;
    ++(*this);
    return temp;
}



// --- COUNTER SPIN ITERATOR ---------------------------------------------------

/** Default constructor 
 */
template <typename T>
Pair3DConstCounterSpinIterator<T>::Pair3DConstCounterSpinIterator():
    Pair3DConstIteratorBase()
{
}



/** Constructor.
 */
template <typename T>
Pair3DConstCounterSpinIterator<T>::Pair3DConstCounterSpinIterator(const TPair* iBeginPair,
                                                                  Polarity a_edgePolarity):
    Pair3DConstIteratorBase(iBeginPair, a_edgePolarity)
{
}



/** Construct iterator to counter spin around an edge starting at iBeginPair,
 *  with correct polarity by iCell.
 */
template <typename T>
Pair3DConstCounterSpinIterator<T>::Pair3DConstCounterSpinIterator(const TPair* iBeginPair, 
															      const Cell3* iCell)
{
    reset(iBeginPair, iCell);
}



/** Construct iterator to counter spin around an edge starting at iBeginPair,
 *  with correct polarity by iPoint.
 */
template <typename T>
Pair3DConstCounterSpinIterator<T>::Pair3DConstCounterSpinIterator(const TPair* iBeginPair, 
															      const Point3& iPoint)
{
    reset(iBeginPair, iPoint);
}



/** Reset iterator to counter spin around an edge starting at iBeginPair,
 *  with correct polarity by iCell
 */
template <typename T>
void Pair3DConstCounterSpinIterator<T>::reset(const TPair* iBeginPair, const Cell3* iCell)
{
    Pair3DConstIteratorBase<T>::reset(iBeginPair, iBeginPair->edgePolarity(iCell));
}



/** Reset iterator to counter spin around a face starting at iBeginPair,
 *  with correct polarity by iPoint.
 */
template <typename T>
void Pair3DConstCounterSpinIterator<T>::reset(const TPair* iBeginPair, const Point3& iPoint)
{
    Pair3DConstIteratorBase<T>::reset(iBeginPair, iBeginPair->edgePolarity(iPoint));
}



/** Prefix counter spin.  Counter spin and return result.
 */
template <typename T>
const Pair3DConstCounterSpinIterator& Pair3DConstCounterSpinIterator<T>::operator++()
{
    if (currentPair_)
    {
        currentPair_ = currentPair_->counterSpin(polarity_);
        if (currentPair_ == beginPair_)
        {
            currentPair_ = 0;
        }
    }

    return *this;
}



/** Postfix counter spin.  Counter spin and return situation before 
 *  counter spin.
 */
template <typename T>
Pair3DConstCounterSpinIterator Pair3DConstCounterSpinIterator<T>::operator++(int)
{
    Pair3DConstCounterSpinIterator temp = *this;
    ++(*this);
    return temp;
}



}

}

}

#endif

// EOF
