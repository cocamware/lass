/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
 *
 *	Distributed under the terms of the GPL (GNU Public License)
 *
 * 	The LASS License:
 *
 *	Copyright 2004 Bram de Greve and Tom De Muer
 *
 *	LASS is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



/** @class lass::util::NonCopyable
 *  @brief boost::non_copyable, base class to make types non copyable
 *  @author Bram de Greve [Bramz].
 *
 *  These is copied from the boost libary, and is original written by Dave Abrahams
 *  (boost/non_copyable.hpp).
 *
 *  <i>(C) Copyright Boost.org 1999-2003. Permission to copy, use, modify, sell and distribute this
 *  software is granted provided this copyright notice appears in all copies. This software is 
 *  provided "as is" without express or implied warranty, and with no claim as to its suitability 
 *  for any purpose.</i>
 *
 *  @section documentation
 *
 *  Let's copy from boost what Dave Abrahams has to say about it 
 *  (http://www.boost.org/libs/utility/utility.htm)
 *
 *  <i>Class noncopyable is a base class.  Derive your own class from noncopyable when you want to
 *  prohibit copy construction and copy assignment.
 *
 *  Some objects, particularly those which hold complex resources like files or network connections,
 *  have no sensible copy semantics.  Sometimes there are possible copy semantics, but these would 
 *  be of very limited usefulness and be very difficult to implement correctly.  Sometimes you're 
 *  implementing a class that doesn't need to be copied just yet and you don't want to take the 
 *  time to write the appropriate functions.  Deriving from noncopyable will prevent the otherwise
 *  implicitly-generated functions (which don't have the proper semantics) from becoming a trap for
 *  other programmers.
 *
 *  The traditional way to deal with these is to declare a private copy constructor and copy 
 *  assignment, and then document why this is done.  But deriving from noncopyable is simpler and 
 *  clearer, and doesn't require additional documentation.
 *
 *  <b>Rationale:</b> Class noncopyable has protected constructor and destructor members to 
 *  emphasize that it is to be used only as a base class.  Dave Abrahams notes concern about the 
 *  effect on compiler optimization of adding (even trivial inline) destructor declarations. He 
 *  says "Probably this concern is misplaced, because noncopyable will be used mostly for classes 
 *  which own resources and thus have non-trivial destruction semantics."</i>
 *
 *  @code
 *  #include <lass/util/non_copyable.h>
 *  
 *  class MyNonCopyableManager: public lass::util::NonCopyable
 *  {
 *		// ...
 *  };
 *  @endcode
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_NON_COPYABLE_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_NON_COPYABLE_H

#include "util_common.h"

namespace lass
{
namespace util
{

class LASS_DLL NonCopyable
{
public:
    NonCopyable() {}
    ~NonCopyable() {}
private:
    NonCopyable( const NonCopyable& iOther );
    NonCopyable& operator=( const NonCopyable& iOther );
};



}

}

#endif

// EOF
