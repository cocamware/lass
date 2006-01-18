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
#include "binary_i_file.h"
#include "binary_o_file.h"
#include "file_attribute.h"
#include "../stde/extended_string.h"
#include "../stde/extended_algorithm.h"
#include "../stde/range_algorithm.h"
#include "../stde/static_vector.h"

#define LASS_IO_IMAGE_ENFORCE_SAME_SIZE(a, b)\
	LASS_UTIL_IMPL_MAKE_ENFORCER(\
		::lass::util::impl::DefaultPredicate,\
		::lass::util::impl::DefaultRaiser,\
		((a).rows() == (b).rows() && (a).cols() == (b).cols()),\
		"Images '" LASS_STRINGIFY(a) "' and '" LASS_STRINGIFY(b) "' have different size in '" LASS_HERE "'.")

namespace lass
{
namespace io
{
namespace impl
{
	class Bytes4
	{
	public:
		const num::Tuint8 operator[](std::size_t k) const { LASS_ASSERT(k < 4); return values_[k]; }
		num::Tuint8& operator[](std::size_t k) { LASS_ASSERT(k < 4); return values_[k]; }
		const num::Tuint8* get() const { return values_; }
		num::Tuint8* get() { return values_; }
		bool operator==(const Bytes4& iOther) const { return std::equal(values_, values_ + 4, iOther.values_); }
	private:
		num::Tuint8 values_[4];
	};
}

Image::TFileFormats Image::fileFormats_ = Image::fillFileFormats();
num::Tuint32 Image::magicLass_ = 0x7373616c; // 'lass' in little endian
std::string Image::magicRadiance_ = "#?RADIANCE\n";

// --- public --------------------------------------------------------------------------------------

/** Default constructor.  empty image.
 */
Image::Image():
	rows_(0),
	cols_(0),
	raster_()
{
}



/** Construct image of given width and height.
 */
Image::Image(unsigned iRows, unsigned iCols):
	rows_(0),
	cols_(0),
	raster_()
{
	const TPixel black;
	resize(iRows, iCols);
	std::fill(raster_.begin(), raster_.end(), black);
}



/** Construct image from given file.
 */
Image::Image(const std::string& iFilename):
	rows_(0),
	cols_(0),
	raster_()
{
	open(iFilename);
}



/** Copy constructor
 */
Image::Image(const Image& iOther):
	rows_(iOther.rows_),
	cols_(iOther.cols_),
	raster_(iOther.raster_)
{
}



/** Destructor
 */
Image::~Image()
{
}



// --- PUBLIC METHODS ----------------------------------------------------------

/** Reset image to empty image.
 */
void Image::reset()
{
	Image temp;
	swap(temp);
}



/** Reset image to (black) image of given width.
 */
void Image::reset(unsigned iRows, unsigned iCols)
{
	Image temp(iRows, iCols);
	swap(temp);
}



/** Reset image to the one in the given file.
 */
void Image::reset(const std::string& iFilename)
{
	Image temp(iFilename);
	swap(temp);
}



/** Reset image to copy of given image.
 */
void Image::reset(const Image& iOther)
{
	Image temp(iOther);
	swap(temp);
}



/** Open image from file
 */
void Image::open(const std::string& iFilename)
{
	try
	{
		BinaryIFile file(iFilename);
		if (!file)
		{
			LASS_THROW("could not open file to read.");
		}

		open(file, fileExtension(iFilename));
	}
	catch (const util::Exception& error)
	{
		LASS_THROW("'" << iFilename << "': " << error.message());
	}
}



/** Open image from binary stream
 */
void Image::open(BinaryIStream& iStream, const std::string& iFormatTag)
{
	Image temp;
	TFileOpener opener = findFormat(iFormatTag).open;
	if (!opener)
	{
		LASS_THROW("cannot open images in file format '" << iFormatTag << "'.");
	}

	(temp.*opener)(iStream);

	if (iStream.good())
	{
		swap(temp);
	}
	else if (iStream.eof())
	{
		LASS_THROW("tried to read past end of file.");
	}
	else
	{
		LASS_THROW("unknown failure in file.");
	}
}



/** Save image to file
 */
void Image::save(const std::string& iFilename)
{
	try
	{
		BinaryOFile file(iFilename);
		if (!file)
		{
			LASS_THROW("could not open file to write.");
		}

		save(file, fileExtension(iFilename));
	}
	catch (const util::Exception& error)
	{
		LASS_THROW("'" << iFilename << "': " << error.message());
	}
}



/** Save image to file
 */
void Image::save(BinaryOStream& iStream, const std::string& iFormatTag)
{
	TFileSaver saver = findFormat(iFormatTag).save;
	if (!saver)
	{
		LASS_THROW("cannot save images in file format '" << iFormatTag << "'.");
	}

	(this->*saver)(iStream);

	if (iStream.eof())
	{
		LASS_THROW("tried to read past end of file.");
	}
	if (!iStream.good())
	{
		LASS_THROW("unknown failure in file.");
	}
}



/** Copy iOther into this image.
 */
Image& Image::operator=(const Image& iOther)
{
	Image temp(iOther);
	swap(temp);
	return *this;
}



/** swap two images
 */
void Image::swap(Image& iOther)
{
	std::swap(rows_, iOther.rows_);
	std::swap(cols_, iOther.cols_);
	raster_.swap(iOther.raster_);
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
	return raster_[flatIndex(i, j)];
}



/** Return reference to pixel at position iRow, iCol. position is wrapped around.
 *  @param iRow row of pixel, y coordinate.
 *  @param iCol column of pixel, x coordinate.
 */
Image::TPixel& Image::at(signed iRow, signed iCol)
{
	const unsigned i = num::mod(iRow, rows_);
	const unsigned j = num::mod(iCol, cols_);
	return raster_[flatIndex(i, j)];
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



/** apply exposure to image
 */
void Image::filterInverseExposure(TParam iExposureTime)
{
	const TRaster::size_type size = raster_.size();
	for (TRaster::size_type i = 0; i < size; ++i)
	{
		raster_[i] = raster_[i].invExposed(iExposureTime);
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



/** Open LASS Raw file.
 */
BinaryIStream& Image::openRaw(BinaryIStream& iStream)
{
	HeaderRaw header;
	header.readFrom(iStream);
	if (!iStream || header.lass != magicLass_ || header.version != 1)
	{
		LASS_THROW("not a LASS RAW version 1 file.");
	}

	EndiannessSetter(iStream, num::littleEndian);
	resize(header.rows, header.cols);
	num::Tfloat32 r, g, b, a;
	for (TRaster::iterator i = raster_.begin(); i != raster_.end(); ++i)
	{
		iStream >> r >> g >> b >> a;
		i->r = r;
		i->g = g;
		i->b = b;
		i->a = a;
	}

	return iStream;
}



/** Open TARGA file.
 */
BinaryIStream& Image::openTarga(BinaryIStream& iStream)
{
	HeaderTarga header;
	header.readFrom(iStream);
	resize(header.imageHeight, header.imageWidth);

	switch (header.imageType)
	{
	case 2:
	case 10:
		openTargaTrueColor(iStream, header);
		break;
	default:
		LASS_THROW("unsupported image type '" << static_cast<unsigned>(header.imageType) << "'");
		break;
	};

	return iStream;
}



/** Open Type 2 and 11 TARGA file
 */
BinaryIStream& Image::openTargaTrueColor(BinaryIStream& iStream, const HeaderTarga& iHeader)
{
	const TValue scale = num::inv(255.f);

	std::size_t numBytes = 0;
	switch (iHeader.imagePixelSize)
	{
	case 24: 
		numBytes = 3; 
		break;
	case 32:
		numBytes = 4;
		break;
	default:
		LASS_THROW("image pixel size '" << iHeader.imagePixelSize << "' not supported.");
	};

	iStream.seekg(iHeader.idLength + iHeader.colorMapLength * iHeader.colorMapEntrySize, 
		std::ios_base::cur);

	LASS_ASSERT(cols_ == iHeader.imageWidth);
	std::vector<impl::Bytes4> buffer(cols_);
	for (unsigned y = rows_; y > 0; --y)
	{	
		if (iHeader.imageType == 10)
		{
			// decode rle
			//
			unsigned x = 0;
			while (x < cols_)
			{
				num::Tuint8 code;
				iStream >> code;
				const num::Tuint8 packetSize = (code & 0x7f) + 1;
				impl::Bytes4 bytes;
				if (code & 0x80)
				{
					iStream.read(bytes.get(), numBytes);
					for (num::Tuint8 i = 0; i < packetSize; ++i)
					{
						LASS_ASSERT(x < cols_);
						buffer[x++] = bytes;
					}
				}
				else
				{
					for (num::Tuint8 i = 0; i < packetSize; ++i)
					{
						iStream.read(bytes.get(), numBytes);
						LASS_ASSERT(x < cols_);
						buffer[x++] = bytes;
					}
				}
			}
		}
		else
		{
			LASS_ASSERT(iHeader.imageType == 2);
			for (unsigned x = 0; x < cols_; ++x)
			{
				iStream.read(buffer[x].get(), numBytes);
			}
		}

		// decode scanline
		//
		TPixel* scanline = &raster_[(y - 1) * cols_];
		for (unsigned x = 0; x < cols_; ++x)
		{
			const impl::Bytes4& bytes = buffer[x];
			TPixel& pixel = scanline[x];
			pixel.r = static_cast<TValue>(bytes[2]) * scale;
			pixel.g = static_cast<TValue>(bytes[1]) * scale;
			pixel.b = static_cast<TValue>(bytes[0]) * scale;
			pixel.a = numBytes == 3 ? TNumTraits::one : static_cast<TValue>(bytes[3]) * scale;
		}
	}

	return iStream;
}



BinaryIStream& Image::openRadianceHdr(BinaryIStream& iStream)
{
	HeaderRadianceHdr header;
	header.readFrom(iStream);
	if (!iStream)
	{
		LASS_THROW("syntax error in RADIANCE HDR header.");
	}

	float exponents[256];
	exponents[0] = 0.f;
	for (int i = 1; i < 256; ++i)
	{
		exponents[i] = ::ldexpf(1.f, i - 128 - 8);
	}
	float inverseCorrections[3];
	for (int i = 0; i < 3; ++i)
	{
		inverseCorrections[i] = num::inv(header.exposure * header.colorCorr[i]);
	}
	
	resize(header.height, header.width);

	const std::ptrdiff_t firstY = !header.yIncreasing ? 0 : (header.height - 1);	
	const std::ptrdiff_t lastY = !header.yIncreasing ? static_cast<std::ptrdiff_t>(header.height) : -1;	
	const std::ptrdiff_t deltaY = !header.yIncreasing ? 1 : -1;
	const std::ptrdiff_t firstX = header.xIncreasing ? 0 : (header.width - 1);
	const std::ptrdiff_t lastX = header.xIncreasing ? static_cast<std::ptrdiff_t>(header.width) : -1;
	const std::ptrdiff_t deltaX = header.xIncreasing ? 1 : -1;

	std::vector<impl::Bytes4> buffer(header.width);
	unsigned rleCount = 0;
	unsigned rleCountByte = 0;

	for (std::ptrdiff_t y = firstY; y != lastY; y += deltaY)
	{
		// unpack scanline to buffer
		//
		for (std::ptrdiff_t x = firstX; x != lastX; /* increment in loop */ )
		{
			impl::Bytes4 rgbe;
			iStream.read(rgbe.get(), 4);
			if (rgbe[0] == 2 && rgbe[1] == 2 && (rgbe[2] & 0x80) == 0)
			{
				// new rle
				//
				const unsigned lineLength = rgbe[2] * 256 + rgbe[3];
				const ptrdiff_t lastX2 = x + lineLength * deltaX;
				for (unsigned k = 0; k < 4; ++k)
				{
					ptrdiff_t x2 = x;
					while (x2 != lastX2)
					{
						num::Tuint8 spanSize = 0, value = 0;
						iStream >> spanSize;
						const bool isHomogenousSpan = spanSize > 128;
						if (isHomogenousSpan) 
						{
							spanSize -= 128;
							iStream >> value;
						}
						for (num::Tuint8 i = spanSize; i > 0; --i)
						{
							if (!isHomogenousSpan)
							{
								iStream >> value;
							}
							LASS_ASSERT(x2 != lastX2);
							buffer[x2][k] = value;
							x2 += deltaX;
						}
					}
				}
				x = lastX2;
			}
			else
			{
				// no rle or old rle
				//
				if (rgbe[0] == 1 && rgbe[1] == 1 && rgbe[2] == 1)
				{
					rleCount |= rgbe[4] << rleCountByte;
					++rleCountByte;
				}
				else
				{
					if (rleCount > 0)
					{
						for (unsigned k = rleCount - 1; k > 0; --k)
						{
							buffer[x] = rgbe;
							x += deltaX;
						}
						rleCount = 0;
						rleCountByte = 0;
					}
					buffer[x] = rgbe;
					x += deltaX;
				}
			}
		}

		// decode rgbe information
		//
		TPixel* scanline = &raster_[y * header.width];	
		for (std::ptrdiff_t x = firstX; x != lastX; x += deltaX)
		{
			const impl::Bytes4 rgbe = buffer[x];
			TPixel& pixel = scanline[x];
			const float exponent = exponents[rgbe[3]];
			for (unsigned k = 0; k < 3; ++k)
			{
				pixel[k] = rgbe[k] * inverseCorrections[k] * exponent;
			}
		}
	}	
	return iStream;
}



BinaryOStream& Image::saveRaw(BinaryOStream& oStream) const
{
	HeaderRaw header;
	header.lass = magicLass_;
	header.version = 1;
	header.rows = rows_;
	header.cols = cols_;
	header.writeTo(oStream);

	EndiannessSetter(oStream, num::littleEndian);
	num::Tfloat32 r, g, b, a;
	for (TRaster::const_iterator i = raster_.begin(); i != raster_.end(); ++i)
	{
		r = i->r;
		g = i->g;
		b = i->b;
		a = i->a;
		oStream << r << g << b << a;
	}

	return oStream;
}


/** Save TARGA file (type 2, 32 bit).
 */
BinaryOStream& Image::saveTarga(BinaryOStream& oStream) const
{
	const TValue scale(255);
	const TValue zero(0);
	const TValue one(1);

	// STEP 1: Make a header of the right type
	HeaderTarga header;
	header.idLength = 0;
	header.colorMapType = 0;
	header.imageType = 10; // rle true color
	header.colorMapOrigin = 0;
	header.colorMapLength = 0;
	header.colorMapEntrySize = 0;
	header.imageXorigin = 0;
	header.imageYorigin = 0;
	header.imageWidth = cols_;
	header.imageHeight = rows_;
	header.imagePixelSize = 32;
	header.imageDescriptor = 0x08; // 8 attribute bits

	header.writeTo(oStream);

	std::vector<impl::Bytes4> buffer(header.imageWidth);
	std::vector<impl::Bytes4> rleBuffer(128);
	for (unsigned y = rows_; y > 0; --y)
	{
		unsigned x;

		// encode in scanline buffer
		//
		const TPixel* scanline = &raster_[(y - 1) * cols_];
		for (x = 0; x < cols_; ++x)
		{
			const TPixel& pixel = scanline[x];
			impl::Bytes4& bytes = buffer[x];
			bytes[0] = static_cast<num::Tuint8>(num::clamp(pixel.b, zero, one) * scale);
			bytes[1] = static_cast<num::Tuint8>(num::clamp(pixel.g, zero, one) * scale);
			bytes[2] = static_cast<num::Tuint8>(num::clamp(pixel.r, zero, one) * scale);
			bytes[3] = static_cast<num::Tuint8>(num::clamp(pixel.a, zero, one) * scale);
		}

		// run-length encode buffer
		//
		unsigned totalLength = 0;
		num::Tuint8 numDiff = 0;
		x = 0;
		while (x < cols_)
		{
			const impl::Bytes4& bytes = buffer[x];
			unsigned x2 = x;
			num::Tuint8 numSame = 0;
			while (x2 < cols_ && numSame < 128 && buffer[x2] == bytes)
			{
				++x2;
				++numSame;
			}
			if (numSame == 1)
			{
				rleBuffer[numDiff] = bytes;
				++numDiff;
				++x;
			}
			if (numDiff == 128 || ((numSame > 1 || x == cols_) && numDiff > 0))
			{
				oStream << static_cast<num::Tuint8>(numDiff - 1);
				for (num::Tuint8 i = 0; i < numDiff; ++i)
				{
					oStream.write(&rleBuffer[i], 4);
				}
				totalLength += numDiff;
				LASS_ASSERT(totalLength == x);
				numDiff = 0;
			}
			if (numSame > 1)
			{
				oStream << static_cast<num::Tuint8>((numSame - 1) | 0x80);
				oStream.write(bytes.get(), 4);
				x = x2;
				totalLength += numSame;
				LASS_ASSERT(totalLength == x);
				numSame = 0;
			}
		}
		LASS_ASSERT(totalLength == cols_);
	}

	return oStream;
}


/** Save RADIANCE HDR
 */
BinaryOStream& Image::saveRadianceHdr(BinaryOStream& oStream) const
{
	if (cols_ > 0x7fff)
	{
		LASS_THROW("cannot save this as RADIANCE HDR, because image is too wide");
	}
	HeaderRadianceHdr header;
	header.height = rows_;
	header.width = cols_;
	header.yIncreasing = false;
	header.xIncreasing = true;
	header.writeTo(oStream);

	std::vector<impl::Bytes4> buffer(cols_);
	std::vector<num::Tuint8> rleBuffer(128);

	for (unsigned y = 0; y < rows_; ++y)
	{
		// first, transform a line of rgb pixels to rgbe reprentation
		//
		const TPixel* line = &raster_[y * cols_];
		for (unsigned x = 0; x < cols_; ++x)
		{
			const TPixel& pixel = line[x];
			impl::Bytes4& rgbe = buffer[x];
			const float maximum = std::max(std::max(pixel.r, pixel.g), pixel.b);
			if (maximum < 1e-32)
			{
				rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
			}
			else
			{
				int exponent;
				const float mantissa = ::frexpf(maximum, &exponent);
				for (unsigned k = 0; k < 3; ++k)
				{
					const float normalized = pixel[k] * (256.f * mantissa / maximum);
					rgbe[k] = static_cast<num::Tuint8>(num::clamp(normalized, 0.f, 255.f));
				}
				rgbe[3] = static_cast<num::Tuint8>(num::clamp(exponent + 128, 0, 255));
			}
		}

		// rle encode each channel
		//
		num::Tuint8 bytes[4];
		bytes[0] = 2;
		bytes[1] = 2;
		bytes[2] = (cols_ & 0x7f00) >> 8;
		bytes[3] = cols_ & 0xff;
		oStream.write(bytes, 4);

		for (unsigned k = 0; k < 4; ++k)
		{
			num::Tuint8 numDiff = 0;
			unsigned x = 0;
			while (x < cols_)
			{
				num::Tuint8 value = buffer[x][k];
				unsigned x2 = x;
				num::Tuint8 numSame = 0;
				while (x2 < cols_ && numSame < 127 && buffer[x2][k] == value)
				{
					++x2;
					++numSame;
				}
				if (numSame > 2)
				{
					if (numDiff > 0)
					{
						oStream << numDiff;
						oStream.write(&rleBuffer[0], numDiff);
						numDiff = 0;
					}
					numSame += 128;
					oStream << numSame << value;
					x = x2;
				}
				else
				{
					if (numDiff == 128)
					{
						oStream << numDiff;
						oStream.write(&rleBuffer[0], numDiff);
						numDiff = 0;
					}
					rleBuffer[numDiff] = value;
					++numDiff;
					++x;
				}
			}
			if (numDiff > 0)
			{
				oStream << numDiff;
				oStream.write(&rleBuffer[0], numDiff);
			}
		}
	}
	return oStream;
}


Image::FileFormat Image::findFormat(const std::string& iFormatTag)
{
	const std::string key = stde::tolower(iFormatTag);
	TFileFormats::const_iterator i = fileFormats_.find(key);
	if (i == fileFormats_.end())
	{
		LASS_THROW("unknown fileformat '" << key << "'.");
	}
	return i->second;
}



Image::TFileFormats Image::fillFileFormats()
{
	TFileFormats formats;
	formats["raw"] = FileFormat(&Image::openRaw, &Image::saveRaw);
	formats["targa"] = FileFormat(&Image::openTarga, &Image::saveTarga);
	formats["tga"] = formats["targa"];
	formats["hdr"] = FileFormat(&Image::openRadianceHdr, &Image::saveRadianceHdr);
	formats["pic"] =formats["hdr"];
	return formats;
}



// --- HeaderRaw -----------------------------------------------------------------------------------

void Image::HeaderRaw::readFrom(BinaryIStream& ioIStream)
{
	EndiannessSetter(ioIStream, num::littleEndian);
	ioIStream >> lass >> version >> rows >> cols;
}



void Image::HeaderRaw::writeTo(BinaryOStream& ioOStream)
{
	EndiannessSetter(ioOStream, num::littleEndian);
	ioOStream << lass << version << rows << cols;
}



// --- HeaderTarga ---------------------------------------------------------------------------------

void Image::HeaderTarga::readFrom(BinaryIStream& ioIStream)
{
	EndiannessSetter(ioIStream, num::littleEndian);
	ioIStream >> idLength >> colorMapType >> imageType >> colorMapOrigin >> colorMapLength >>
		colorMapEntrySize >> imageXorigin >> imageYorigin >> imageWidth >> imageHeight >>
		imagePixelSize >> imageDescriptor;
}



void Image::HeaderTarga::writeTo(BinaryOStream& ioOStream)
{
	EndiannessSetter(ioOStream, num::littleEndian);
	ioOStream << idLength << colorMapType << imageType << colorMapOrigin << colorMapLength <<
		colorMapEntrySize << imageXorigin << imageYorigin << imageWidth << imageHeight <<
		imagePixelSize << imageDescriptor;
}



// --- HeaderRadianceHdr ---------------------------------------------------------------------------

Image::HeaderRadianceHdr::HeaderRadianceHdr():
	exposure(1.f),
	height(0),
	width(0),
	yIncreasing(false),
	xIncreasing(true),
	isRgb(false)	
{
	std::fill_n(colorCorr, static_cast<int>(sizeColorCorr), 1.f);
	const float defaultPrimaries[sizePrimaries] = 
		{ 0.640f, 0.330f, 0.290f, 0.600f, 0.150f, 0.060f, 0.333f, 0.333f };
	stde::copy_n(defaultPrimaries, static_cast<int>(sizePrimaries), primaries);
}

void Image::HeaderRadianceHdr::readFrom(BinaryIStream& iStream)
{
	std::size_t magicSize = magicRadiance_.size();
	std::vector<char> magic(magicSize);
	iStream.read(&magic[0], magicSize);
	if (!iStream.good() || !stde::equal_r(magicRadiance_, magic))
	{
		LASS_THROW("file is not of RADIANCE file format");
	}

	// first, read the real header
	//
	while (true)
	{
		std::string line = readString(iStream);
		if (!iStream.good())
		{
			LASS_THROW("syntax error in header of RADIANCE HDR file");
		}
		if (line.empty())
		{
			break;
		}
		if (stde::begins_with(line, std::string("#")))
		{
			continue;
		}
		std::vector<std::string> splitted = stde::split(line, std::string("="), 1);
		if (splitted.size() != 2)
		{
			LASS_THROW("syntax error in header of RADIANCE HDR file");
		}
		std::string command = stde::toupper(splitted[0]);
		std::string value = stde::strip(splitted[1]);
		if (command == "FORMAT")
		{
			if (value == "32-bit_rle_rgbe")
			{
				isRgb = true;
			}
			else if (value == "32-bit_rle_xyze")
			{
				isRgb = false;
			}
			else
			{
				LASS_THROW("unknown value for FORMAT field of header of RADIANCE HDR file");
			}
		}
		else if (command == "EXPOSURE")
		{
			exposure *= util::stringCast<float>(value);
		}
		else if (command == "COLORCORR")
		{
			std::vector<std::string> values = stde::split(value);
			if (values.size() != sizeColorCorr)
			{
				LASS_THROW("syntax error in COLORCORR field of header of RADIANCE HDR file");
			}
			stde::transform_r(values, colorCorr, util::stringCast<float, std::string>);
		}
		else if (command == "PRIMARIES")
		{
			std::vector<std::string> values = stde::split(value);
			if (values.size() != sizePrimaries)
			{
				LASS_THROW("syntax error in PRIMARIES field of header of RADIANCE HDR file");
			}
			stde::transform_r(values, primaries, util::stringCast<float, std::string>);
		}
	}

	// second, interpret the resolution line
	std::string line = readString(iStream);
	if (!iStream.good())
	{
		LASS_THROW("syntax error in resolution line of RADIANCE HDR file");
	}
	std::vector<std::string> splitted = stde::split(line);
	if (splitted.size() != 4)
	{
		LASS_THROW("resolution line of RADIANCE HDR file not of 4 elements");
	}
	std::string y = stde::toupper(splitted[0]);
	std::string x = stde::toupper(splitted[2]);
	if (!(y == "+Y" || y == "-Y") || !(x == "+X" || x == "-X"))
	{
		LASS_THROW("syntax error in resolution line of RADIANCE HDR file");
	}
	yIncreasing = y == "+Y";
	xIncreasing = x == "+X";
	height = util::stringCast<unsigned>(splitted[1]);
	width = util::stringCast<unsigned>(splitted[3]);
}



void Image::HeaderRadianceHdr::writeTo(BinaryOStream& oStream)
{
	oStream.write(magicRadiance_.data(), magicRadiance_.length());
	writeString(oStream, "SOFTWARE=LASS, http://lass.sourceforge.net");
	writeString(oStream, std::string("FORMAT=32-bit_rle_") + (isRgb ? "rgbe" : "xyze"));
	writeString(oStream, "EXPOSURE=" + util::stringCast<std::string>(exposure));
	writeString(oStream, std::string("COLORCORR=") + 
		stde::join(std::string(" "), colorCorr, colorCorr + sizeColorCorr));
	writeString(oStream, std::string("PRIMARIES=") + 
		stde::join(std::string(" "), primaries, primaries + sizePrimaries));
	writeString(oStream, "");
	std::ostringstream resolution;
	resolution << (yIncreasing ? "+Y" : "-Y") << " " << height;
	resolution << (xIncreasing ? "+X" : "-X") << " " << width;
	writeString(oStream, resolution.str());
}



std::string Image::HeaderRadianceHdr::readString(BinaryIStream& iStream)
{
	std::string result;
	while (iStream.good())
	{
		char character;
		iStream >> character;
		if (iStream.good())
		{
			if (character == '\n')
			{
				return result;
			}
			else
			{
				result += character;
			}
		}
	}
	return "";
}



void Image::HeaderRadianceHdr::writeString(BinaryOStream& oStream, const std::string& iString)
{
	oStream.write(iString.data(), iString.size());
	oStream << '\n';
}




// --- free ----------------------------------------------------------------------------------------



}

}

// EOF
