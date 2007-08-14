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
		::lass::util::impl::TruePredicate,\
		::lass::util::impl::DefaultRaiser,\
		((a).rows() == (b).rows() && (a).cols() == (b).cols()),\
		int(0),\
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
		bool operator==(const Bytes4& other) const { return std::equal(values_, values_ + 4, other.values_); }
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
	defaultChromaticities();
}



/** Construct image of given width and height.
 */
Image::Image(unsigned rows, unsigned cols):
	rows_(0),
	cols_(0),
	raster_()
{
	const TPixel black;
	resize(rows, cols);
	std::fill(raster_.begin(), raster_.end(), black);
	defaultChromaticities();
}



/** Construct image from given file.
 */
Image::Image(const std::string& filename):
	rows_(0),
	cols_(0),
	raster_()
{
	open(filename);
}



/** Copy constructor
 */
Image::Image(const Image& other):
	rows_(other.rows_),
	cols_(other.cols_),
	raster_(other.raster_)
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
void Image::reset(unsigned rows, unsigned cols)
{
	Image temp(rows, cols);
	swap(temp);
}



/** Reset image to the one in the given file.
 */
void Image::reset(const std::string& filename)
{
	Image temp(filename);
	swap(temp);
}



/** Reset image to copy of given image.
 */
void Image::reset(const Image& other)
{
	Image temp(other);
	swap(temp);
}



/** Open image from file
 */
void Image::open(const std::string& filename)
{
	try
	{
		BinaryIFile file(filename);
		if (!file)
		{
			LASS_THROW("could not open file to read.");
		}

		open(file, fileExtension(filename));
	}
	catch (const BadFormat& error)
	{
		LASS_THROW_EX(BadFormat, "'" << filename << "': " << error.message());
	}
	catch (const util::Exception& error)
	{
		LASS_THROW("'" << filename << "': " << error.message());
	}
}



/** Open image from binary stream
 */
void Image::open(BinaryIStream& stream, const std::string& formatTag)
{
	Image temp;
	TFileOpener opener = findFormat(formatTag).open;
	if (!opener)
	{
		LASS_THROW_EX(BadFormat, "cannot open images in file format '" << formatTag << "'.");
	}

	(temp.*opener)(stream);

	if (stream.good())
	{
		swap(temp);
	}
	else if (stream.eof())
	{
		LASS_THROW_EX(BadFormat, "tried to read past end of file.");
	}
	else
	{
		LASS_THROW_EX(BadFormat, "unknown failure in file.");
	}
}



/** Save image to file
 */
void Image::save(const std::string& filename)
{
	try
	{
		BinaryOFile file(filename);
		if (!file)
		{
			LASS_THROW("could not open file to write.");
		}

		save(file, fileExtension(filename));
	}
	catch (const BadFormat& error)
	{
		LASS_THROW_EX(BadFormat, "'" << filename << "': " << error.message());
	}
	catch (const util::Exception& error)
	{
		LASS_THROW_EX(BadFormat, "'" << filename << "': " << error.message());
	}
}



/** Save image to file
 */
void Image::save(BinaryOStream& stream, const std::string& formatTag)
{
	TFileSaver saver = findFormat(formatTag).save;
	if (!saver)
	{
		LASS_THROW_EX(BadFormat, "cannot save images in file format '" << formatTag << "'.");
	}

	(this->*saver)(stream);

	if (stream.eof())
	{
		LASS_THROW_EX(BadFormat, "tried to write past end of file.");
	}
	if (!stream.good())
	{
		LASS_THROW_EX(BadFormat, "unknown failure in file.");
	}
}



/** Copy other into this image.
 */
Image& Image::operator=(const Image& other)
{
	Image temp(other);
	swap(temp);
	return *this;
}



/** swap two images
 */
void Image::swap(Image& other)
{
	std::swap(rows_, other.rows_);
	std::swap(cols_, other.cols_);
	raster_.swap(other.raster_);
}



/** Return const pixel at position row, col.
 *  @param row row of pixel, y coordinate.
 *  @param col column of pixel, x coordinate.
 */
const Image::TPixel& Image::operator()(unsigned row, unsigned col) const
{
	LASS_ASSERT(row < rows_ && col < cols_);
	return raster_[flatIndex(row, col)];
}



/** Return reference to pixel at position row, col.
 *  @param row row of pixel, y coordinate.
 *  @param col column of pixel, x coordinate.
 */
Image::TPixel& Image::operator()(unsigned row, unsigned col)
{
	LASS_ASSERT(row < rows_ && col < cols_);
	return raster_[flatIndex(row, col)];
}



/** Return const pixel at position row, col.  position is wrapped around.
 *  @param row row of pixel, y coordinate.
 *  @param col column of pixel, x coordinate.
 */
const Image::TPixel& Image::at(signed row, signed col) const
{
	const unsigned i = num::mod(row, rows_);
	const unsigned j = num::mod(col, cols_);
	return raster_[flatIndex(i, j)];
}



/** Return reference to pixel at position row, col. position is wrapped around.
 *  @param row row of pixel, y coordinate.
 *  @param col column of pixel, x coordinate.
 */
Image::TPixel& Image::at(signed row, signed col)
{
	const unsigned i = num::mod(row, rows_);
	const unsigned j = num::mod(col, cols_);
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



/** Return chromaticities of image data
 */
const Image::Chromaticities& Image::chromaticities() const
{
	return chromaticities_;
}



/** Return chromaticities of image data
 */
Image::Chromaticities& Image::chromaticities()
{
	return chromaticities_;
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
void Image::over(const Image& other)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, other);
	std::transform(raster_.begin(), raster_.end(), other.raster_.begin(), raster_.begin(), prim::over);
}



/** this = this in other
 */
void Image::in(const Image& other)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, other);
	std::transform(raster_.begin(), raster_.end(), other.raster_.begin(), raster_.begin(), prim::in);
}



/** this = this out other
 */
void Image::out(const Image& other)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, other);
	std::transform(raster_.begin(), raster_.end(), other.raster_.begin(), raster_.begin(), prim::out);
}



/** this = this atop other
 */
void Image::atop(const Image& other)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, other);
	std::transform(raster_.begin(), raster_.end(), other.raster_.begin(), raster_.begin(), prim::atop);
}



/** this = this through other
 */
void Image::through(const Image& other)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, other);
	std::transform(raster_.begin(), raster_.end(), other.raster_.begin(), raster_.begin(), prim::through);
}



/** this = other over this
 */
void Image::rover(const Image& other)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, other);
	std::transform(other.raster_.begin(), other.raster_.end(), raster_.begin(), raster_.begin(), prim::over);
}



/** this = other in this
 */
void Image::rin(const Image& other)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, other);
	std::transform(other.raster_.begin(), other.raster_.end(), raster_.begin(), raster_.begin(), prim::in);
}



/** this = other out this
 */
void Image::rout(const Image& other)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, other);
	std::transform(other.raster_.begin(), other.raster_.end(), raster_.begin(), raster_.begin(), prim::out);
}



/** this = other atop this
 */
void Image::ratop(const Image& other)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, other);
	std::transform(other.raster_.begin(), other.raster_.end(), raster_.begin(), raster_.begin(), prim::atop);
}



/** this = other through this
 */
void Image::rthrough(const Image& other)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, other);
	std::transform(other.raster_.begin(), other.raster_.end(), raster_.begin(), raster_.begin(), prim::through);
}



/** this = this plus other = other plus this
 */
void Image::plus(const Image& other)
{
	LASS_IO_IMAGE_ENFORCE_SAME_SIZE(*this, other);
	std::transform(raster_.begin(), raster_.end(), other.raster_.begin(), raster_.begin(), prim::plus);
}



/** Apply a median filter on image.
 *  @param boxSize size of box filter, must be odd.
 *
 *  - Filters only pixels with alphachannel != 0.
 *  - We use the vector median filter for colour images, as described on page
 *    in GAUCH J. M. (1998). Noise Removal and contrast enhancement. In:
 *    Sangwine S. J. & Horne R. E. N. (Eds.) The Colour Image Processing
 *    Handbook. London, Chapman & Hall, 149-162.
 */
void Image::filterMedian(unsigned boxSize)
{
	if ((boxSize & 0x1) == 0)
	{
		LASS_THROW("boxSize '" << boxSize << "' isn't odd as requested.");
		return;
	}

	const unsigned boxArea = boxSize * boxSize;
	const int boxRadius = (boxSize - 1) / 2;

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
void Image::filterGamma(TParam gammaExponent)
{
	const TRaster::size_type size = raster_.size();
	for (TRaster::size_type i = 0; i < size; ++i)
	{
		raster_[i] = raster_[i].gammaCorrected(gammaExponent);
	}
}



/** apply exposure to image
 */
void Image::filterExposure(TParam exposureTime)
{
	const TRaster::size_type size = raster_.size();
	for (TRaster::size_type i = 0; i < size; ++i)
	{
		raster_[i] = raster_[i].exposed(exposureTime);
	}
}



/** apply exposure to image
 */
void Image::filterInverseExposure(TParam exposureTime)
{
	const TRaster::size_type size = raster_.size();
	for (TRaster::size_type i = 0; i < size; ++i)
	{
		raster_[i] = raster_[i].invExposed(exposureTime);
	}
}



// --- protected -----------------------------------------------------------------------------------




// --- private -------------------------------------------------------------------------------------

/** (re)allocate data chunck
 */
unsigned Image::resize(unsigned rows, unsigned cols)
{
	const unsigned size = rows * cols;
	raster_.resize(size);
	rows_ = rows;
	cols_ = cols;
	return size;
}



void Image::defaultChromaticities()
{
	chromaticities_.red = TChromaticity(0.6400f, 0.3300f);
	chromaticities_.green = TChromaticity(0.3000f, 0.6000f);
	chromaticities_.blue = TChromaticity(0.1500f, 0.0600f);
	chromaticities_.white = TChromaticity(0.3127f, 0.3290f);
}



/** Open LASS Raw file.
 */
BinaryIStream& Image::openLass(BinaryIStream& stream)
{
	HeaderLass header;
	header.readFrom(stream);
	if (!stream || header.lass != magicLass_ || header.version > 2)
	{
		LASS_THROW_EX(BadFormat, "not a LASS RAW version 1 - 2 file.");
	}

	EndiannessSetter(stream, num::littleEndian);

	if (header.version >= 2)
	{
		for (size_t i = 0; i < numChromaticities; ++i)
		{
			LASS_META_ASSERT(sizeof(TChromaticity::TValue) == sizeof(num::Tfloat32), should_use_32_bit_floating_points_here);
			stream >> chromaticities_[i].x >> chromaticities_[i].y;
		}
	}
	else
	{
		defaultChromaticities();
	}

	resize(header.rows, header.cols);
	num::Tfloat32 r, g, b, a;
	for (TRaster::iterator i = raster_.begin(); i != raster_.end(); ++i)
	{
		stream >> r >> g >> b >> a;
		i->r = r;
		i->g = g;
		i->b = b;
		i->a = a;
	}

	return stream;
}



/** Open TARGA file.
 */
BinaryIStream& Image::openTarga(BinaryIStream& stream)
{
	HeaderTarga header;
	header.readFrom(stream);
	defaultChromaticities();
	resize(header.imageHeight, header.imageWidth);

	switch (header.imageType)
	{
	case 2:
	case 10:
		openTargaTrueColor(stream, header);
		break;
	default:
		LASS_THROW_EX(BadFormat, "unsupported image type '" << static_cast<unsigned>(header.imageType) << "'");
		break;
	};

	return stream;
}



/** Open Type 2 and 11 TARGA file
 */
BinaryIStream& Image::openTargaTrueColor(BinaryIStream& stream, const HeaderTarga& iHeader)
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
		LASS_THROW_EX(BadFormat, "image pixel size '" << iHeader.imagePixelSize << "' not supported.");
	};

	LASS_ASSERT(cols_ == iHeader.imageWidth);
	const int xBegin = iHeader.flipHorizontalFlag() ? static_cast<int>(cols_) - 1 : 0;
	const int xEnd   = iHeader.flipHorizontalFlag() ? -1 : static_cast<int>(cols_);
	const int xDelta = iHeader.flipHorizontalFlag() ? -1 : 1;

	LASS_ASSERT(rows_ == iHeader.imageHeight);
	const int yBegin = iHeader.flipVerticalFlag() ? 0 : static_cast<int>(rows_) - 1;
	const int yEnd   = iHeader.flipVerticalFlag() ? static_cast<int>(rows_) : -1;
	const int yDelta = iHeader.flipVerticalFlag() ? 1 : -1;

	stream.seekg(iHeader.idLength + iHeader.colorMapLength * iHeader.colorMapEntrySize, 
		std::ios_base::cur);
	std::vector<impl::Bytes4> buffer(cols_);
	for (int y = yBegin; y != yEnd; y += yDelta)
	{	
		if (iHeader.imageType == 10)
		{
			// decode rle
			//
			unsigned x = 0;
			while (x < cols_)
			{
				num::Tuint8 code;
				stream >> code;
				const num::Tuint8 packetSize = (code & 0x7f) + 1;
				impl::Bytes4 bytes;
				if (code & 0x80)
				{
					stream.read(bytes.get(), numBytes);
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
						stream.read(bytes.get(), numBytes);
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
				stream.read(buffer[x].get(), numBytes);
			}
		}

		// decode scanline
		//
		TPixel* pixel = &raster_[y * cols_];
		for (int x = xBegin; x != xEnd; x += xDelta)
		{
			const impl::Bytes4& bytes = buffer[x];
			pixel->r = static_cast<TValue>(bytes[2]) * scale;
			pixel->g = static_cast<TValue>(bytes[1]) * scale;
			pixel->b = static_cast<TValue>(bytes[0]) * scale;
			pixel->a = numBytes == 3 ? TNumTraits::one : static_cast<TValue>(bytes[3]) * scale;
			++pixel;
		}
	}

	return stream;
}



BinaryIStream& Image::openRadianceHdr(BinaryIStream& stream)
{
	HeaderRadianceHdr header;
	header.readFrom(stream);
	if (!stream)
	{
		LASS_THROW_EX(BadFormat, "syntax error in RADIANCE HDR header.");
	}

	if (header.isRgb)
	{
		for (size_t i = 0; i < numChromaticities; ++i)
		{
			chromaticities_[i] = TChromaticity(header.primaries[2 * i], header.primaries[2 * i + 1]);
		}
	}
	else
	{
		chromaticities_.red = TChromaticity(1, 0);
		chromaticities_.green = TChromaticity(0, 1);
		chromaticities_.blue = TChromaticity(0, 0);
		chromaticities_.white = TChromaticity(1.f / 3, 1.f / 3);
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
			stream.read(rgbe.get(), 4);
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
						stream >> spanSize;
						const bool isHomogenousSpan = spanSize > 128;
						if (isHomogenousSpan) 
						{
							spanSize -= 128;
							stream >> value;
						}
						for (num::Tuint8 i = spanSize; i > 0; --i)
						{
							if (!isHomogenousSpan)
							{
								stream >> value;
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
	return stream;
}



BinaryIStream& Image::openPfm(BinaryIStream& stream)
{
	HeaderPfm header;
	header.readFrom(stream);
	if (!stream)
	{
		LASS_THROW_EX(BadFormat, "not a PFM file.");
	}

	defaultChromaticities();

	resize(header.height, header.width);
	EndiannessSetter(stream, header.endianness);

	if (header.isGrey)
	{
		num::Tfloat32 y;
		for (TRaster::iterator i = raster_.begin(); i != raster_.end(); ++i)
		{
			stream >> y;
			i->r = y;
			i->g = y;
			i->b = y;
			i->a = 1;
		}
	}
	else
	{
		num::Tfloat32 r, g, b;
		for (TRaster::iterator i = raster_.begin(); i != raster_.end(); ++i)
		{
			stream >> r >> g >> b;
			i->r = r;
			i->g = g;
			i->b = b;
			i->a = 1;
		}
	}

	return stream;
}



BinaryOStream& Image::saveLass(BinaryOStream& stream) const
{
	HeaderLass header;
	header.lass = magicLass_;
	header.version = 2;
	header.rows = rows_;
	header.cols = cols_;
	header.writeTo(stream);

	EndiannessSetter(stream, num::littleEndian);
	
	for (size_t i = 0; i < numChromaticities; ++i)
	{
		LASS_META_ASSERT(sizeof(TChromaticity::TValue) == sizeof(num::Tfloat32), should_use_32_bit_floating_points_here);
		stream << chromaticities_[i].x << chromaticities_[i].y;
	}

	num::Tfloat32 r, g, b, a;
	for (TRaster::const_iterator i = raster_.begin(); i != raster_.end(); ++i)
	{
		r = i->r;
		g = i->g;
		b = i->b;
		a = i->a;
		stream << r << g << b << a;
	}

	return stream;
}


/** Save TARGA file (type 2, 32 bit).
 */
BinaryOStream& Image::saveTarga(BinaryOStream& stream) const
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

	header.writeTo(stream);

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
				stream << static_cast<num::Tuint8>(numDiff - 1);
				for (num::Tuint8 i = 0; i < numDiff; ++i)
				{
					stream.write(&rleBuffer[i], 4);
				}
				totalLength += numDiff;
				LASS_ASSERT(totalLength == x);
				numDiff = 0;
			}
			if (numSame > 1)
			{
				stream << static_cast<num::Tuint8>((numSame - 1) | 0x80);
				stream.write(bytes.get(), 4);
				x = x2;
				totalLength += numSame;
				LASS_ASSERT(totalLength == x);
				numSame = 0;
			}
		}
		LASS_ASSERT(totalLength == cols_);
	}

	return stream;
}


/** Save RADIANCE HDR
 */
BinaryOStream& Image::saveRadianceHdr(BinaryOStream& stream) const
{
	if (cols_ > 0x7fff)
	{
		LASS_THROW_EX(BadFormat, "cannot save this as RADIANCE HDR, because image is too wide");
	}
	HeaderRadianceHdr header;
	header.height = rows_;
	header.width = cols_;
	header.yIncreasing = false;
	header.xIncreasing = true;
	header.isRgb = true;
	for (size_t i = 0; i < numChromaticities; ++i)
	{
		header.primaries[2 * i] = chromaticities_[i].x;
		header.primaries[2 * i + 1] = chromaticities_[i].y;
	}
	header.writeTo(stream);

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
		stream.write(bytes, 4);

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
						stream << numDiff;
						stream.write(&rleBuffer[0], numDiff);
						numDiff = 0;
					}
					numSame += 128;
					stream << numSame << value;
					x = x2;
				}
				else
				{
					if (numDiff == 128)
					{
						stream << numDiff;
						stream.write(&rleBuffer[0], numDiff);
						numDiff = 0;
					}
					rleBuffer[numDiff] = value;
					++numDiff;
					++x;
				}
			}
			if (numDiff > 0)
			{
				stream << numDiff;
				stream.write(&rleBuffer[0], numDiff);
			}
		}
	}
	return stream;
}



BinaryOStream& Image::savePfm(BinaryOStream& stream) const
{
	HeaderPfm header;
	header.height = rows_;
	header.width = cols_;
	header.writeTo(stream);

	EndiannessSetter(stream, header.endianness);

	for (TRaster::const_iterator i = raster_.begin(); i != raster_.end(); ++i)
	{
		const num::Tfloat32 r = i->r;
		const num::Tfloat32 g = i->g;
		const num::Tfloat32 b = i->b;
		stream << r << g << b;
	}

	return stream;
}



Image::FileFormat Image::findFormat(const std::string& formatTag)
{
	const std::string key = stde::tolower(formatTag);
	TFileFormats::const_iterator i = fileFormats_.find(key);
	if (i == fileFormats_.end())
	{
		LASS_THROW_EX(BadFormat, "unknown fileformat '" << key << "'.");
	}
	return i->second;
}



BinaryIStream& Image::readLine(BinaryIStream& stream, std::string& line)
{
	std::string result;
	while (stream.good())
	{
		char character;
		stream >> character;
		if (stream.good())
		{
			if (character == '\n')
			{
				line.swap(result);
			}
			else
			{
				result += character;
			}
		}
	}
	return stream;
}



BinaryOStream& Image::writeLine(BinaryOStream& stream, const std::string& iString)
{
	stream.write(iString.data(), iString.size());
	stream << '\n';
	return stream;
}



Image::TFileFormats Image::fillFileFormats()
{
	TFileFormats formats;
	formats["lass"] = FileFormat(&Image::openLass, &Image::saveLass);
	formats["targa"] = FileFormat(&Image::openTarga, &Image::saveTarga);
	formats["tga"] = formats["targa"];
	formats["hdr"] = FileFormat(&Image::openRadianceHdr, &Image::saveRadianceHdr);
	formats["pic"] = formats["hdr"];
	formats["rgbe"] = formats["hdr"];
	formats["pfm"] = FileFormat(&Image::openPfm, &Image::savePfm);
	return formats;
}



// --- HeaderLass -----------------------------------------------------------------------------------

void Image::HeaderLass::readFrom(BinaryIStream& ioIStream)
{
	EndiannessSetter(ioIStream, num::littleEndian);
	ioIStream >> lass >> version >> rows >> cols;
}



void Image::HeaderLass::writeTo(BinaryOStream& ioOStream)
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
	isRgb(true)	
{
	std::fill_n(colorCorr, static_cast<int>(sizeColorCorr), 1.f);
	const float defaultPrimaries[sizePrimaries] = 
		{ 0.640f, 0.330f, 0.290f, 0.600f, 0.150f, 0.060f, 0.333f, 0.333f };
	stde::copy_n(defaultPrimaries, static_cast<int>(sizePrimaries), primaries);
}

void Image::HeaderRadianceHdr::readFrom(BinaryIStream& stream)
{
	std::size_t magicSize = magicRadiance_.size();
	std::vector<char> magic(magicSize);
	stream.read(&magic[0], magicSize);
	if (!stream.good() || !stde::equal_r(magicRadiance_, magic))
	{
		LASS_THROW_EX(BadFormat, "file is not of RADIANCE file format");
	}

	// first, read the real header
	//
	while (true)
	{
		std::string line;
		readLine(stream, line);
		if (!stream.good())
		{
			LASS_THROW_EX(BadFormat, "syntax error in header of RADIANCE HDR file");
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
			LASS_THROW_EX(BadFormat, "syntax error in header of RADIANCE HDR file");
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
				LASS_THROW_EX(BadFormat, "unknown value for FORMAT field of header of RADIANCE HDR file");
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
				LASS_THROW_EX(BadFormat, "syntax error in COLORCORR field of header of RADIANCE HDR file");
			}
			stde::transform_r(values, colorCorr, util::stringCast<float, std::string>);
		}
		else if (command == "PRIMARIES")
		{
			std::vector<std::string> values = stde::split(value);
			if (values.size() != sizePrimaries)
			{
				LASS_THROW_EX(BadFormat, "syntax error in PRIMARIES field of header of RADIANCE HDR file");
			}
			stde::transform_r(values, primaries, util::stringCast<float, std::string>);
		}
	}

	// second, interpret the resolution line
	std::string line;
	readLine(stream, line);
	if (!stream.good())
	{
		LASS_THROW_EX(BadFormat, "syntax error in resolution line of RADIANCE HDR file");
	}
	std::vector<std::string> splitted = stde::split(line);
	if (splitted.size() != 4)
	{
		LASS_THROW_EX(BadFormat, "resolution line of RADIANCE HDR file not of 4 elements");
	}
	std::string y = stde::toupper(splitted[0]);
	std::string x = stde::toupper(splitted[2]);
	if (!(y == "+Y" || y == "-Y") || !(x == "+X" || x == "-X"))
	{
		LASS_THROW_EX(BadFormat, "syntax error in resolution line of RADIANCE HDR file");
	}
	yIncreasing = y == "+Y";
	xIncreasing = x == "+X";
	height = util::stringCast<unsigned>(splitted[1]);
	width = util::stringCast<unsigned>(splitted[3]);
}



void Image::HeaderRadianceHdr::writeTo(BinaryOStream& stream)
{
	stream.write(magicRadiance_.data(), magicRadiance_.length());
	writeLine(stream, "SOFTWARE=LASS, http://lass.sourceforge.net");
	writeLine(stream, std::string("FORMAT=32-bit_rle_") + (isRgb ? "rgbe" : "xyze"));
	writeLine(stream, "EXPOSURE=" + util::stringCast<std::string>(exposure));
	writeLine(stream, std::string("COLORCORR=") + 
		stde::join(std::string(" "), colorCorr, colorCorr + sizeColorCorr));
	writeLine(stream, std::string("PRIMARIES=") + 
		stde::join(std::string(" "), primaries, primaries + sizePrimaries));
	writeLine(stream, "");
	std::ostringstream resolution;
	resolution << (yIncreasing ? "+Y" : "-Y") << " " << height;
	resolution << (xIncreasing ? "+X" : "-X") << " " << width;
	writeLine(stream, resolution.str());
}



// --- HeaderPfm -----------------------------------------------------------------------------------

Image::HeaderPfm::HeaderPfm():
	width(0),
	height(0),
	aspect(1),
	endianness(num::littleEndian),
	isGrey(false)	
{
}



void Image::HeaderPfm::readFrom(BinaryIStream& stream)
{
	const std::string magicPF = "PF";
	const std::string magicPf = "Pf";
	LASS_ASSERT(magicPF.length() == magicPf.length());
	const size_t magicSize = magicPF.length();
	std::vector<char> magic(magicSize);
	stream.read(&magic[0], magicSize);
	if (!stream.good() || !(stde::equal_r(magicPF, magic) || stde::equal_r(magicPf, magic)))
	{
		LASS_THROW_EX(BadFormat, "file is not a PFM file");
	}

	isGrey = stde::equal_r(magicPf, magic);

	// we need to read a number of attributes that may be spread across one or more lines
	//
	const size_t numAttributes = 3;
	std::string attributes[numAttributes];
	size_t k = 0;
	std::string line;
	std::string attr;
	while (k < numAttributes)
	{
		Image::readLine(stream, line);
		if (!stream.good())
		{
			LASS_THROW_EX(BadFormat, "syntax error in header of PFM file");
		}
		const size_t startComments = line.find("#");
		std::istringstream buffer(line.substr(0, startComments));
		while (k < numAttributes && buffer.good())
		{
			buffer >> attr;
			if (buffer.good())
			{
				attributes[k++] = attr;
			}
		}
	}

	try
	{
		width = util::stringCast<unsigned>(attributes[0]);
		height = util::stringCast<unsigned>(attributes[1]);
		aspect = util::stringCast<float>(attributes[2]);
	}
	catch (const util::BadStringCast&)
	{
		LASS_THROW_EX(BadFormat, "syntax error in header of PFM file");
	}

	endianness = aspect < 0 ? num::littleEndian : num::bigEndian;
	aspect = num::abs(aspect);
}



void Image::HeaderPfm::writeTo(BinaryOStream& stream)
{
	std::ostringstream buffer;
	buffer << "PF\n" << width << " " << height << "\n" << (endianness == num::littleEndian ? -aspect : aspect);
	writeLine(stream, buffer.str());
}



// --- free ----------------------------------------------------------------------------------------



}

}

// EOF
