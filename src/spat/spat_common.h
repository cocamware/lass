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



/** @namespace lass::spat
 *  @brief spatial subdivisions, quadtrees, octrees, meshes in 2D and 3D, triangulators, ...
 *
 *  @section overview
 *
 *  @ref lass::spat::world_3d:: the templated version of the original @c World3 model in @c bass3
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_SPAT_SPAT_COMMON_H
#define LASS_GUARDIAN_OF_INCLUSION_SPAT_SPAT_COMMON_H




#include "../lass_common.h"

///** name of library
// */
//#define LASS_LIB_SPAT LASS_LIB_PREFIX "spat" LASS_LIB_SUFFIX
//
//#if defined(LASS_LIB_AUTO_LINK) && LASS_PLATFORM_TYPE == LASS_PLATFORM_TYPE_WIN32
//#	pragma comment(lib, LASS_LIB_SPAT)
//#endif


namespace lass
{
namespace spat
{

#ifdef LASS_SPAT_OBJECT_TREES_DIAGNOSTICS

namespace impl
{
	template <typename InfoType>
	struct ObjectTreesDiagnostics
	{
		ObjectTreesDiagnostics(InfoType* info): info(info), visitedObjects(0), visitedNodes(0) {}

		const InfoType* info;
		size_t visitedObjects;
		size_t visitedNodes;
	};
}

#	define LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(t_info, v_info)\
		::lass::spat::impl::ObjectTreesDiagnostics< t_info >* _lass_spat_object_trees_diagnostics =\
			(::lass::spat::impl::ObjectTreesDiagnostics< t_info >*) v_info;\
		if (_lass_spat_object_trees_diagnostics) {\
			v_info = _lass_spat_object_trees_diagnostics->info;\
			_lass_spat_object_trees_diagnostics->visitedNodes += 1;\
		} else {}

#	define LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_NODE\
		if (_lass_spat_object_trees_diagnostics) { _lass_spat_object_trees_diagnostics->visitedNodes += 1; } else {}

#	define LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT\
		if (_lass_spat_object_trees_diagnostics) { _lass_spat_object_trees_diagnostics->visitedObjects += 1; } else {}

#else

#	define LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_INIT_NODE(t_info, v_info)
#	define LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_NODE
#	define LASS_SPAT_OBJECT_TREES_DIAGNOSTICS_VISIT_OBJECT

#endif

}
}

#endif

// EOF
