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



/** @defgroup FileAttribute FileAttribute
 *  @brief group of functions to test file attributes
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_FILE_ATTRIBUTE_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_FILE_ATTRIBUTE_H

#include "io_common.h"

namespace lass
{
namespace io
{

#if LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
const char pathSeperator = '\\';
const char pathAlternativeSeperator = '/';
const char extensionSeperator = '.';
#else
const char pathSeperator = '/';
const char pathAlternativeSeperator = '\0';
const char extensionSeperator = '.';
#endif

LASS_DLL_EXPORT bool LASS_CALL fileDoesExist(const std::string& iFileName);
LASS_DLL_EXPORT std::string LASS_CALL fileExtension(const std::string& iFileName);
LASS_DLL_EXPORT std::string LASS_CALL fileWithoutExtension(const std::string& iFileName);
LASS_DLL_EXPORT std::string LASS_CALL filePath(const std::string& iFileName);
LASS_DLL_EXPORT std::string LASS_CALL fileWithoutPath(const std::string& iFileName);

}

}

#endif

// EOF
