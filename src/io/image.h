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



/** @class lass::io::Image
 *  @brief image thing?
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_IMAGE_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_IMAGE_H

#include "io_common.h"

#include "../prim/color_rgba.h"

namespace lass
{

// new interfaces

namespace io
{

class LASS_DLL_EXPORT Image
{
public:

	typedef prim::ColorRGBA TPixel;
	typedef std::vector<TPixel> TRaster;

	typedef prim::ColorRGBA::TValue TValue;
	typedef prim::ColorRGBA::TParam TParam;
	typedef prim::ColorRGBA::TNumTraits TNumTraits;

    class ConstRow;
    class RefRow;

    // STRUCTORS

    Image();
    Image(unsigned iRows, unsigned iCols);
    Image(const std::string& iFilename);
    Image(const Image& iOther);
    ~Image();


    // METHODS

    void reset();
    void reset(unsigned iRows, unsigned iCols);
    void reset(const std::string& iFilename);
    void reset(const Image& iOther);

    void open(const std::string& iFilename);
    void save(const std::string& iFilename);

    Image& operator=(const Image& iOther);

    const TPixel& operator[](unsigned iFlatIndex) const { return raster_[iFlatIndex]; }
    TPixel& operator[](unsigned iFlatIndex) { return raster_[iFlatIndex]; }

	const TPixel& operator()(unsigned iRow, unsigned iCol) const;
	TPixel& operator()(unsigned iRow, unsigned iCol);

	const TPixel& at(signed iRow, signed iCol) const;
    TPixel& at(signed iRow, signed iCol);

	const TPixel* const data() const;
	TPixel* const data();

    const unsigned rows() const;
    const unsigned cols() const;
    const bool isEmpty() const;

    // FILTERS

    void filterMedian(unsigned iBoxSize);
    void filterGamma(TParam iGamma);

private:

    struct HeaderTARGA
    {
		num::Tuint8  idFieldLength;
        num::Tuint8  colMapType;
        num::Tuint8  imgType; 
        num::Tuint16 colMapOrigin;
        num::Tuint16 colMapLength;
        num::Tuint8  colMapEntrySize;
        num::Tuint8  imgXorigin;
        num::Tuint8  imgYorigin;
        num::Tuint16 imgWidth;
        num::Tuint16 imgHeight;
        num::Tuint8  imgPixelSize;
        num::Tuint8  imgDescriptor;

        unsigned numAttributeBits() const { return imgDescriptor & 0x0F; }
        unsigned screenOrigin() const { return (imgDescriptor & 0x10) >> 5; }
        unsigned interleavingFlag() const { return (imgDescriptor&0x60) >> 6; }
    };


    // PRIVATE METHODS

    unsigned resize(unsigned iRows, unsigned iCols);
    unsigned flatIndex(unsigned iRows, unsigned iCols) const { return iRows * cols_ + iCols; }
    void openTARGA(const std::string& iFilename);
    void openTARGA2(std::ifstream& iFile, const HeaderTARGA& iHeader);
    void saveTARGA(const std::string& iFilename) const;


    // PRIVATE DATA

    unsigned rows_;
    unsigned cols_;
	TRaster raster_;
};



}

}

#endif

// EOF
