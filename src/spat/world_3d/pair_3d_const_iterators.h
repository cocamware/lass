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



/** @class lass::spat::world_3d::Pair3DConstIteratorBase
 *  @brief pure virtual base class of the const pair3 iterators.
 *  @author Bram de Greve --- BdG ---
 *  @date 2002-2003
 */


/** @class lass::spat::world_3d::Pair3DConstClockIterator
 *  @brief iterator to run through const lass::spat::world_3d::TPair's around 
 *         lass::spat::world_3d::Face3 in clockwise order
 *  @author Bram de Greve --- BdG ---
 *  @date 2002-2003
 */


/** @class lass::spat::world_3d::Pair3DConstCounterClockIterator
 *  @brief iterator to run through const lass::spat::world_3d::TPair's around 
 *		   lass::spat::world_3d::Face3 in counterclockwise order.
 *  @author Bram de Greve --- BdG ---
 *  @date 2002-2003
 */


/** @class lass::spat::world_3d::Pair3DConstSpinIterator
 *  @brief iterator to run through const lass::spat::world_3d::TPair's around 
 *         lass::spat::world_3d::Edge3 in spin order.
 *  @author Bram de Greve --- BdG ---
 *  @date 2002-2003
 */


/** @class lass::spat::world_3d::Pair3DConstCounterSpinIterator
 *  @brief iterator to run through const lass::spat::world_3d::TPair's around 
 *         lass::spat::world_3d::Edge3 in counter spin order.
 *  @author Bram de Greve --- BdG ---
 *  @date 2002-2003
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_PAIR_3D_CONST_ITERATORS_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_PAIR_3D_CONST_ITERATORS_H



#include "../spat_common.h"
#include "world_3d.h"



namespace lass
{
namespace spat
{
namespace world_3d
{

template <typename T>
class Pair3DConstIteratorBase
{
public:

	typedef World3D<T> TWorld;
	typedef typename World3D<T>::TPair TPair;
	typedef typename World3D<T>::TCell TCell;
	typedef typename World3D<T>::TPoint TPoint;

    // STRUCTORS

    Pair3DConstIteratorBase();
    Pair3DConstIteratorBase(const TPair* iBeginPair, 
                           Polarity iPolarity = positive);
    virtual ~Pair3DConstIteratorBase();

    // METHODS

    void reset(const TPair* iBeginPair, Polarity iPolarity = positive);
    virtual void reset(const TPair* iBeginPair, const TCell* iCell) = 0;
    virtual void reset(const TPair* iBeginPair, const TPoint& iPoint) = 0;
    const TPair* get() const;
    bool isEnd() const;

    const TPair* operator->() const;
    const TPair& operator*() const;
    bool operator==(const TPair* iPair) const;  
    bool operator!=(const TPair* iPair) const;

protected:

    const TPair* beginPair;
    const TPair* currentPair_;
    Polarity polarity_;
};



template <typename T>
class Pair3DConstClockIterator: public Pair3DConstIteratorBase<T>
{
public:
    
    // STRUCTORS

    Pair3DConstClockIterator();
    Pair3DConstClockIterator(const TPair* iBeginPair, Polarity iFacePolarity = positive);
    Pair3DConstClockIterator(const TPair* iBeginPair, const TCell* iCell);
    Pair3DConstClockIterator(const TPair* iBeginPair, const TPoint& iPoint);

    // METHODS

    using Pair3DConstIteratorBase<T>::reset;
    void reset(const TPair* iBeginPair, const TCell* iCell);
    void reset(const TPair* iBeginPair, const TPoint& iPoint);

    const Pair3DConstClockIterator<T>& operator++();
    Pair3DConstClockIterator<T> operator++(int);
};



template <typename T>
class Pair3DConstCounterClockIterator: public Pair3DConstIteratorBase<T>
{
public:
    
    // STRUCTORS

    Pair3DConstCounterClockIterator();
    Pair3DConstCounterClockIterator(const TPair* iBeginPair, Polarity iFacePol = positive);
    Pair3DConstCounterClockIterator(const TPair* iBeginPair, const TCell* iCell);
    Pair3DConstCounterClockIterator(const TPair* iBeginPair, const TPoint& iPoint);

    // METHODS

    using Pair3DConstIteratorBase<T>::reset;
    void reset(const TPair* iBeginPair, const TCell* iCell);
    void reset(const TPair* iBeginPair, const TPoint& iPoint);

    const Pair3DConstCounterClockIterator<T>& operator++();
    Pair3DConstCounterClockIterator<T> operator++(int);
};




template <typename T>
class Pair3DConstSpinIterator: public Pair3DConstIteratorBase<T>
{
public:
    
    // STRUCTORS

    Pair3DConstSpinIterator();
    Pair3DConstSpinIterator(const TPair* iBeginPair, Polarity iEdgePolarity = positive);
    Pair3DConstSpinIterator(const TPair* iBeginPair, const TCell* iCell);
    Pair3DConstSpinIterator(const TPair* iBeginPair, const TPoint& iPoint);

    // METHODS

    using Pair3DConstIteratorBase<T>::reset;
    void reset(const TPair* iBeginPair, const TCell* iCell);
    void reset(const TPair* iBeginPair, const TPoint& iPoint);

    const Pair3DConstSpinIterator<T>& operator++();
    Pair3DConstSpinIterator<T> operator++(int);
};



template <typename T>
class Pair3DConstCounterSpinIterator: public Pair3DConstIteratorBase<T>
{
public:
    
    // STRUCTORS

    Pair3DConstCounterSpinIterator();
    Pair3DConstCounterSpinIterator(const TPair* iBeginPair, Polarity iEdgePol = positive);
    Pair3DConstCounterSpinIterator(const TPair* iBeginPair, const TCell* iCell);
    Pair3DConstCounterSpinIterator(const TPair* iBeginPair, const TPoint& iPoint);

    // METHODS

    using Pair3DConstIteratorBase<T>::reset;
    void reset(const TPair* iBeginPair, const TCell* iCell);
    void reset(const TPair* iBeginPair, const TPoint& iPoint);

    const Pair3DConstCounterSpinIterator<T>& operator++();
    Pair3DConstCounterSpinIterator<T> operator++(int);
};



}

}

}

#include "pair_3d_const_iterators.inl"

#endif

// EOF
