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



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_CELL_3D_INL
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_WORLD_3D_CELL_3D_INL

#include "../spat_common.h"
#include "cell_3d.h"

namespace lass
{
namespace spat
{
namespace world_3d
{

#pragma LASS_FIXME("create a real static generator of this");
unsigned Cell3D<T>::currentVisitId_ = 0;

// --- public --------------------------------------------------------------------------------------

/** Constructor 
 */
template <typename T>
Cell3D<T>::Cell3D(const TWorld& iWorld, THandle iMedium):
    world_(iWorld),
    medium_(iMedium),
    visitId_(0)
{
}



/** Delete a cell and its unshared faces.  it will try to also delete its faces, 
 *  but it can only delete those it doesn't share with other cells if this 
 *  cell is the front cell of such a face shared with another cell, this 
 *  destructor will set the other cell as front cell.  
 */
template <typename T>
Cell3D<T>::~Cell3D()
{
    TFaces::iterator fit;
    for (fit = faces_.begin(); fit != faces_.end(); ++fit)
    {
        deleteFace(*fit);
    }
}



/** Return reference to world data structure containing this cell.
 */
template <typename T>
const Cell3D<T>::TWorld& Cell3D<T>::world() const
{
    return world_;
}



/** Return handle to cell medium.
 */
template <typename T>
Cell3D<T>::THandle Cell3D<T>::medium() const
{
    return medium_;
}



/** Add a face to this cell
 */
template <typename T>
void Cell3D<T>::addFace(TFace& iFace)
{
    faces_.push_back(&iFace);
}



/** Return the number of faces 
 */
template <typename T>
Cell3D<T>::TFaces::size_type Cell3D<T>::sizeFaces() const
{
    return faces_.size();
}



/** Return iterator to the first face.
 */
template <typename T>
Cell3D<T>::TFaces::iterator Cell3D<T>::beginFace()
{
    return faces_.begin();
}



/** Return const iterator to the first face.
 */
template <typename T>
Cell3D<T>::TFaces::const_iterator Cell3D<T>::beginFace() const
{
    return faces_.begin();
}



/** Return iterator one-past last face.
 */
template <typename T>
Cell3D<T>::TFaces::iterator Cell3D<T>::endFace()
{
    return faces_.end();
}



/** Return const iterator one-past last face.
 */
template <typename T>
Cell3D<T>::TFaces::const_iterator Cell3D<T>::endFace() const
{
    return faces_.end();
}



/** Delete a face, and return iterator to next face of cell.
 */
template <typename T>
Cell3D<T>::TFaces::iterator Cell3D<T>::eraseFace(Cell3D<T>::TFaces::iterator iFace)
{
    LASS_ASSERT(std::find(faces_.begin(), faces_.end(), *iFace) == iFace);
    deleteFace(*iFace);
    return faces_.erase(iFace);
}



/** Erase (delete) all faces of cell.
 */
template <typename T>
void Cell3D<T>::clearFaces()
{
    TFaces::iterator fit;
    for (fit = faces_.begin(); fit != faces_.end();)
    {
        fit = eraseFace(fit);
    }
}



/** Clear all pointers to the faces of the cell, does NOT DELETE the faces.
 */
template <typename T>
void Cell3D<T>::clearFacePointers()
{
    faces_.clear();
}



/** Add an object to this cell.
 */
template <typename T>
void Cell3D<T>::addObject(TObject& iObject)
{
    objects_.push_back(&iObject);
}



/** remove an pointer to an object from the cell
 *  @return - true if the cell had a pointer to the object
 *          - false if the cell didn't know the object.
 */
template <typename T>
bool Cell3D<T>::removeObject(TObject& iObject)
{
	TObjects::iterator oit = std::find(objects_.begin(), objects_.end(), &iObject);
	if (oit != objects_.end())
	{
		objects_.erase(oit);
		return true;
	}
	else
	{
		return false;
	}
}



/** Return the number of objects 
 */
template <typename T>
Cell3D<T>::TObjects::size_type Cell3D<T>::sizeObjects() const
{
    return objects_.size();
}



/** Return iterator to first object.
 */
template <typename T>
Cell3D<T>::TObjects::iterator Cell3D<T>::beginObject()
{
    return objects_.begin();
}



/** Return const iterator to first object.
 */
template <typename T>
Cell3D<T>::TObjects::const_iterator Cell3D<T>::beginObject() const
{
    return objects_.begin();
}



/** Return iterator to one past last object.
 */
template <typename T>
Cell3D<T>::TObjects::iterator Cell3D<T>::endObject()
{
    return objects_.end();
}



/** Return const iterator to one past last object.
 */
template <typename T>
Cell3D<T>::TObjects::const_iterator Cell3D<T>::endObject() const
{
    return objects_.end();
}



/** Return ID of last algorithm that visited this cell.
 */
template <typename T>
unsigned Cell3D<T>::visitId() const
{
    return visitId_;
}



/** Set the tag of the last algorithm that has been here.
 */
template <typename T>
void Cell3D<T>::setVisitId(unsigned iVisitId)
{
    visitId_ = iVisitId;
}



/** Return the barycenter of the cell.
 *  @warning this is a pseudo barycenter, calculated on base of the
 *           barycenters of its faces.  Hence, it is not the barycenter of the
 *           volume of the cell.
 */
template <typename T>
Cell3D<T>::TPointH Cell3D<T>::barycenter() const
{
    TPointH result;

    TFaces::const_iterator fit;
    for (fit = faces_.begin(); fit != faces_.end(); ++fit)
    {
        result += (*fit)->barycenter();
    }
    
    LASS_ASSERT(this->contains(result.affine()));
    return result;
}



/** Return true if the cell contains point.  This function also returns true if
 *  point lays on surface of cell.  With iSpacing you can specify how far
 *  the point must be from the cell surface.
 *  - iSpacing == 0.0: the point may lay on the surface or inside the cell,
 *                      the normal situation :)
 *  - iSpacing > 0.0: the point must lay inside the cell and at a minimum
 *                     distance of iSpacing to the surface of the cell.
 *  - iSpacing < 0.0: don't know what you'll use this for, but i'll write
 *                     the effect down anyway :)  the point may lay outside the
 *                     world as long as the distance to the surface is maximum
 *                     -iSpacing.
 */
template <typename T>
bool Cell3D<T>::contains(const TPoint& iPoint, TParam iSpacing) const
{
    TFaces::const_iterator fit;
    for (fit = faces_.begin(); fit != faces_.end(); ++fit)
    {
        TFace* face = *fit;
        const TValue result = face->plane(this).equation(iPoint);
        if (result < iSpacing)
        {
            return false;
        }
    }

    return true;
}



/** get rid of obsolete faces
 */
template <typename T>
void Cell3D<T>::eraseObsolete()
{
    TFaces tempFaces;
    TFaces::iterator fit;

    for (fit = faces_.begin(); fit != faces_.end(); ++fit)
    {
        TFace* face = *fit;
        if (face->pair() == 0)
        {
            deleteFace(face);
        }
        else
        {
            tempFaces.push_back(face);
        }
    }
    std::swap(faces_, tempFaces);
}



// --- PUBLIC STATIC METHODS ---------------------------------------------------

/** create a new unique visitId number for an algorithm so it can visit cells
 *  and doing stuff and all.
 */
template <typename T>
unsigned Cell3D<T>::newVisitID()
{
    return ++currentVisitId_;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

/** Disconnect this cell from face; if this is the only cell left connected
 *  to face, then delete face.
 *  @warning This method does not get rid of the entry in faces_.  i.e. if you
 *           call this method to remove a face from the cell, this cell will 
 *           still remember the pointer to the face, even if the face is 
 *           deleted.  That's why we keep it private.  Anyway, the way we 
 *           disconnect a cell from a face is by two possibilities: 1. there
 *           is a twinCell, then the face is still needed for the twincell and
 *           must not be deleted, we only have to remove in the face the pointer
 *           to this cell from the face. 2. there is no twinCell, then the face
 *           must be deleted, since it's not necessary for anyone anymore.
 *           Phiew! :)
 */
template <typename T>
void Cell3D<T>::deleteFace(TFace& iFace)
{
    LASS_ASSERT(iFace.link(this));
    if (iFace.twinCell(this))
    {
        iFace.replaceCell(this, 0);
    }
    else
    {
        delete &iFace;
    }
}




// --- free ----------------------------------------------------------------------------------------




}

}

}

#endif

// EOF
