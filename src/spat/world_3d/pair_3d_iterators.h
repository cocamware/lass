/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



/** @class lass::spat::world_3d::Pair3DIteratorBase
 *  @brief pure virtual base class of the Pair3D iterators.
 *  @author Bram de Greve --- BdG ---
 *  @date 2002-2003
 */


/** @class lass::spat::world_3d::Pair3DClockIterator
 *  @brief iterator to run through lass::spat::world_3d::TPair's around 
 *         lass::spat::world_3d::Face3 in clockwise order
 *  @author Bram de Greve --- BdG ---
 *  @date 2002-2003
 */


/** @class lass::spat::world_3d::Pair3DCounterClockIterator
 *  @brief iterator to run through lass::spat::world_3d::TPair's around 
 *		   lass::spat::world_3d::Face3 in counterclockwise order.
 *  @author Bram de Greve --- BdG ---
 *  @date 2002-2003
 */


/** @class lass::spat::world_3d::Pair3DSpinIterator
 *  @brief iterator to run through lass::spat::world_3d::TPair's around 
 *         lass::spat::world_3d::Edge3 in spin order.
 *  @author Bram de Greve --- BdG ---
 *  @date 2002-2003
 */


/** @class lass::spat::world_3d::Pair3DCounterSpinIterator
 *  @brief iterator to run through lass::spat::world_3d::TPair's around 
 *         lass::spat::world_3d::Edge3 in counter spin order.
 *  @author Bram de Greve --- BdG ---
 *  @date 2002-2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_PAIR_3D_ITERATORS_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_PAIR_3D_ITERATORS_H



#include "../spat_common.h"
#include "world_3d.h"



namespace lass
{
namespace spat
{
namespace world_3d
{

template <typename T>
class Pair3DIteratorBase
{
public:

	typedef World3D<T> TWorld;
	typedef typename World3D<T>::TPair TPair;
	typedef typename World3D<T>::TCell TCell;
	typedef typename World3D<T>::TPoint TPoint;

    // STRUCTORS

    Pair3DIteratorBase();
    Pair3DIteratorBase(const TPair* iBeginPair, 
                           Polarity a_polarity = Positive);
    virtual ~Pair3DIteratorBase();

    // METHODS

    void reset(const TPair* iBeginPair, Polarity a_polarity = Positive);
    virtual void reset(const TPair* iBeginPair, const TCell* iCell) = 0;
    virtual void reset(const TPair* iBeginPair, const TPoint& iPoint) = 0;
    const TPair* get() const;
    bool isEnd() const;

    const TPair* operator->() const;
    const TPair& operator*() const;
    bool operator==(const TPair* a_pair) const;  
    bool operator!=(const TPair* a_pair) const;

protected:

    const TPair* beginPair_;
    const TPair* currentPair_;
    Polarity polarity_;
};



template <typename T>
class Pair3DClockIterator: public Pair3DIteratorBase<T>
{
public:
    
    // STRUCTORS

    Pair3DClockIterator();
    Pair3DClockIterator(const TPair* iBeginPair, Polarity iFacePolarity = Positive);
    Pair3DClockIterator(const TPair* iBeginPair, const TCell* iCell);
    Pair3DClockIterator(const TPair* iBeginPair, const TPoint& iPoint);

    // METHODS

    using Pair3DIteratorBase<T>::reset;
    void reset(const TPair* iBeginPair, const TCell* iCell);
    void reset(const TPair* iBeginPair, const TPoint& iPoint);

    const Pair3DClockIterator<T>& operator++();
    Pair3DClockIterator<T> operator++(int);
};



template <typename T>
class Pair3DCounterClockIterator: public Pair3DIteratorBase<T>
{
public:
    
    // STRUCTORS

    Pair3DCounterClockIterator();
    Pair3DCounterClockIterator(const TPair* iBeginPair, Polarity iFacePolarity = Positive);
    Pair3DCounterClockIterator(const TPair* iBeginPair, const TCell* iCell);
    Pair3DCounterClockIterator(const TPair* iBeginPair, const TPoint& iPoint);

    // METHODS

    using Pair3DIteratorBase<T>::reset;
    void reset(const TPair* iBeginPair, const TCell* iCell);
    void reset(const TPair* iBeginPair, const TPoint& iPoint);

    const Pair3DCounterClockIterator<T>& operator++();
    Pair3DCounterClockIterator<T> operator++(int);
};




template <typename T>
class Pair3DSpinIterator: public Pair3DIteratorBase<T>
{
public:
    
    // STRUCTORS

    Pair3DSpinIterator();
    Pair3DSpinIterator(const TPair* iBeginPair, Polarity iEdgePolarity = Positive);
    Pair3DSpinIterator(const TPair* iBeginPair, const TCell* iCell);
    Pair3DSpinIterator(const TPair* iBeginPair, const TPoint& iPoint);

    // METHODS

    using Pair3DIteratorBase<T>::reset;
    void reset(const TPair* iBeginPair, const TCell* iCell);
    void reset(const TPair* iBeginPair, const TPoint& iPoint);

    const Pair3DSpinIterator<T>& operator++();
    Pair3DSpinIterator<T> operator++(int);
};



template <typename T>
class Pair3DCounterSpinIterator: public Pair3DIteratorBase<T>
{
public:
    
    // STRUCTORS

    Pair3DCounterSpinIterator();
    Pair3DCounterSpinIterator(const TPair* iBeginPair, Polarity iEdgePolarity = Positive);
    Pair3DCounterSpinIterator(const TPair* iBeginPair, const TCell* iCell);
    Pair3DCounterSpinIterator(const TPair* iBeginPair, const TPoint& iPoint);

    // METHODS

    using Pair3DIteratorBase<T>::reset;
    void reset(const TPair* iBeginPair, const TCell* iCell);
    void reset(const TPair* iBeginPair, const TPoint& iPoint);

    const Pair3DCounterSpinIterator<T>& operator++();
    Pair3DCounterSpinIterator<T> operator++(int);
};



}

}

}

#include "pair_3d_iterators.inl"

#endif

// EOF
