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

#include "util_common.h"
#include "type_info.h"

namespace lass
{
namespace util
{

/** construct an Null type info object.
 *  it stores info on lass::meta::NullType, as to say: there's no info in this TypeInfo.
 */
TypeInfo::TypeInfo():
	type_info_(&typeid(meta::NullType))
{
	LASS_ASSERT(type_info_);
}



/** Implicit conversion constructor of std::type_info to lass::util::TypeInfo
 */
TypeInfo::TypeInfo(const std::type_info& iType_info):
	type_info_(&iType_info)
{
	LASS_ASSERT(type_info_);
}



/** return reference to std::type_info object, used by this TypeInfo object.
 */
const std::type_info& TypeInfo::type_info() const
{
	LASS_ASSERT(type_info_);
	return *type_info_;
}



/** return true if this TypeInfo comes before the other one (conform std::type_info::before).
 */
bool TypeInfo::before(const TypeInfo& iOther) const
{
	LASS_ASSERT(type_info_);
	return type_info_->before(*iOther.type_info_) != 0;
}



/** return name of type (conform std::type_info::name)
 */
const char* TypeInfo::name() const
{
	LASS_ASSERT(type_info_);
	return type_info_->name();
}



/** return true if both TypeInfo objects refer to the same type.
 *  @relates TypeInfo
 */
bool operator==(const TypeInfo& iA, const TypeInfo& iB)
{
	return (iA.type_info() == iB.type_info()) != 0;
}



/** return true if iA and iB refer to different types.
 *  @relates TypeInfo
 */
bool operator!=(const TypeInfo& iA, const TypeInfo& iB)
{
	return !(iA == iB);
}



/** return true if iA comes before iB.
 *  @relates TypeInfo
 */
bool operator<(const TypeInfo& iA, const TypeInfo& iB)
{
	return iA.before(iB);
}



/** return true if iB comes before iA.
 *  @relates TypeInfo
 */
bool operator>(const TypeInfo& iA, const TypeInfo& iB)
{
	return iB < iA;
}



/** return true if iB does not come before iA.
 *  @relates TypeInfo
 *  same as (iA comes before iB) or (iA and iB are identical).
 */
bool operator<=(const TypeInfo& iA, const TypeInfo& iB)
{
	return !(iB < iA);
}



/** return true if iA does not come before iB.
 *  @relates TypeInfo
 *  same as (iB comes before iA) or (iA and iB are identical).
 */
bool operator>=(const TypeInfo& iA, const TypeInfo& iB)
{
	return !(iA < iB);
}



/** @relates TypeInfo
 */
std::ostream& operator<<(std::ostream& iStream, const TypeInfo& iTypeInfo)
{
	iStream << iTypeInfo.name();
	return iStream;
}



}

}

// EOF
