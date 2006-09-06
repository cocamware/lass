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

/** @namespace lass::util
 *  @brief general utility, debug facilities, ...
 *
 *  The util namespace contains general utilities, debug facilities, etc. used by our lass
 *  software.  The following stuff is provided:
 *
 *  - @ref BitManip: a set of bit  manipulation routines
 *  - @ref Callback: library to wrap callback functions in first class objects.
 *  - CallTraits: defines @e best types for arguments, references, etc.
 *  - Clock: an class to measure time.
 *  - CloneFactory: implements Alexandrescu's clone factory pattern.
 *  - @ref CommonMacros: a set of common macros for your convenience
 *  - Dictionary: a bidirectional dictonary wrapper around a std::map.
 *  - empty.h: an empty header for those moments you want to include absolutely @e nothing.
 *  - @ref Enforcers: release-time counterpart of assertions to ease condition verifying
 *  - Exception: the exception class used by LASS
 *  - FixedArray: A STL compliant (english?) hybride between a C-array and a std::vector.
 *  - IdGenerator: Generates unique ID's of the type you want (well, not @e all types).
 *  - NonCopyable: boost::non_copyable
 *  - ObjectFactory: implements Alexandrescu's object factory.
 *  - @ref Process: concerning process settings.
 *  - ProgressIndicator: a class to print a progress message.
 *  - @ref Python: Python binding library
 *  - Singleton: implements the singleton idiom in a non-intrusive way, very easy.
 *  - SmallObject: base class for small objects that need a specialized allocator.
 *  - @ref SmartPtr: library of configurable smart pointers like ScopedPtr and SharedPtr
 *  - @ref stringCast: casts a value to a type by using string streams, to cast to/from strings.
 *  - @ref Threading: basic threading library
 *  - @ref ThreadFun: Uses the Callback library to run existing functions in threads
 *  - @ref VisitorPattern: implements Alexandrescu's visitor pattern.
 */



/** @namespace lass::util::impl
 *  @brief implementation details of lass::util, not for clients.
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_UTIL_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_UTIL_COMMON_H

#include "../lass_common.h"

#define LASS_LIB_UTIL LASS_LIB_PREFIX "util" LASS_LIB_SUFFIX

#if defined(LASS_LIB_AUTO_LINK) && LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
#	pragma comment(lib, LASS_LIB_UTIL)
#endif

#endif
