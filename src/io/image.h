/** @file
 *  @author Bram de Greve (bramz@users.sourceforge.net)
 *  @author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  Distributed under the terms of the GPL (GNU Public License)
 *
 *  The LASS License:
 *
 *  Copyright 2004-2006 Bram de Greve and Tom De Muer
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



/** @class lass::io::Image
 *  @brief image thing?
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_IMAGE_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_IMAGE_H

#include "io_common.h"

#include "../num/num_common.h"
#include "../num/endianness.h"
#include "../prim/color_rgba.h"
#include "../prim/point_2d.h"

namespace lass
{

// new interfaces

namespace io
{

class BinaryIStream;
class BinaryOStream;

class LASS_DLL Image
{
public:

	typedef prim::ColorRGBA TPixel;
	typedef std::vector<TPixel> TRaster;

	typedef prim::ColorRGBA::TValue TValue;
	typedef prim::ColorRGBA::TParam TParam;
	typedef prim::ColorRGBA::TNumTraits TNumTraits;

	typedef TValue (*TFilterFunction)(TValue);

	typedef prim::Point2D<num::Tfloat32> TChromaticity;
	enum { numChromaticities = 4 };
	struct Chromaticities
	{
		TChromaticity red;
		TChromaticity green;
		TChromaticity blue;
		TChromaticity white;
		bool isFromFile;
		const TChromaticity& operator[](size_t index) const { LASS_ASSERT(index < numChromaticities); return (&red)[index]; }
		TChromaticity& operator[](size_t index) { LASS_ASSERT(index < numChromaticities); return (&red)[index]; }
	};

	class BadFormat: public util::Exception
	{
	public:
		BadFormat(const std::string& msg, const std::string& loc): util::Exception(msg, loc) {}
	private:
		LASS_UTIL_EXCEPTION_PRIVATE_IMPL(BadFormat)
	};

	// STRUCTORS

	Image();
	Image(unsigned rows, unsigned cols);
	Image(const std::string& filename);
	Image(const Image& other);
	~Image();


	// METHODS

	void reset();
	void reset(unsigned rows, unsigned cols);
	void reset(const std::string& filename);
	void reset(const Image& other);

	void open(const std::string& filename);
	void open(BinaryIStream& stream, const std::string& formatTag);
	void save(const std::string& filename);
	void save(BinaryOStream& stream, const std::string& formatTag);

	Image& operator=(const Image& other);
	void swap(Image& other);

	const TPixel& operator[](unsigned flatIndex) const { return raster_[flatIndex]; }
	TPixel& operator[](unsigned flatIndex) { return raster_[flatIndex]; }

	const TPixel& operator()(unsigned row, unsigned col) const;
	TPixel& operator()(unsigned row, unsigned col);

	const TPixel& at(signed row, signed col) const;
	TPixel& at(signed row, signed col);

	const TPixel* const data() const;
	TPixel* const data();

	const Chromaticities& chromaticities() const;
	Chromaticities& chromaticities();

	const unsigned rows() const;
	const unsigned cols() const;
	const bool isEmpty() const;

	// OPERATIONS

	void over(const Image& other);
	void in(const Image& other);
	void out(const Image& other);
	void atop(const Image& other);
	void through(const Image& other);
	void rover(const Image& other);
	void rin(const Image& other);
	void rout(const Image& other);
	void ratop(const Image& other);
	void rthrough(const Image& other);
	void plus(const Image& other);

	// FILTERS

	void filterMedian(unsigned boxSize);
	void filterGamma(TParam gammaExponent);
	void filterExposure(TParam exposureTime);
	void filterInverseExposure(TParam exposureTime);
	void filter(TFilterFunction function);

private:

	struct HeaderLass
	{
		num::Tuint32 lass;
		num::Tuint32 version;
		num::Tuint32 rows;
		num::Tuint32 cols;

		void readFrom(BinaryIStream& stream);
		void writeTo(BinaryOStream& stream);
	};

	struct HeaderTarga
	{
		num::Tuint8 idLength;
		num::Tuint8 colorMapType;
		num::Tuint8 imageType;
		num::Tuint16 colorMapOrigin;
		num::Tuint16 colorMapLength;
		num::Tuint8	colorMapEntrySize;
		num::Tuint16 imageXorigin;
		num::Tuint16 imageYorigin;
		num::Tuint16 imageWidth;
		num::Tuint16 imageHeight;
		num::Tuint8 imagePixelSize;
		num::Tuint8 imageDescriptor;

		const unsigned numAttributeBits() const { return imageDescriptor & 0x0F; }
		const bool flipHorizontalFlag() const { return ((imageDescriptor >> 4) & 0x01) == 0x01; }
		const bool flipVerticalFlag() const { return ((imageDescriptor >> 5) & 0x01) == 0x01; }
		const bool interleavingFlag() const { return ((imageDescriptor >> 6) & 0x01) == 0x01; }

		void readFrom(BinaryIStream& stream);
		void writeTo(BinaryOStream& stream);
	};

	struct HeaderRadianceHdr
	{
		enum 
		{
			sizeColorCorr = 3,
			sizePrimaries = 8
		};

		float exposure;
		float colorCorr[sizeColorCorr];
		float primaries[sizePrimaries];
		unsigned height;
		unsigned width;
		bool yIncreasing;
		bool xIncreasing;
		bool isRgb;

		HeaderRadianceHdr();
		void readFrom(BinaryIStream& stream);
		void writeTo(BinaryOStream& stream);
	};

	struct HeaderPfm
	{
		unsigned width;
		unsigned height;
		float aspect;
		num::Endianness endianness;
		bool isGrey;

		HeaderPfm();
		void readFrom(BinaryIStream& stream);
		void writeTo(BinaryOStream& stream);
	};

	typedef BinaryIStream& (Image::*TFileOpener)(BinaryIStream&);
	typedef BinaryOStream& (Image::*TFileSaver)(BinaryOStream&) const;

	struct FileFormat
	{
		TFileOpener open;
		TFileSaver save;
		FileFormat(TFileOpener iOpen, TFileSaver iSave): open(iOpen), save(iSave) {}
		FileFormat(): open(0), save(0) {}
	};

	typedef std::map<std::string, FileFormat> TFileFormats;

	unsigned resize(unsigned rows, unsigned cols);
	unsigned flatIndex(unsigned rows, unsigned cols) const 
	{ 
		return rows * cols_ + cols;
	}

	void defaultChromaticities();

	BinaryIStream& openLass(BinaryIStream& stream);
	BinaryIStream& openTarga(BinaryIStream& stream);
	BinaryIStream& openTargaTrueColor(BinaryIStream& stream, const HeaderTarga& iHeader);
	BinaryIStream& openRadianceHdr(BinaryIStream& stream);
	BinaryIStream& openPfm(BinaryIStream& stream);

	BinaryOStream& saveLass(BinaryOStream& stream) const;
	BinaryOStream& saveTarga(BinaryOStream& stream) const;
	BinaryOStream& saveRadianceHdr(BinaryOStream& stream) const;
	BinaryOStream& savePfm(BinaryOStream& stream) const;
	
	FileFormat findFormat(const std::string& formatTag);
	std::string readRadianceHdrString(BinaryIStream& stream) const;
		
	static BinaryIStream& readLine(BinaryIStream& stream, std::string& line);
	static BinaryOStream& writeLine(BinaryOStream& stream, const std::string& line);

	static TFileFormats fillFileFormats();

	Chromaticities chromaticities_;
	unsigned rows_;
	unsigned cols_;
	TRaster raster_;

	static TFileFormats fileFormats_;
	static num::Tuint32 magicLass_;
	static std::string magicRadiance_;
};



}

}

#endif

// EOF
