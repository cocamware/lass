/** 
*   @author Bram de Greve (bramz@users.sourceforge.net)
*   @author Tom De Muer (tomdemuer@users.sourceforge.net)
*
*	Distributed under the terms of the GPL (GNU Public License)
*
* 	The LASS License:
*   
*	Copyright 2004 Bram de Greve and Tom De Muer
*
*   LASS is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*


 *  @date 2004
 */

#include "io_common.h"
#include "avi.h"

// Win32 implementation
//
#ifdef LASS_IO_AVI_WIN32

#include "../prim/color_rgba.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "impl/easyAVI.h"

#pragma comment(lib, "vfw32.lib")

namespace lass
{
namespace io
{

// --- public --------------------------------------------------------------------------------------

/** construct an Avi object without opening any file.
 */
Avi::Avi():
	pimpl_(new impl::easyAVI)
{
}



/** Construct an Avi object and open the file
 *  @param iFileName file name of AVI to open
 *  @param iFps frames per second of AVI
 *  @param iWidth width of one frame of AVI (measured in pixels)
 *  @param iHeight height of one frame of AVI (measured in pixels)
 *  @param iBpp bits per pixel of AVI (valid values are 16, 24 and 32).
 */
Avi::Avi(const std::string& iFileName, int iFps, int iWidth, int iHeight, int iBpp):
	pimpl_(new impl::easyAVI)
{
	open(iFileName, iFps, iWidth, iHeight, iBpp);
}



/** close AVI if open
 */
Avi::~Avi()
{
	close();
}



/** create AVI file to write to.
 *  @param iFileName file name of AVI to open
 *  @param iFps frames per second of AVI
 *  @param iWidth width of one frame of AVI (measured in pixels)
 *  @param iHeight height of one frame of AVI (measured in pixels)
 *  @param iBpp bits per pixel of AVI (valid values are 24).
 */
void Avi::open(const std::string& iFileName, int iFps, int iWidth, int iHeight, int iBpp)
{
	if (isOpen())
	{
		LASS_THROW("Invalid call: Avi is already open.");
	}

	if (iFps <= 0 || iWidth <= 0 || iHeight <= 0)
	{
		LASS_THROW("Invalid parameters: one of the values iFps '" << iFps << "', iWidth '" 
			<< iWidth << "' or iHeight '" << iHeight << "' is less than or equal to zero.");
	}
	if (iBpp != 24)
	{
		LASS_THROW("Invalid parameter iBpp '" << iBpp << "': should be 24.");
	}
	
	fps_ = fps_;
	width_ = iWidth;
	height_ = iHeight;
	bpp_ = iBpp;

	LASS_ENFORCE_POINTER(reinterpret_cast<impl::easyAVI*>(pimpl_))->openAVI(
		const_cast<char*>(iFileName.c_str()), iFps, iWidth, iHeight);
}	



/** close the AVI file
 */
void Avi::close()
{
	buffer_.reset();
	LASS_ENFORCE_POINTER(reinterpret_cast<impl::easyAVI*>(pimpl_))->closeAVI();
}



/** return true if this object is associated with an open AVI file
 */
bool Avi::isOpen() const
{
	return LASS_ENFORCE_POINTER(reinterpret_cast<impl::easyAVI*>(pimpl_))->isOpen();
}



/** return number of frames per second (only valid if isOpen() yields true).
 */
int Avi::fps() const
{
	return fps_;
}



/** return width of frame in pixels (only valid if isOpen() yields true).
 */
int Avi::width() const
{
	return width_;
}



/** return height of frame in pixels (only valid if isOpen() yields true).
 */
int Avi::height() const
{
	return height_;
}



/** return bit depth of pixels in frame (only valid if isOpen() yields true).
 */
int Avi::bpp() const
{
	return bpp_;
}



/** add new frame, by array of 32bit XRGB pixels (X not used).
 */
void Avi::frame(const TRaw32* iFrame)
{
	LASS_ENFORCE_POINTER(reinterpret_cast<impl::easyAVI*>(pimpl_))->addFrame(
		const_cast<TRaw32*>(iFrame));
}



/** add new frame, by array prim::ColorRGBA
 */
void Avi::frame(const prim::ColorRGBA* iFrame)
{
	const int size = width_ * height_;

	if (!buffer_)
	{
		buffer_.reset(new TRaw32[size]);
	}
	
	for (int i = 0; i < size; ++i)
	{
		buffer_[i] = (static_cast<TRaw32>(iFrame[i].r * 255.0f) << 16) +
					 (static_cast<TRaw32>(iFrame[i].g * 255.0f) << 8) +
					 (static_cast<TRaw32>(iFrame[i].b * 255.0f));
	}
	LASS_ENFORCE_POINTER(reinterpret_cast<impl::easyAVI*>(pimpl_))->addFrame(buffer_.get());
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------



}

}

#endif // LASS_IO_AVI_WIN32

// EOF
