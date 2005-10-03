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



/** @class lass::io::Image
 *  @brief image thing?
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_IMAGE_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_IMAGE_H

#include "io_common.h"

#include "../num/num_common.h"
#include "../prim/color_rgba.h"

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
	void open(BinaryIStream& iStream, const std::string& iFormatTag);
	void save(const std::string& iFilename);
	void save(BinaryOStream& iStream, const std::string& iFormatTag);

	Image& operator=(const Image& iOther);
	void swap(Image& iOther);

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

	// OPERATIONS

	void over(const Image& iOther);
	void in(const Image& iOther);
	void out(const Image& iOther);
	void atop(const Image& iOther);
	void through(const Image& iOther);
	void rover(const Image& iOther);
	void rin(const Image& iOther);
	void rout(const Image& iOther);
	void ratop(const Image& iOther);
	void rthrough(const Image& iOther);
	void plus(const Image& iOther);

	// FILTERS

	void filterMedian(unsigned iBoxSize);
	void filterGamma(TParam iGammaExponent);
	void filterExposure(TParam iExposureTime);

private:

	struct HeaderRaw
	{
		num::Tuint32 lass;
		num::Tuint32 version;
		num::Tuint32 rows;
		num::Tuint32 cols;

		void readFrom(BinaryIStream& ioIStream);
		void writeTo(BinaryOStream& ioOStream);
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

		void readFrom(BinaryIStream& ioIStream);
		void writeTo(BinaryOStream& ioOStream);
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

	enum
	{
		magicLass_ = LASS_LITTLE_ENDIAN ? 0x7373616c : 0x6c617373 /**< "lass" in ascii */
	};

	unsigned resize(unsigned iRows, unsigned iCols);
	unsigned flatIndex(unsigned iRows, unsigned iCols) const 
	{ 
		return iRows * cols_ + iCols;
	}

	BinaryIStream& openRaw(BinaryIStream& iFile);
	BinaryIStream& openTarga(BinaryIStream& iFile);
	BinaryIStream& openTarga2(BinaryIStream& iFile, const HeaderTarga& iHeader);

	BinaryOStream& saveRaw(BinaryOStream& iFile) const;
	BinaryOStream& saveTarga(BinaryOStream& iFile) const;
	
	FileFormat findFormat(const std::string& iFormatTag);

	static TFileFormats fillFileFormats();

	unsigned rows_;
	unsigned cols_;
	TRaster raster_;

	static TFileFormats fileFormats_;
};



}

}

#endif

// EOF
