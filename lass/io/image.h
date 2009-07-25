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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

	typedef num::BasicType<8 * sizeof(size_t)>::Tint TSignedSize;

	typedef prim::ColorRGBA TPixel;
	typedef std::vector<TPixel> TRaster;

	typedef prim::ColorRGBA::TValue TValue;
	typedef prim::ColorRGBA::TParam TParam;
	typedef prim::ColorRGBA::TNumTraits TNumTraits;

	typedef TValue (*TFilterFunction)(TValue);

	typedef prim::Point2D<num::Tfloat32> TChromaticity;
	enum { numChromaticities = 4 };
	struct ColorSpace
	{
		TChromaticity red;
		TChromaticity green;
		TChromaticity blue;
		TChromaticity white;
		num::Tfloat32 gamma;
		bool isFromFile;
		const TChromaticity& operator[](size_t index) const { LASS_ASSERT(index < numChromaticities); return (&red)[index]; }
		TChromaticity& operator[](size_t index) { LASS_ASSERT(index < numChromaticities); return (&red)[index]; }
		bool operator==(const ColorSpace& other) const 
		{
			return red == other.red && green == other.green && blue == other.blue && white == other.white && gamma == other.gamma;
		}
		bool operator!=(const ColorSpace& other) const { return !(*this == other); }
	};

	class BadFormat: public util::ExceptionMixin<BadFormat>
	{
	public:
		BadFormat(const std::string& msg, const std::string& loc): util::ExceptionMixin<BadFormat>(msg, loc) {}
	};

	// STRUCTORS

	Image();
	Image(size_t rows, size_t cols);
	Image(const std::string& filename);
	Image(const Image& other);
	~Image();


	// METHODS

	void reset();
	void reset(size_t rows, size_t cols);
	void reset(const std::string& filename);
	void reset(const Image& other);

	void open(const std::string& filename);
	void open(BinaryIStream& stream, const std::string& formatTag);
	void save(const std::string& filename);
	void save(BinaryOStream& stream, const std::string& formatTag);

	Image& operator=(const Image& other);
	void swap(Image& other);

	const TPixel& operator[](size_t flatIndex) const { return raster_[flatIndex]; }
	TPixel& operator[](size_t flatIndex) { return raster_[flatIndex]; }

	const TPixel& operator()(size_t row, size_t col) const;
	TPixel& operator()(size_t row, size_t col);

	const TPixel& at(TSignedSize row, TSignedSize col) const;
	TPixel& at(TSignedSize row, TSignedSize col);

	const TPixel* data() const;
	TPixel* data();

	const ColorSpace& colorSpace() const;
	ColorSpace& colorSpace();
	void transformColors(const ColorSpace& newColorSpace);

	size_t rows() const;
	size_t cols() const;
	bool isEmpty() const;

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

	void clampNegatives();

	// FILTERS

	void filterMedian(size_t boxSize);
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
		num::Tuint8 colorMapEntrySize;
		num::Tuint16 imageXorigin;
		num::Tuint16 imageYorigin;
		num::Tuint16 imageWidth;
		num::Tuint16 imageHeight;
		num::Tuint8 imagePixelSize;
		num::Tuint8 imageDescriptor;

		unsigned numAttributeBits() const { return imageDescriptor & 0x0F; }
		bool flipHorizontalFlag() const { return ((imageDescriptor >> 4) & 0x01) == 0x01; }
		bool flipVerticalFlag() const { return ((imageDescriptor >> 5) & 0x01) == 0x01; }
		bool interleavingFlag() const { return ((imageDescriptor >> 6) & 0x01) == 0x01; }

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
		size_t height;
		size_t width;
		bool yIncreasing;
		bool xIncreasing;
		bool isRgb;
		bool isDefaultPrimaries;

		HeaderRadianceHdr();
		void readFrom(BinaryIStream& stream);
		void writeTo(BinaryOStream& stream);
	};

	struct HeaderPfm
	{
		size_t width;
		size_t height;
		float aspect;
		num::Endianness endianness;
		bool isGrey;

		HeaderPfm();
		void readFrom(BinaryIStream& stream);
		void writeTo(BinaryOStream& stream);
	};

	struct HeaderIgi
	{
		num::Tint32 magic;
		num::Tint32 version;
		num::Tfloat64 numSamples;
		num::Tuint32 width;
		num::Tuint32 height;
		num::Tuint32 superSampling;
		num::Tint32 zipped;
		num::Tint32 dataSize;
		num::Tuint32 rgb;
		// num::Tfloat64 render_time;  // we don't care!
		enum { padding = 5000 };
		
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

	size_t resize(size_t rows, size_t cols);
	size_t flatIndex(size_t rows, size_t cols) const 
	{ 
		return rows * cols_ + cols;
	}

	BinaryIStream& openLass(BinaryIStream& stream);
	BinaryIStream& openTarga(BinaryIStream& stream);
	BinaryIStream& openTargaTrueColor(BinaryIStream& stream, const HeaderTarga& iHeader);
	BinaryIStream& openRadianceHdr(BinaryIStream& stream);
	BinaryIStream& openPfm(BinaryIStream& stream);
	BinaryIStream& openIgi(BinaryIStream& stream);

	BinaryOStream& saveLass(BinaryOStream& stream) const;
	BinaryOStream& saveTarga(BinaryOStream& stream) const;
	BinaryOStream& saveRadianceHdr(BinaryOStream& stream) const;
	BinaryOStream& savePfm(BinaryOStream& stream) const;
	BinaryOStream& saveIgi(BinaryOStream& stream) const;
	
	FileFormat findFormat(const std::string& formatTag);
	std::string readRadianceHdrString(BinaryIStream& stream) const;
	
	static BinaryIStream& readLine(BinaryIStream& stream, std::string& line);
	static BinaryOStream& writeLine(BinaryOStream& stream, const std::string& line);

	static TFileFormats fillFileFormats();

	static const ColorSpace defaultColorSpace();
	static const ColorSpace xyzColorSpace();

	ColorSpace colorSpace_;
	size_t rows_;
	size_t cols_;
	TRaster raster_;

	static TFileFormats fileFormats_;
	static num::Tuint32 magicLass_;
	static std::string magicRadiance_;
	static num::Tint32 magicIgi_;
};



}

}

#endif

// EOF
