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
		const bool operator==(const Chromaticities& other) const 
		{
			return red == other.red && green == other.green && blue == other.blue && white == other.white;
		}
		const bool operator!=(const Chromaticities& other) const { return !(*this == other); }
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
	void convertChromaticities(const Chromaticities& newChromaticities);

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

	void clampNegatives();

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

	unsigned resize(unsigned rows, unsigned cols);
	unsigned flatIndex(unsigned rows, unsigned cols) const 
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

	static const Chromaticities defaultChromaticities();
	static const Chromaticities xyzChromaticities();

	Chromaticities chromaticities_;
	unsigned rows_;
	unsigned cols_;
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
