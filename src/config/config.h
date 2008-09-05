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



#ifndef LASS_GUARDIAN_OF_INCLUSION_CONFIG_CONFIG_H
#define LASS_GUARDIAN_OF_INCLUSION_CONFIG_CONFIG_H



/** @def LASS_PYTHON_HAS_DEBUG_BUILD
 *  @brief Define this one if your Python distribution has a debug build that can be used for the
 *      debug build of Lass.  If not, the release build of Python will be used instead.
 */
#ifndef LASS_PYTHON_HAS_DEBUG_BUILD
#	define LASS_PYTHON_HAS_DEBUG_BUILD 1
#endif


/** @def LASS_PYTHON_INHERITANCE_FROM_EMBEDDING
 *  @brief Define this one if you want to be able to derive from embedding Python objects at the cost
 *	of some extra memory per object.
 */
//#define LASS_PYTHON_INHERITANCE_FROM_EMBEDDING


/** @def #define LASS_BOOST_ALWAYS
 *  @brief Define this if you have Boost (www.boost.org) around, and you always want to fall back
 *         on it whenever possible.
 *
 *  This is when you always want to use the boost implementations of lass::util::CallTraits<>, ...
 *  Obviously, you'll have to have boost installed.
 *
 *  @warning if you neither define LASS_BOOST_ALWAYS or LASS_BOOST_IF_BROKEN, so stuff might
 *           not want to compile, or you'll have bad performance on some stuff.
 */
//#define LASS_BOOST_ALWAYS

/** @def LASS_BOOST_IF_BROKEN
 *  @brief Define this if you want to fall back on Boost (www.boost.org) for broken compilers only.
 *
 *  If you define this, we'll try to use our own implentations of lass::util::CallTraits<>, etc., but only
 *  if the compiler is willing to play it nicely.  If not, we'll fall back on boost.
 *
 *  So, if you have a nice compiler (like the intel one :), you won't need to have boost around to
 *  compile.  But if you have a broken compiler, you'll have to have boost if you want this to work.
 *
 *  @warning if you neither define LASS_BOOST_ALWAYS or LASS_BOOST_IF_BROKEN, some stuff might
 *           not want to compile, or you'll have bad performance on some stuff.
 */
#define LASS_BOOST_IF_BROKEN

// go check compiler and platform support
//
#include "compilers.h"
#include "platforms.h"



#endif
