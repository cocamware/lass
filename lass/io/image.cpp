/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

#include "lass_common.h"
#include "image.h"
#include "binary_i_file.h"
#include "binary_o_file.h"
#include "file_attribute.h"
#include "../stde/extended_string.h"
#include "../stde/extended_algorithm.h"
#include "../stde/range_algorithm.h"
#include "../stde/static_vector.h"
#include "../prim/transformation_3d.h"
#include "../num/num_cast.h"

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
		Bytes4(): values_() {}
		num::Tuint8 operator[](size_t k) const { LASS_ASSERT(k < 4); return values_[k]; }
		num::Tuint8& operator[](size_t k) { LASS_ASSERT(k < 4); return values_[k]; }
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
num::Tint32 Image::magicIgi_ = 66613373;

// --- public --------------------------------------------------------------------------------------

/** Default constructor.  empty image.
 */
Image::Image():
	colorSpace_(defaultColorSpace()),
	rows_(0),
	cols_(0),
	raster_()
{
}



/** Construct image of given width and height.
 */
Image::Image(size_t rows, size_t cols):
	colorSpace_(defaultColorSpace()),
	rows_(0),
	cols_(0),
	raster_()
{
	const TPixel black;
	resize(rows, cols);
	std::fill(raster_.begin(), raster_.end(), black);
}



/** Construct image from given file.
 */
Image::Image(const std::string& filename):
	colorSpace_(defaultColorSpace()),
	rows_(0),
	cols_(0),
	raster_()
{
	open(filename);
}



/** Copy constructor
 */
Image::Image(const Image& other):
	colorSpace_(other.colorSpace_),
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
void Image::reset(size_t rows, size_t cols)
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

	try
	{
		(this->*saver)(stream);
	}
	catch (const num::BadNumCast&)
	{
		LASS_THROW_EX(BadFormat, "image size is too large for this format");
	}

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
	std::swap(colorSpace_, other.colorSpace_);
	std::swap(rows_, other.rows_);
	std::swap(cols_, other.cols_);
	raster_.swap(other.raster_);
}



/** Return const pixel at position row, col.
 *  @param row row of pixel, y coordinate.
 *  @param col column of pixel, x coordinate.
 */
const Image::TPixel& Image::operator()(size_t row, size_t col) const
{
	LASS_ASSERT(row < rows_ && col < cols_);
	return raster_[flatIndex(row, col)];
}



/** Return reference to pixel at position row, col.
 *  @param row row of pixel, y coordinate.
 *  @param col column of pixel, x coordinate.
 */
Image::TPixel& Image::operator()(size_t row, size_t col)
{
	LASS_ASSERT(row < rows_ && col < cols_);
	return raster_[flatIndex(row, col)];
}



/** Return const pixel at position row, col.  position is wrapped around.
 *  @param row row of pixel, y coordinate.
 *  @param col column of pixel, x coordinate.
 */
const Image::TPixel& Image::at(TSignedSize row, TSignedSize col) const
{
	const size_t i = num::mod(row, rows_);
	const size_t j = num::mod(col, cols_);
	return raster_[flatIndex(i, j)];
}



/** Return reference to pixel at position row, col. position is wrapped around.
 *  @param row row of pixel, y coordinate.
 *  @param col column of pixel, x coordinate.
 */
Image::TPixel& Image::at(TSignedSize row, TSignedSize col)
{
	const size_t i = num::mod(row, rows_);
	const size_t j = num::mod(col, cols_);
	return raster_[flatIndex(i, j)];
}



/** Return const data block.
 */
const Image::TPixel* Image::data() const
{
	return &raster_[0];
}



/** Return data block.
 */
Image::TPixel* Image::data()
{
	return &raster_[0];
}



/** Return colorSpace of image data
 */
const Image::ColorSpace& Image::colorSpace() const
{
	return colorSpace_;
}



/** Return colorSpace of image data
 *
 *  You can change the parameters of the current color space without transforming
 *  the pixels.  This is most useful if you know that the pixels are in a different
 *  color space that the current one is set to.  For example, if you know the pixels
 *  have a gamma correction 2.2 applied, but the this->colorSpace().gamma == 1.
 */
Image::ColorSpace& Image::colorSpace()
{
	return colorSpace_;
}



/** Transform the colors from the current color spacer to @a destColorSpace.
 *
 *	The transformation consists of three stages:
 *  @arg linearise pixels if gamma of current color space != 1
 *  @arg transform colors to linearised destination colour space
 *  @arg apply gamma correction if @a destColorSpace.gamma != 1
 *
 *  For the color transformation, two transformations are concatenated.
 *  Both of them will also perform a chromatic adaption transform if 
 *  the white point of either color space is not the equal energy one (E):
 *  @arg from source RGB to XYZ
 *  @arg from XYZ to dest RGB
 */
void Image::transformColors(const ColorSpace& newColorSpace)
{
	typedef prim::Transformation3D<TValue> TTransformation;

	struct Impl
	{
		static const TTransformation rgb2xyz(const ColorSpace& colorSpace)
		{
			typedef TTransformation::TVector TVector;
			const ColorSpace& C = colorSpace;
			TValue primaries[16] = 
			{
				C.red.x, C.green.x, C.blue.x, 0,
				C.red.y, C.green.y, C.blue.y, 0,
				1 - C.red.x - C.red.y, 1 - C.green.x - C.green.y, 1 - C.blue.x - C.blue.y, 0,
				0, 0, 0, 1
			};
			TTransformation M(primaries, primaries + 16);
			const TVector w(C.white.x / C.white.y, 1, (1 - C.white.x - C.white.y) / C.white.y);
			const TVector S = transform(w, M.inverse());
			M = concatenate(TTransformation::scaler(S), M);

			TValue bradford[16] = 
			{
				 0.8951f,  0.2664f, -0.1614f, 0.f,
				-0.7502f,  1.7135f,  0.0367f, 0.f,
				 0.0389f, -0.0685f,  1.0296f, 0.f,
				 0.f,      0.f,      0.f,     1.f
			};
			const TTransformation B(bradford, bradford + 16);
			const TVector sw = transform(w, B);
			TTransformation M_cat = concatenate(B, TTransformation::scaler(sw.reciprocal()));
			M_cat = concatenate(M_cat, B.inverse());
			
			return concatenate(M, M_cat);
		}
	};

	const TTransformation A = Impl::rgb2xyz(colorSpace_);
	const TTransformation B = Impl::rgb2xyz(newColorSpace);
	const TTransformation C = concatenate(A, B.inverse());

	if (colorSpace_.gamma != 1)
	{
		filterGamma(1 / colorSpace_.gamma);
	}
	for (TRaster::iterator i = raster_.begin(); i != raster_.end(); ++i)
	{
		*i = transform(*i, C);
	}
	if (newColorSpace.gamma != 1)
	{
		filterGamma(newColorSpace.gamma);
	}
	colorSpace_ = newColorSpace;
	colorSpace_.isFromFile = false;
}



/** Return height of image.
 */
size_t Image::rows() const
{
	return rows_;
}



/** Return width of image.
 */
size_t Image::cols() const
{
	return cols_;
}



/** Return true if image is empty (no data)
 */
bool Image::isEmpty() const
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



/** clamp all negative pixel components to zero.
 */
void Image::clampNegatives()
{
	const TRaster::iterator last = raster_.end();
	for (TRaster::iterator i = raster_.begin(); i != last; ++i)
	{
		i->r = std::max(i->r, 0.f);
		i->g = std::max(i->g, 0.f);
		i->b = std::max(i->b, 0.f);
		i->a = std::max(i->a, 0.f);
	}
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
void Image::filterMedian(size_t boxSize)
{
	if (boxSize <= 1)
	{
		return;
	}
	if ((boxSize & 0x1) == 0)
	{
		LASS_THROW("boxSize '" << boxSize << "' isn't odd as requested.");
	}

	const size_t boxArea = boxSize * boxSize;
	const size_t boxRadius = (boxSize - 1) / 2;
	const size_t invalidCandidate = boxArea;

	TRaster box(boxArea);
	std::vector<float> distances(boxArea);

	for (size_t y0 = 0; y0 < rows_; ++y0)
	{
		const size_t yFirst = std::max(y0, boxRadius) - boxRadius;
		const size_t yLast = std::min(y0 + boxRadius + 1, rows_);
		LASS_ASSERT(yLast - yFirst <= boxSize);
		for (size_t x0 = 0; x0 < cols_; ++x0)
		{
			TPixel& center = (*this)(y0, x0);
			if (center.a == TNumTraits::zero)
			{
				continue; // no filtering on pixels with alphachannel == 0
			}

			const size_t xFirst = std::max(x0, boxRadius) - boxRadius;
			const size_t xLast = std::min(x0 + boxRadius + 1, cols_);
			LASS_ASSERT(xLast - xFirst <= boxSize);

			// fill filterbox
			//
			size_t pixelsInBox = 0;
			size_t candidate = invalidCandidate;
			for (size_t y = yFirst; y < yLast; ++y)
			{
				for (size_t x = xFirst; x < xLast; ++x)
				{
					const TPixel& pixel = (*this)(y0, x0);
					if (pixel.a != TNumTraits::zero)
					{
						box[pixelsInBox] = pixel;
						if (x == x0 && y == y0)
						{
							candidate = pixelsInBox;
						}
						++pixelsInBox;
					}
				}
			}

			// get median in filterbox
			//
			LASS_ASSERT(pixelsInBox > 0 && pixelsInBox <= boxArea);
			LASS_ASSERT(candidate != invalidCandidate && candidate < pixelsInBox);
			for (size_t i = 0; i < pixelsInBox; ++i)
			{
				distances[i] = 0.0;
				for (size_t j = 0; j < pixelsInBox; ++j)
				{
					distances[i] += prim::distance(box[i], box[j]);
				}
			}
			for (size_t i = 0; i < pixelsInBox; ++i)
			{
				if (distances[i] < distances[candidate])
				{
					candidate = i;
				}
			}

			LASS_ASSERT(candidate < pixelsInBox);
			center = box[candidate];
		}
	}
}



/** apply gamma correction to image
 */
void Image::filterGamma(TParam gammaExponent)
{
	const size_t size = raster_.size();
	for (size_t i = 0; i < size; ++i)
	{
		raster_[i] = raster_[i].gammaCorrected(gammaExponent);
	}
	colorSpace_.gamma *= gammaExponent;
}



/** apply exposure to image
 */
void Image::filterExposure(TParam exposureTime)
{
	const size_t size = raster_.size();
	for (size_t i = 0; i < size; ++i)
	{
		raster_[i] = raster_[i].exposed(exposureTime);
	}
}



/** apply exposure to image
 */
void Image::filterInverseExposure(TParam exposureTime)
{
	const size_t size = raster_.size();
	for (size_t i = 0; i < size; ++i)
	{
		raster_[i] = raster_[i].invExposed(exposureTime);
	}
}



// --- protected -----------------------------------------------------------------------------------




// --- private -------------------------------------------------------------------------------------

/** (re)allocate data chunck
 */
size_t Image::resize(size_t rows, size_t cols)
{
	const size_t size = rows * cols;
	raster_.resize(size);
	rows_ = rows;
	cols_ = cols;
	return size;
}



/** Open LASS Raw file.
 */
BinaryIStream& Image::openLass(BinaryIStream& stream)
{
	HeaderLass header;
	header.readFrom(stream);
	if (!stream || header.lass != magicLass_ || header.version > 3)
	{
		LASS_THROW_EX(BadFormat, "not a LASS RAW version 1 - 3 file.");
	}

	EndiannessSetter(stream, num::littleEndian);

	if (header.version >= 2)
	{
		for (size_t i = 0; i < numChromaticities; ++i)
		{
			num::Tfloat32 x, y;
			stream >> x >> y;
			colorSpace_[i] = TChromaticity(x, y);
		}
		colorSpace_.isFromFile = true;
	}
	else
	{
		colorSpace_ = defaultColorSpace();
	}

	if (header.version >= 3)
	{
		num::Tfloat32 g;
		stream >> g;
		colorSpace_.gamma = g;
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
	colorSpace_ = defaultColorSpace();
	colorSpace_.gamma = 2.2f;
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
BinaryIStream& Image::openTargaTrueColor(BinaryIStream& stream, const HeaderTarga& header)
{
	const TValue scale = num::inv(255.f);

	std::size_t numBytes = 0;
	switch (header.imagePixelSize)
	{
	case 24: 
		numBytes = 3; 
		break;
	case 32:
		numBytes = 4;
		break;
	default:
		LASS_THROW_EX(BadFormat, "image pixel size '" << header.imagePixelSize << "' not supported.");
	};

	LASS_ASSERT(cols_ == header.imageWidth);
	const int xBegin = header.flipHorizontalFlag() ? static_cast<int>(cols_) - 1 : 0;
	const int xEnd   = header.flipHorizontalFlag() ? -1 : static_cast<int>(cols_);
	const int xDelta = header.flipHorizontalFlag() ? -1 : 1;

	LASS_ASSERT(rows_ == header.imageHeight);
	const int yBegin = header.flipVerticalFlag() ? 0 : static_cast<int>(rows_) - 1;
	const int yEnd   = header.flipVerticalFlag() ? static_cast<int>(rows_) : -1;
	const int yDelta = header.flipVerticalFlag() ? 1 : -1;

	stream.seekg(header.idLength + header.colorMapLength * header.colorMapEntrySize, 
		std::ios_base::cur);
	std::vector<impl::Bytes4> buffer(cols_);
	for (int y = yBegin; y != yEnd; y += yDelta)
	{	
		if (header.imageType == 10)
		{
			// decode rle
			//
			unsigned x = 0;
			while (x < cols_)
			{
				num::Tuint8 code;
				stream >> code;
				const num::Tuint8 packetSize = static_cast<num::Tuint8>((code & 0x7f) + 1);
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
			LASS_ASSERT(header.imageType == 2);
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
			colorSpace_[i] = TChromaticity(header.primaries[2 * i], header.primaries[2 * i + 1]);
		}
	}
	else
	{
		colorSpace_.red = TChromaticity(1, 0);
		colorSpace_.green = TChromaticity(0, 1);
		colorSpace_.blue = TChromaticity(0, 0);
		colorSpace_.white = TChromaticity(1.f / 3, 1.f / 3);
	}
	colorSpace_.gamma = 1;
	colorSpace_.isFromFile = true;

	float exponents[256];
	exponents[0] = 0.f;
	for (int i = 1; i < 256; ++i)
	{
		exponents[i] = ::ldexpf(1.f, i - 128 - 8);
	}
	float inverseCorrections[3];
	for (size_t i = 0; i < 3; ++i)
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
	size_t rleCount = 0;
	size_t rleCountByte = 0;

	for (std::ptrdiff_t y = firstY; y != lastY; y += deltaY)
	{
		// unpack scanline to buffer
		//
		for (std::ptrdiff_t x = firstX; x != lastX; /* increment in loop */ )
		{
			impl::Bytes4 rgbe, previous;
			stream.read(rgbe.get(), 4);
			if (rgbe[0] == 2 && rgbe[1] == 2 && (rgbe[2] & 0x80) == 0)
			{
				// new rle
				//
				const size_t lineLength = rgbe[2] * 256 + rgbe[3];
				const ptrdiff_t lastX2 = x + lineLength * deltaX;
				for (size_t k = 0; k < 4; ++k)
				{
					ptrdiff_t x2 = x;
					while (x2 != lastX2)
					{
						num::Tuint8 spanField = 0, value = 0;
						stream >> spanField;
						const bool isHomogenousSpan = (spanField & 0x80) != 0;
						const size_t spanSize = spanField & 0x7f;
						if (isHomogenousSpan) 
						{
							stream >> value;
						}
						for (size_t i = 0; i < spanSize; ++i)
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
					LASS_ASSERT(rleCountByte < sizeof(rleCount));
					rleCount |= rgbe[3] << (8 * rleCountByte);
					++rleCountByte;
				}
				else
				{
					if (rleCount > 0)
					{
						for (size_t k = rleCount; k > 0; --k)
						{
							buffer[x] = previous;
							x += deltaX;
						}
						rleCount = 0;
						rleCountByte = 0;
					}
					buffer[x] = rgbe;
					x += deltaX;
				}
			}
			previous = rgbe;
		}

		// decode rgbe information
		//
		TPixel* scanline = &raster_[y * header.width];	
		for (std::ptrdiff_t x = firstX; x != lastX; x += deltaX)
		{
			const impl::Bytes4 rgbe = buffer[x];
			TPixel& pixel = scanline[x];
			const float exponent = exponents[rgbe[3]];
			for (size_t k = 0; k < 3; ++k)
			{
				pixel[k] = static_cast<float>(rgbe[k]) * inverseCorrections[k] * exponent;
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

	colorSpace_ = defaultColorSpace();
	colorSpace_.gamma = 1;

	resize(header.height, header.width);
	EndiannessSetter(stream, header.endianness);

	for (size_t k = rows_; k > 0; --k)
	{
		const TRaster::iterator first = raster_.begin() + (k - 1) * cols_;
		const TRaster::iterator last = first + cols_;
		if (header.isGrey)
		{
			num::Tfloat32 y;
			for (TRaster::iterator i = first; i != last; ++i)
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
			for (TRaster::iterator i = first; i != last; ++i)
			{
				stream >> r >> g >> b;
				i->r = r;
				i->g = g;
				i->b = b;
				i->a = 1;
			}
		}
	}

	return stream;
}



BinaryIStream& Image::openIgi(BinaryIStream& stream)
{
	HeaderIgi header;
	header.readFrom(stream);

	if (!stream || header.magic != magicIgi_ || header.version < 1 || header.version > 2)
	{
		LASS_THROW_EX(BadFormat, "not an IGI version <= 2 file.");
	}
	if (header.zipped)
	{
		LASS_THROW_EX(BadFormat, "cannot read zipped IGI files.");
	}
	if (header.dataSize < 0 || static_cast<num::Tuint32>(header.dataSize) != 12 * header.width * header.height)
	{
		LASS_THROW_EX(BadFormat, "error in dataSize field.");
	}

	// The current file format does not specify chromaticies, only distinguishes
	// between RGB and XYZ.  Use default RGB space in case of the former.
	colorSpace_ = header.rgb ? defaultColorSpace() : xyzColorSpace();
	colorSpace_.gamma = 1;
	colorSpace_.isFromFile = true;

	resize(header.height, header.width);
	num::Tfloat32 r, g, b;
	EndiannessSetter(stream, num::littleEndian);
	for (TRaster::iterator i = raster_.begin(); i != raster_.end(); ++i)
	{
		stream >> r >> g >> b;
		i->r = r;
		i->g = g;
		i->b = b;
		i->a = 1;
	}

	return stream;
}


BinaryOStream& Image::saveLass(BinaryOStream& stream) const
{
	HeaderLass header;
	header.lass = magicLass_;
	header.version = 2;
	header.rows = num::numCast<num::Tuint32>(rows_);
	header.cols = num::numCast<num::Tuint32>(cols_);
	header.writeTo(stream);

	EndiannessSetter(stream, num::littleEndian);
	
	for (size_t i = 0; i < numChromaticities; ++i)
	{
		const num::Tfloat32 x = colorSpace_[i].x;
		const num::Tfloat32 y = colorSpace_[i].y;
		stream << x << y;
	}

	const num::Tfloat32 c = colorSpace_.gamma;
	stream << c;

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
	header.imageWidth = num::numCast<num::Tuint16>(cols_);;
	header.imageHeight = num::numCast<num::Tuint16>(rows_);
	header.imagePixelSize = 32;
	header.imageDescriptor = 0x08; // 8 attribute bits

	header.writeTo(stream);

	std::vector<impl::Bytes4> buffer(header.imageWidth);
	std::vector<impl::Bytes4> rleBuffer(128);
	for (size_t y = rows_; y > 0; --y)
	{
		size_t x;

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
		size_t totalLength = 0;
		num::Tuint8 numDiff = 0;
		x = 0;
		while (x < cols_)
		{
			const impl::Bytes4& bytes = buffer[x];
			size_t x2 = x;
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
	header.isRgb = colorSpace_ != xyzColorSpace();
	for (size_t i = 0; i < numChromaticities; ++i)
	{
		header.primaries[2 * i] = colorSpace_[i].x;
		header.primaries[2 * i + 1] = colorSpace_[i].y;
	}
	header.writeTo(stream);

	std::vector<impl::Bytes4> buffer(cols_);
	std::vector<num::Tuint8> rleBuffer(128);

	for (size_t y = 0; y < rows_; ++y)
	{
		// first, transform a line of rgb pixels to rgbe reprentation
		//
		const TPixel* line = &raster_[y * cols_];
		for (size_t x = 0; x < cols_; ++x)
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
				for (size_t k = 0; k < 3; ++k)
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
		bytes[2] = static_cast<num::Tuint8>((cols_ & 0x7f00) >> 8);
		bytes[3] = static_cast<num::Tuint8>(cols_ & 0xff);
		stream.write(bytes, 4);

		for (size_t k = 0; k < 4; ++k)
		{
			num::Tuint8 numDiff = 0;
			size_t x = 0;
			while (x < cols_)
			{
				num::Tuint8 value = buffer[x][k];
				size_t x2 = x;
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
					
					const num::Tuint8 spanField = static_cast<num::Tuint8>(numSame | 0x80);
					stream << spanField << value;
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

	for (size_t k = rows_; k > 0; --k)
	{
		const TRaster::const_iterator first = raster_.begin() + (k - 1) * cols_;
		const TRaster::const_iterator last = first + cols_;
		for (TRaster::const_iterator i = first; i != last; ++i)
		{
			const num::Tfloat32 r = i->r;
			const num::Tfloat32 g = i->g;
			const num::Tfloat32 b = i->b;
			stream << r << g << b;
		}
	}

	return stream;
}



BinaryOStream& Image::saveIgi(BinaryOStream& stream) const
{
	HeaderIgi header;
	header.magic = magicIgi_;
	header.version = 1;
	header.numSamples = static_cast<num::Tfloat64>(rows_ * cols_);
	header.width = num::numCast<num::Tuint32>(cols_);
	header.height = num::numCast<num::Tuint32>(rows_);
	header.superSampling = 1;
	header.zipped = 0;
	header.dataSize = num::numCast<num::Tuint32>(12 * rows_ * cols_);
	header.rgb = colorSpace_ == xyzColorSpace() ? 0 : 1;
	header.writeTo(stream);

	EndiannessSetter(stream, num::littleEndian);

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
	while (true)
	{
		char character;
		stream >> character;
		if (!stream.good())
		{
			return stream;
		}
		if (character == '\n')
		{
			line.swap(result);
			return stream;
		}
		result += character;
	}
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
	formats["igi"] = FileFormat(&Image::openIgi, &Image::saveIgi);
	return formats;
}



/** return sRGB as colorSpace
 */
const Image::ColorSpace Image::defaultColorSpace()
{
	ColorSpace result;
	result.red = TChromaticity(0.6400f, 0.3300f);
	result.green = TChromaticity(0.3000f, 0.6000f);
	result.blue = TChromaticity(0.1500f, 0.0600f);
	result.white = TChromaticity(0.3127f, 0.3290f);
	result.gamma = 1;
	result.isFromFile = false;
	return result;
}



const Image::ColorSpace Image::xyzColorSpace()
{
	ColorSpace result;
	result.red = TChromaticity(1.f, 0.f);
	result.green = TChromaticity(0.f, 1.f);
	result.blue = TChromaticity(0.f, 0.f);
	result.white = TChromaticity(1.f / 3, 1.f / 3);
	result.gamma = 1;
	result.isFromFile = false;
	return result;
}



// --- HeaderLass -----------------------------------------------------------------------------------

void Image::HeaderLass::readFrom(BinaryIStream& stream)
{
	EndiannessSetter(stream, num::littleEndian);
	stream >> lass >> version >> rows >> cols;
}



void Image::HeaderLass::writeTo(BinaryOStream& stream)
{
	EndiannessSetter(stream, num::littleEndian);
	stream << lass << version << rows << cols;
}



// --- HeaderTarga ---------------------------------------------------------------------------------

void Image::HeaderTarga::readFrom(BinaryIStream& stream)
{
	EndiannessSetter(stream, num::littleEndian);
	stream >> idLength >> colorMapType >> imageType >> colorMapOrigin >> colorMapLength >>
		colorMapEntrySize >> imageXorigin >> imageYorigin >> imageWidth >> imageHeight >>
		imagePixelSize >> imageDescriptor;
}



void Image::HeaderTarga::writeTo(BinaryOStream& stream)
{
	EndiannessSetter(stream, num::littleEndian);
	stream << idLength << colorMapType << imageType << colorMapOrigin << colorMapLength <<
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
	isRgb(true),
	isDefaultPrimaries(true)
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
			isDefaultPrimaries = false;
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
	height = util::stringCast<size_t>(splitted[1]);
	width = util::stringCast<size_t>(splitted[3]);
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
	resolution << " " << (xIncreasing ? "+X" : "-X") << " " << width;
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
		width = util::stringCast<size_t>(attributes[0]);
		height = util::stringCast<size_t>(attributes[1]);
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



// --- HeaderIgi -----------------------------------------------------------------------------------

void Image::HeaderIgi::readFrom(BinaryIStream& stream)
{
	EndiannessSetter(stream, num::littleEndian);
	stream >> magic >> version >> numSamples >> width >> height >> superSampling >> zipped
		>> dataSize >> rgb;
	stream.seekg(padding, std::ios_base::cur);
}



void Image::HeaderIgi::writeTo(BinaryOStream& stream)
{
	EndiannessSetter(stream, num::littleEndian);
	stream << magic << version << numSamples << width << height << superSampling << zipped
		<< dataSize << rgb;
	stream.seekp(padding, std::ios_base::cur);
}

// --- free ----------------------------------------------------------------------------------------



}

}

// EOF
