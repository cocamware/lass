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



#ifndef LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_PRIM_TRANSFORMATION_3D_INL




#include "transformation_3d.h"

namespace lass
{

namespace prim
{

// --- public --------------------------------------------------------------------------------------

template <typename T>
Transformation3D<T>::Transformation3D()
{
    typedef TNumTraits TNT;
    v_[ 0] = TNT::one;  v_[ 1] = TNT::zero; v_[ 2] = TNT::zero; v_[ 3] = TNT::zero;
    v_[ 4] = TNT::zero; v_[ 5] = TNT::one;  v_[ 6] = TNT::zero; v_[ 7] = TNT::zero;
    v_[ 8] = TNT::zero; v_[ 9] = TNT::zero; v_[10] = TNT::one;  v_[11] = TNT::zero;
    v_[12] = TNT::zero; v_[13] = TNT::zero; v_[14] = TNT::zero; v_[15] = TNT::one;
}



template <typename T> inline
typename util::CallTraits<T>::TConstReference
Transformation3D<T>::operator()(TSize iRow, TSize iColumn) const
{
    LASS_ASSERT(iRow < 4 && iColumn < 4);
    return v_[iRow * 4 + iColumn];
}



template <typename T> inline
typename util::CallTraits<T>::TReference
Transformation3D<T>::operator()(TSize iRow, TSize iColumn) 
{
    LASS_ASSERT(iRow < 4 && iColumn < 4);
    return v_[iRow * 4 + iColumn];
}



template <typename T> inline
typename util::CallTraits<T>::TConstReference
Transformation3D<T>::at(TSize iRow, TSize iColumn) const
{
    return v_[num::mod(iRow, 4) * 4 + num::mod(iColumn, 4)];
}



template <typename T> inline
typename util::CallTraits<T>::TReference
Transformation3D<T>::at(TSize iRow, TSize iColumn)
{
    return v_[num::mod(iRow, 4) * 4 + num::mod(iColumn, 4)];
}



template <typename T> inline
Transformation3D<T>& 
Transformation3D<T>::operator*=(const Transformation3D<T>& iOther)
{
    *this = (*this * iOther);
    return *this;
}



template <typename T> inline
const typename Transformation3D<T>::TValue*
Transformation3D<T>::data() const
{
    return v_;
}



template <typename T> inline
typename Transformation3D<T>::TValue*
Transformation3D<T>::data()
{
    return v_;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename T>
Transformation3D<T>::Transformation3D(bool iDontInitialise)
{
}



// --- free ----------------------------------------------------------------------------------------

template <typename T>
Transformation3D<T> operator*(const Transformation3D<T>& iA, const Transformation3D<T>& iB)
{
    Transformation3D<T> result(false);
    for (TSize i = 0; i < 4; ++i)
    {
        for (TSize j = 0; j < 4; ++j)
        {
            result(i, j) = 
                iA(i, 0) * iB(0, j) + 
                iA(i, 1) * iB(1, j) + 
                iA(i, 2) * iB(2, j) + 
                iA(i, 3) * iB(3, j);
        }
    }
    return result;
}



template <typename T>
Vector3D<T> operator*(const Transformation3D<T>& iA, const Vector3D<T>& iB)
{
    return Point3D<T>(iA(0, 0) * iB.x + iA(0, 1) * iB.y + iA(0, 2) * iB.z,
                      iA(1, 0) * iB.x + iA(1, 1) * iB.y + iA(1, 2) * iB.z,
                      iA(2, 0) * iB.x + iA(2, 1) * iB.y + iA(2, 2) * iB.z);
}



template <typename T>
Point3D<T> operator*(const Transformation3D<T>& iA, const Point3D<T>& iB)
{
    return Point3D<T>(iA(0, 0) * iB.x + iA(0, 1) * iB.y + iA(0, 2) * iB.z + iA(0, 3),
                      iA(1, 0) * iB.x + iA(1, 1) * iB.y + iA(1, 2) * iB.z + iA(1, 3),
                      iA(2, 0) * iB.x + iA(2, 1) * iB.y + iA(2, 2) * iB.z + iA(2, 3));
}



/** @relates lass::prim::Vector3D
 */
template<typename T, typename Char, typename Traits> 
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& oOStream, 
                                             const Transformation3D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream) << "("
        << iB(0, 0) << ", " << iB(0, 1)  << ", " << iB(0, 2) << ", " << iB(0, 3) << "), "
        << iB(1, 0) << ", " << iB(1, 1)  << ", " << iB(1, 2) << ", " << iB(1, 3) << "), "
        << iB(2, 0) << ", " << iB(2, 1)  << ", " << iB(2, 2) << ", " << iB(2, 3) << "), "
        << iB(3, 0) << ", " << iB(3, 1)  << ", " << iB(3, 2) << ", " << iB(3, 3) << "))";
	return oOStream;
}



/** @relates lass::prim::Vector3D
 */
template<typename T>
io::XmlOStream& operator<<(io::XmlOStream& oOStream, const Transformation3D<T>& iB)
{
	LASS_ENFORCE_STREAM(oOStream) << "<Transformation3D>"
        << iB(0, 0) << " " << iB(0, 1)  << " " << iB(0, 2) << " " << iB(0, 3) << " "
        << iB(1, 0) << " " << iB(1, 1)  << " " << iB(1, 2) << " " << iB(1, 3) << " "
        << iB(2, 0) << " " << iB(2, 1)  << " " << iB(2, 2) << " " << iB(2, 3) << " "
        << iB(3, 0) << " " << iB(3, 1)  << " " << iB(3, 2) << " " << iB(3, 3) 
        << "</Transformation3D>\n";
	return oOStream;
}
    


}

}

#endif
