/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004 Bram de Greve and Tom De Muer
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

#include "io_common.h"
#include "image.h"
#include "file_attribute.h"
#include "../stde/extended_string.h"

#include <fstream>

#define LASS_IO_IMAGE_ENFORCE_SAME_SIZE(iA, iB)\
	(*lass::util::impl::makeEnforcer<lass::util::impl::DefaultPredicate,\
									 lass::util::impl::DefaultRaiser>\
	((iA).rows() == (iB).rows() && (iA).cols() == (iB).cols(), "Images '" LASS_STRINGIFY(iA)\
	 "' and '" LASS_STRINGIFY(iB) "' have different size in '" LASS_HERE "'."))

namespace lass
{
namespace io
{

// --- public --------------------------------------------------------------------------------------

/** Default constructor.  empty image.
 */
Image::Image():
	raster_()
{
	reset();
}



/** Construct image of given width and height.
 */
Image::Image(unsigned iRows, unsigned iCols):
	raster_()
{
	reset(iRows, iCols);
}



/** Construct image from given file.
 */
Image::Image(const std::string& iFilename):
	raster_()
{
	reset(iFilename);
}



/** Copy constructor
 */
Image::Image(const Image& iOther):
	raster_()
{
	reset(iOther);
}



/** Destructor
 */
Image::~Image()
{
	reset();
}



// --- PUBLIC METHODS ----------------------------------------------------------

/** Reset image to empty image.
 */
void Image::reset()
{
	rows_ = 0;
	cols_ = 0;
	raster_.clear();
}



/** Reset image to (black) image of given width.
 */
void Image::reset(unsigned iRows, unsigned iCols)
{
	if (iRows == 0 || iCols == 0)
	{
		LASS_THROW("invalid parameters '" << iRows << ", " << iCols << "'");
	}
	const TPixel black;
	resize(iRows, iCols);
	std::fill(raster_.begin(), raster_.end(), black);
}



/** Reset image to the one in the given file.
 */
void Image::reset(const std::string& iFilename)
{
	open(iFilename);
}



/** Reset image to copy of given image.
 */
void Image::reset(const Image& iOther)
{
	if (this != &iOther)
	{
		const unsigned size = resize(iOther.rows_, iOther.cols_);
		memcpy(&raster_[0], &iOther.raster_[0], size * sizeof(TPixel));
	}
}



/** Open image from file
 */
void Image::open(const std::string& iFilename)
{
	const std::string ext = stde::tolower(fileExtension(iFilename));
	if (ext == "tga")
	{
		openTARGA(iFilename);
	}
	else
	{
		LASS_THROW("'" << iFilename << "': unknown fileformat '" << ext << "'.");
	}
}



/** Save image to file
 */
void Image::save(const std::string& iFilename)
{
	const std::string ext = stde::tolower(fileExtension(iFilename));
	if (ext == "tga")
	{
		saveTARGA(iFilename);
	}
	else
	{
		LASS_THROW("'" << iFilename << "': unknown fileformat '" << ext << ".");
	}
}



/** Copy iOther into this image.
 */
Image& Image::operator=(const Image& iOther)
{
	reset(iOther);
	return *this;
}



/** Return const pixel at position iRow, iCol.
 *  @param iRow row of pixel, y coordinate.
 *  @param iCol column of pixel, x coordinate.
 */
const Image::TPixel& Image::operator()(unsigned iRow, unsigned iCol) const
{
	LASS_ASSERT(iRow < rows_ && iCol < cols_);
	return raster_[flatIndex(iRow, iCol)];
}



/** Return reference to pixel at position iRow, iCol.
 *  @param iRow row of pixel, y coordinate.
 *  @param iCol column of pixel, x coordinate.
 */
Image::TPixel& Image::operator()(unsigned iRow, unsigned iCol)
{
	LASS_ASSERT(iRow < rows_ && iCol < cols_);
	return raster_[flatIndex(iRow, iCol)];
}



/** Return const pixel at position iRow, iCol.  position is wrapped around.
 *  @param iRow row of pixel, y coordinate.
 *  @param iCol column of pixel, x coordinate.
 */
const Image::TPixel& Image::at(signed iRow, signed iCol) const
{
	const unsigned i = num::mod(iRow, rows_);
	const unsigned j = num::mod(iCol, cols_);
	return raster_[flatIndex(iRow, iCol)];
}



/** Return reference to pixel at position iRow, iCol. position is wrapped around.
 *  @param iRow row of pixel, y coordinate.
 *  @param iCol column of pixel, x coordinate.
 */
Image::TPixel& Image::at(signed iRow, signed iCol)
{
	const unsigned i = num::mod(iRow, rows_);
	const unsigned j = num::mod(iCol, cols_);
	return raster_[flatIndex(iRow, iCol)];
}



/** Return const data block.
 */
const Image::TPixel* const Image::data() const
{
	return &raster_[0];
}



/** Return data block.
 */
Image::TPixel* const Image::data()
{
	return &raster_[0];
}



/** Return height of image.
 */
const unsigned Image::rows() const
{
	return rows_;
}



/** Return width of image.
 */
const unsigned Image::cols() const
{
	return cols_;
}



/** Return true if image is empty (no data)
 */
const bool Image::isEmpty() const
{
	return raster_.empty();
}



/** this = this over other
 */
void Image::over(const Image& iOther)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, iOther);
	std::transform(raster_.begin(), raster_.end(), iOther.raster_.begin(), raster_.begin(), prim::over);
}



/** this = this in other
 */
void Image::in(const Image& iOther)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, iOther);
	std::transform(raster_.begin(), raster_.end(), iOther.raster_.begin(), raster_.begin(), prim::in);
}



/** this = this out other
 */
void Image::out(const Image& iOther)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, iOther);
	std::transform(raster_.begin(), raster_.end(), iOther.raster_.begin(), raster_.begin(), prim::out);
}



/** this = this atop other
 */
void Image::atop(const Image& iOther)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, iOther);
	std::transform(raster_.begin(), raster_.end(), iOther.raster_.begin(), raster_.begin(), prim::atop);
}



/** this = this through other
 */
void Image::through(const Image& iOther)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, iOther);
	std::transform(raster_.begin(), raster_.end(), iOther.raster_.begin(), raster_.begin(), prim::through);
}



/** this = other over this
 */
void Image::rover(const Image& iOther)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, iOther);
	std::transform(iOther.raster_.begin(), iOther.raster_.end(), raster_.begin(), raster_.begin(), prim::over);
}



/** this = other in this
 */
void Image::rin(const Image& iOther)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, iOther);
	std::transform(iOther.raster_.begin(), iOther.raster_.end(), raster_.begin(), raster_.begin(), prim::in);
}



/** this = other out this
 */
void Image::rout(const Image& iOther)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, iOther);
	std::transform(iOther.raster_.begin(), iOther.raster_.end(), raster_.begin(), raster_.begin(), prim::out);
}



/** this = other atop this
 */
void Image::ratop(const Image& iOther)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, iOther);
	std::transform(iOther.raster_.begin(), iOther.raster_.end(), raster_.begin(), raster_.begin(), prim::atop);
}



/** this = other through this
 */
void Image::rthrough(const Image& iOther)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, iOther);
	std::transform(iOther.raster_.begin(), iOther.raster_.end(), raster_.begin(), raster_.begin(), prim::through);
}



/** this = this xor other = other xor this
 */
void Image::xor(const Image& iOther)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, iOther);
	std::transform(raster_.begin(), raster_.end(), iOther.raster_.begin(), raster_.begin(), prim::xor);
}



/** this = this plus other = other plus this
 */
void Image::plus(const Image& iOther)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, iOther);
	std::transform(raster_.begin(), raster_.end(), iOther.raster_.begin(), raster_.begin(), prim::plus);
}



/** Apply a median filter on image.
 *  @param iBoxSize size of box filter, must be odd.
 *
 *  - Filters only pixels with alphachannel != 0.
 *  - We use the vector median filter for colour images, as described on page
 *    in GAUCH J. M. (1998). Noise Removal and contrast enhancement. In:
 *    Sangwine S. J. & Horne R. E. N. (Eds.) The Colour Image Processing
 *    Handbook. London, Chapman & Hall, 149-162.
 */
void Image::filterMedian(unsigned iBoxSize)
{
	if ((iBoxSize & 0x1) == 0)
	{
		LASS_THROW("iBoxSize '" << iBoxSize << "' isn't odd as requested.");
		return;
	}

	const unsigned boxArea = iBoxSize * iBoxSize;
	const int boxRadius = (iBoxSize - 1) / 2;

	TRaster box(boxArea);
	std::vector<float> distances(boxArea);

	for (unsigned y0 = 0; y0 < rows_; ++y0)
	{
		for (unsigned x0 = 0; x0 < cols_; ++x0)
		{
			TPixel& center = raster_[y0 * cols_ + x0];
			if (center.a == TNumTraits::zero)
			{
				continue; // no filtering on pixels with alphachannel == 0
			}

			// fill filterbox

			unsigned pixelsInBox = 0;
			int candidate = -1;
			for (int dy = -boxRadius; dy <= boxRadius; ++dy)
			{
				const int y = y0 + dy;
				const int offset = y * cols_;
				if (y >= 0 && y < signed(rows_))
				{
					for (int dx = -boxRadius; dx <= boxRadius; ++dx)
					{
						const int x = x0 + dx;
						if (x >= 0 && x < signed(cols_))
						{
							const TPixel& pixel = raster_[offset + x];
							if (pixel.a != TNumTraits::zero)
							{
								box[pixelsInBox] = pixel;
								if (dx == 0 && dy == 0)
								{
									candidate = pixelsInBox;
								}
								++pixelsInBox;
							}
						}
					}
				}
			}

			// get median in filterbox
			//
			LASS_ASSERT(pixelsInBox > 0);
			LASS_ASSERT(candidate >= 0 && candidate < signed(pixelsInBox));
			unsigned i;
			for (i = 0; i < pixelsInBox; ++i)
			{
				distances[i] = 0.0;
				for (unsigned j = 0; j < pixelsInBox; ++j)
				{
					distances[i] += prim::distance(box[i], box[j]);
				}
			}

			for (i = 0; i < pixelsInBox; ++i)
			{
				if (distances[i] < distances[candidate])
				{
					candidate = i;
				}
			}

			LASS_ASSERT(candidate >= 0 && candidate < signed(pixelsInBox));
			center = box[candidate];
		}
	}
}



/** apply gamma correction to image
 */
void Image::filterGamma(TParam iGammaExponent)
{
	const TRaster::size_type size = raster_.size();
	for (TRaster::size_type i = 0; i < size; ++i)
	{
		raster_[i] = raster_[i].gammaCorrected(iGammaExponent);
	}
}



/** apply exposure to image
 */
void Image::filterExposure(TParam iExposureTime)
{
	const TRaster::size_type size = raster_.size();
	for (TRaster::size_type i = 0; i < size; ++i)
	{
		raster_[i] = raster_[i].exposed(iExposureTime);
	}
}



// --- protected -----------------------------------------------------------------------------------




// --- private -------------------------------------------------------------------------------------

/** (re)allocate data chunck
 */
unsigned Image::resize(unsigned iRows, unsigned iCols)
{
	const unsigned size = iRows * iCols;
	raster_.resize(size);
	rows_ = iRows;
	cols_ = iCols;
	return size;
}



/** Open TARGA file.
 */
void Image::openTARGA(const std::string& iFilename)
{
	std::ifstream file(iFilename.c_str(), std::ios::in | std::ios::binary);
	if (!file)
	{
		return;
	}

	HeaderTARGA header;
	file.read((char*) &header, sizeof header);

	switch (header.imageType)
	{
	case 2:
		openTARGA2(file, header);
		break;
	default:
		LASS_THROW("'" << iFilename << "': unsupported image type '" << header.imageType << "'");
		break;
	};

	file.close();
}



/** Open Type 2 TARGA file
 */
void Image::openTARGA2(std::ifstream& iFile, const HeaderTARGA& iHeader)
{
	const TValue scale(255);
	
	resize(iHeader.imageHeight, iHeader.imageWidth);
	iFile.seekg(sizeof(HeaderTARGA) + iHeader.idLength + iHeader.colorMapLength * iHeader.colorMapEntrySize);

	switch (iHeader.imagePixelSize)
	{
	case 24:
		{
		   for (unsigned y = rows_; y > 0; --y)
		   {
			  TPixel* pixel = &raster_[(y - 1) * cols_];
			  for (unsigned x = cols_; x > 0; --x)
			  {
				 num::Tuint8 data[3];
				 iFile.read((char*) &data, sizeof data);
				 pixel->r = static_cast<TValue>(data[2]) / scale;
				 pixel->g = static_cast<TValue>(data[1]) / scale;
				 pixel->b = static_cast<TValue>(data[0]) / scale;
				 pixel->a = TNumTraits::one;
				 ++pixel;
			  }
		   }
		}
		break;

	case 32:
		{
		   for (unsigned y = rows_; y > 0; --y)
		   {
			  TPixel* pixel = &raster_[(y - 1) * cols_];
			  for (unsigned x = cols_; x > 0; --x)
			  {
				 num::Tuint8 data[4];
				 iFile.read((char*) &data, sizeof data);
				 pixel->r = static_cast<TValue>(data[2]) / static_cast<TValue>(255);
				 pixel->g = static_cast<TValue>(data[1]) / static_cast<TValue>(255);
				 pixel->b = static_cast<TValue>(data[0]) / static_cast<TValue>(255);
				 pixel->a = static_cast<TValue>(data[3]) / static_cast<TValue>(255);
				 ++pixel;
			  }
		   }
		}
		break;

	default:
		LASS_THROW("unsupported pixel size '" << iHeader.imagePixelSize << "'.");
	}
}



/** Save TARGA file (type 2, 32 bit).
 */
void Image::saveTARGA(const std::string& iFilename) const
{
	const TValue scale(255);

	std::ofstream file(iFilename.c_str(), std::ios::out | std::ios::binary);
	if (!file)
	{
		return;
	}

	// STEP 1: Make a header of the right type
	HeaderTARGA header;
	header.idLength = 0;
	header.colorMapType = 0;
	header.imageType = 2;
	header.colorMapOrigin = 0;
	header.colorMapLength = 0;
	header.colorMapEntrySize = 0;
	header.imageXorigin = 0;
	header.imageYorigin = 0;
	header.imageWidth = cols_;
	header.imageHeight = rows_;
	header.imagePixelSize = 32;
	header.imageDescriptor = 0x08; // 8 attribute bits

	file.write((char*) &header, sizeof header);

	for (unsigned y = rows_; y > 0; --y)
	{
		const TPixel* pixel = &raster_[(y - 1) * cols_];
		for (unsigned x = cols_; x > 0; --x)
		{
			num::Tuint8 data[4];
			data[0] = static_cast<num::Tuint8>(pixel->b * scale);
			data[1] = static_cast<num::Tuint8>(pixel->g * scale);
			data[2] = static_cast<num::Tuint8>(pixel->r * scale);
			data[3] = static_cast<num::Tuint8>(pixel->a * scale);
			file.write((char*) &data, sizeof data);
			++pixel;
		}
	}

	file.close();
}

// --- free ----------------------------------------------------------------------------------------



}

}

// EOF
