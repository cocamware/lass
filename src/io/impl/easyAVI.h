/** @file
 *  @author originally written by Thomas Erich Ludwig (thomasludwig@telkomsa.net)
 *  @author modified by Bram de Greve (bramz@users.sourceforge.net)
 *  @author modified by Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *  [Bramz] This file is origally written by Thomas Ludwig and is kindly provided by him so
 *  we can include AVI support in lass.  see lass::io::Avi.

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



/** @class lass::io::impl::easyAVI
 *  @brief pimpl for lass::io::Avi.
 *  @author originally written by Thomas Erich Ludwig (thomasludwig@telkomsa.net,
 *          http://www.lycium.cfxweb.net)
 *  @author modified by Bram de Greve [Bramz]
 *
 *  [Bramz] This class is origally written by Thomas Ludwig and is kindly provided by him so
 *  we can include AVI support in lass.  I adapted this file a little to be better absorbed
 *  by the lass library.
 *
 *  @warning This class is @e only to be included by lass/io/avi.cpp.  It is to be seen as an
 *  implementation of the class lass::io::Avi (hence the namespace impl :).  lass::io::Avi
 *  uses the pimpl idiom to use this easyAVI class. i.e. lass::io::Avi completely encapsulates
 *  easyAVI, and nothing in the interface (header) of lass::io::Avi gives access to this
 *  file.  In short: don't include this file yourself, but use lass::io::Avi instead.
 *
 *  Things that I have changed:
 *  - surrounding by namespace lass::io::impl
 *  - replaced @c printf by @c LASS_WARNING'
 *  - added the @c isOpen method
 *  - added this comment
 *  - IMPORTANT ONE: flipped y-direction of frame buffer.  origin of buffer is now in upper-
 *    left corner.
 */

#include <vfw.h>

#include "../../util/common_macros.h"

namespace lass
{
namespace io
{
namespace impl
{

class easyAVI
{
public:
	easyAVI();
	void openAVI(char *filename, int framerate, int xr, int yr);
	void closeAVI();
	void addFrame(unsigned int *frame);
	bool isOpen() const { return opened; }
private:
	int xres, yres, fps, alignedsize, currentFrame;
	bool opened;

	unsigned char *alignedFrame;

	PAVIFILE aviFile;
	PAVISTREAM aviStream, compressedAviStream;

	LPBITMAPINFOHEADER img1;
	unsigned char *img1p;
};

easyAVI::easyAVI()
{
	aviFile = NULL;
	aviStream = compressedAviStream = NULL;
	opened = false;
	currentFrame = 0;
}

void easyAVI::openAVI(char *filename, int framerate, int xr, int yr)
{
	AVISTREAMINFO streamInfo;
	AVICOMPRESSOPTIONS options;
	AVICOMPRESSOPTIONS FAR *aviOptions[1] = { &options };
	HRESULT result;

	if (opened) return;

	AVIFileInit();

	result = AVIFileOpen(&aviFile,filename,OF_WRITE | OF_CREATE,NULL);
	if (result != AVIERR_OK)
	{
		LASS_WARNING("Error: AVIFileOpen failed.\n");
		closeAVI();
		return;
	}

	memset(&streamInfo,0,sizeof(streamInfo));
	streamInfo.fccType = streamtypeVIDEO;
	streamInfo.fccHandler = 0;
	streamInfo.dwScale = 1;
	streamInfo.dwRate = framerate;
	streamInfo.dwSuggestedBufferSize = xr * yr * 3;
	SetRect(&streamInfo.rcFrame,0,0,xr,yr);

	result = AVIFileCreateStream(aviFile,&aviStream,&streamInfo); if (result != AVIERR_OK) closeAVI();

	memset(&options,0,sizeof(options));
	if (!AVISaveOptions(NULL,0,1,&aviStream,(LPAVICOMPRESSOPTIONS FAR *)&aviOptions))
	{
		LASS_WARNING("Error: AVISaveOptions failed.\n");
		closeAVI();
		return;
	}

	xres = xr; yres = yr;
	fps = framerate;

	alignedsize = xres * 3;
	//if ((alignedsize & 1) != 0) alignedsize += 2 - (alignedsize & 1);
	if ((alignedsize & 3) != 0) alignedsize += 4 - (alignedsize & 3); // added by [Bramz]

	alignedFrame = new unsigned char[alignedsize * yres + sizeof(BITMAPINFOHEADER) + 1024];

	img1 = (LPBITMAPINFOHEADER)alignedFrame;
	img1->biSize = sizeof(BITMAPINFOHEADER);
	img1->biWidth = xr;
	img1->biHeight = yr;
	img1->biPlanes = 1;
	img1->biBitCount = 24;
	img1->biCompression = BI_RGB;
	img1->biSizeImage = 0;//img1->biWidth * img1->biHeight * 3;
	img1->biXPelsPerMeter = 0;
	img1->biYPelsPerMeter = 0;
	img1->biClrUsed = 0;
	img1->biClrImportant = 0;
	img1p = (unsigned char FAR *)((LPBYTE)img1 + (int)img1->biSize);

	result = AVIMakeCompressedStream(&compressedAviStream,aviStream,&options,NULL); if (result != AVIERR_OK) closeAVI();
	result = AVIStreamSetFormat(compressedAviStream,0,(void *)alignedFrame,img1->biSize); if (result != AVIERR_OK) closeAVI();
	opened = true;
}

void easyAVI::closeAVI()
{
	if (!opened) return;

	if (aviStream) AVIStreamClose(aviStream);
	if (compressedAviStream) AVIStreamClose(compressedAviStream);
	if (aviFile) AVIFileClose(aviFile);
	AVIFileExit();

	delete [] alignedFrame;
	opened = false;
}

void easyAVI::addFrame(unsigned int *frame)
{
	if (!opened)
	{
		LASS_WARNING("Error: trying to add frame to unopened AVI file.\n");
		return;
	}

	for (int y = 0; y < yres; y++)
	{
		const int x0 = (yres - y - 1) * xres; // added by [Bramz]
		for (int x = 0; x < xres; x++)
		{
			img1p[y * alignedsize + x * 3] = frame[x0 + x] & 0x000000ff;
			img1p[y * alignedsize + x * 3 + 1] = (frame[x0 + x] & 0x0000ff00) >> 8;
			img1p[y * alignedsize + x * 3 + 2] = (frame[x0 + x] & 0x00ff0000) >> 16;
		}
	}

	HRESULT result = AVIStreamWrite(compressedAviStream,currentFrame++,1,(void *)img1p,alignedsize * yres,AVIIF_KEYFRAME,NULL,NULL);
	if (result != AVIERR_OK)
	{
		LASS_WARNING("Error: AVIStreamWrite failed.\n");
		closeAVI();
	}
}

}

}

}
