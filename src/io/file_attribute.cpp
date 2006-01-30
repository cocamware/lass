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

#include "io_common.h"
#include "file_attribute.h"
#include "../stde/extended_string.h"

#include <stdio.h>

namespace lass
{
namespace io
{

// --- public --------------------------------------------------------------------------------------



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------



// --- free ----------------------------------------------------------------------------------------

/** @ingroup FileAttribute
 *  return true if file exists
 */
bool fileDoesExist(const std::string& iFileName)
{
	FILE* file = fopen(iFileName.c_str(), "r");
	if (file == 0)
	{
		return false;
	}
	fclose(file);
	return true;
}



/** @ingroup FileAttribute
 *  return the part of the file name behind the last dot.
 *  return an empty string if there's no dot.
 *  e.g. @e foo returns an empty string, @e foo.bar returns @e bar,
 *  @e foo.bar.fun returns @e fun.
 */
std::string fileExtension(const std::string& iFileName)
{
	std::string::size_type lastDot = iFileName.rfind(extensionSeperator);
	return lastDot!= std::string::npos ? iFileName.substr(lastDot + 1) : "";
}



/** @ingroup FileAttribute
 *  return the part of the file name before the last dot.
 *  return an original string if there's no dot.
 *  e.g. @e foo returns @e foo, @e foo.bar returns @e foo,
 *  @e foo.bar.fun returns @e foo.bar .
 */
std::string fileWithoutExtension(const std::string& iFileName)
{
	return iFileName.substr(0, iFileName.rfind(extensionSeperator));
}



/** @ingroup FileAttribute
 *  return the part of the file name before the last (back)slash.
 *  return an empty string if there's no (back)slash.
 *  e.g. @e foo returns an empty string, @e foo/bar returns @e foo,
 *  @e foo/bar/fun returns @e foo/bar.
 */
std::string filePath(const std::string& iFileName)
{
	const char seperators[] = { pathSeperator, pathAlternativeSeperator, '\0' }; 
	std::string::size_type lastSlash = iFileName.find_last_of(seperators);
	return lastSlash != std::string::npos ? iFileName.substr(0, lastSlash) : "";
}



/** @ingroup FileAttribute
 *  return the part of the file name behind the last (back)slash.
 *  return the original string if there's no (back)slash.
 *  e.g. @e foo returns @e foo, @e foo/bar returns @e bar,
 *  @e foo/bar/fun returns @e fun .
 */
std::string fileWithoutPath(const std::string& iFileName)
{
	const char seperators[] = { pathSeperator, pathAlternativeSeperator, '\0' }; 
	std::string::size_type lastSlash = iFileName.find_last_of(seperators);
	return lastSlash != std::string::npos ? iFileName.substr(lastSlash + 1) : iFileName;
}



/** @ingroup FileAttribute
 */
std::string fileJoinExtension(const std::string& iFilename, const std::string& iExtension)
{
	const char seperator [] = { extensionSeperator, '\0' };
	if (stde::begins_with(iExtension, std::string(seperator)))
	{
		return iFilename + iExtension;
	}
	return iFilename + seperator + iExtension;
}



/** @ingroup FileAttribute
 */
std::string fileJoinPath(const std::string& iPath, const std::string& iFilename)
{
	const char seperator [] = { pathSeperator, '\0' };
	if (stde::ends_with(iPath, std::string(seperator)))
	{
		return iPath + iFilename;
	}
	const char alternativeSeperator [] = { pathAlternativeSeperator, '\0' };
	if (pathAlternativeSeperator && stde::ends_with(iPath, std::string(alternativeSeperator)))
	{
		return iPath + iFilename;
	}
	return iPath + seperator + iFilename;
}

}

}

// EOF
