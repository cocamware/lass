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



/** @class lass::io::Avi
 *  @brief writes raw data to avi files
 *  @author Bram de Greve [BdG]
 *
 *  You can use this class to stream images into an AVI file (sound is not supported). The use
 *  of this Avi thing is simply, yet buggy (more on that later, let's examine the simple part
 *  first).
 *
 *  @section overview
 *
 *  To start writing to an AVI, you simply construct an Avi object and you the @c open function
 *  while passing its filename, the frames per second you want, frame width in pixels, frame height
 *  in pixels, and the wanted bit depth of each pixel.  Then to write a frame to the AVI, you fill
 *  a <em>frame buffer</em> with the appropriate pixels, and you call the @c frame function of the
 *  Avi object.  After you've written all frames, you can close the file manually by calling @c
 *  close.
 *
 *  @code
 *  const int width = 320;
 *  const int height = 240;
 *  util::ScopedPtr<prim::ColorRGBA, ArrayStorage> buffer(new prim::ColorRGBA[width * height]);
 *  Avi avi;
 *
 *  LASS_COUT << "SPECIFY AVI FORMAT IN DIALOG!\n";
 *  avi.open("foo.avi", 25, width, height, 24);
 *  for (int frame = 0; frame < 50; ++frame)
 *  {
 *      for (int y = 0; y < height; ++y)
 *      {
 *          for (int x = 0; x < width; ++x)
 *          {
 *              const int i = y * width + x;
 *              buffer[i].r = static_cast<float>(rand()) / RAND_MAX;
 *              buffer[i].g = static_cast<float>(rand()) / RAND_MAX;
 *              buffer[i].b = static_cast<float>(rand()) / RAND_MAX;
 *          }
 *      }
 *      avi.frame(buffer.get());
 *  }
 *  avi.close();
 *  @endcode
 *
 *  You can also open the Avi in the constructor, and you can skip the manual @c close with the
 *  knowledge that the Avi destructor will do it for you instead.
 *
 *  A <em>frame buffer</em> is an array you allocate yourself in main memory.  It has to be able
 *  to contain enough pixels for one frame, thus it has to be of length @e width * @e height.
 *  Frame buffer are row-major with the origin in the upper left corner, meaning that the pixels
 *  are stored line by line, left to right, top to bottom.
 *
 *  You can use two kinds of frame buffers.  The first is a raw 32-bit integer buffer where
 *  every pixel is stored in a 32 bit integer.  The second is a more general prim::ColorRGBA buffer
 *  where every pixel is represented by a prim::ColorRGBA object.  The latter will consume more
 *  memory (128 bits per pixel instead of 32 bits), but it will move the burden of converting
 *  color to the right raw format to the Avi class.  Otherwise, you'll have to do it yourself.
 *
 *  @section gory_details the gory details
 *
 *  @note currently only bit depths of 24 bpp are supported (so, don't attempt to try any other).
 *
 *  @note though the raw buffer format uses a 32 bit integer per pixel, this does NOT mean you're
 *        supposed to enter the pixels in the 32 bit ARGB format!  It only means you have 32 bits
 *        to store your pixel in whatever format you've specified.  If you use 24 bpp (which is
 *        currently the only available format), this means you should only use the lower 24 bits
 *        of the 32 bit integer to store your thing.
 *
 *  @bug You have to be very lucky to create an AVI, so it seems.  It has somehting to do with
 *       the width and height and multiples of eight.   Anyway, it seems that only a few sizes
 *       are supported, and it is not well know which ones.  If you choose the wrong sizes, it has
 *       as effect that the file is written but you will not be able to open in in media player
 *       (or winamp, ...) saying that the file has an unsupported format.
 *
 *  @bug oh, you might notice the compression settings don't work.
 *
 *  @bug It seems that the file is not cleared before opening.  This means that if a file with the
 *       same name already exists, its filesize will not be reduced to zero, with effect that if
 *       the old file was bigger in size than the new one, the file size will remeain the old one.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_AVI_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_AVI_H

#include "io_common.h"
#include "../util/scoped_ptr.h"

// check platform support
//
#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#	define LASS_IO_AVI_WIN32
#else
#	pragma LASS_IO_AVI_NO_SUPPORT
#	pragma LASS_NOTE("COMPATIBILTY WARNING: lass::io::Avi is not supported on this platform")
#endif

#if !defined(LASS_IO_AVI_NO_SUPPORT)

namespace lass
{

// old interfaces

namespace prim
{
class ColorRGBA;
};


// new interfaces

namespace io
{

class LASS_DLL_EXPORT Avi
{
public:

	typedef unsigned int TRaw32; /**< 32 bit word for raw frame buffer */

	Avi();
	Avi(const std::string& iFileName, int iFps, int iWidth, int iHeight, int iBpp = 24);
	~Avi();

	void open(const std::string& iFileName, int iFps, int iWidth, int iHeight, int iBpp = 24);
	void close();
	bool isOpen() const;

	int fps() const;
	int width() const;
	int height() const;
	int bpp() const;

	void frame(const TRaw32* iFrameBuffer);
	void frame(const prim::ColorRGBA* iFrameBuffer);

private:

	void* pimpl_;
	util::ScopedPtr<TRaw32, util::ArrayStorage> buffer_;
	int fps_;
	int width_;
	int height_;
	int bpp_;
};



}

}

#endif

#endif

// EOF
