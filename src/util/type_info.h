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

/** @class lass::util::TypeInfo
 *  @brief first class wrapper around std::type_info
 *  @author Bram de Greve [Bramz]
 *
 *  @code
 *  std::set<TypeInfo> types;
 *
 *  types.insert(typeid(int));
 *  types.insert(typeid(float));
 *  types.insert(typeid(std::string));
 *  types.insert(typeid(FooBar));
 *  types.insert(typeid(Eggs));
 *  types.insert(typeid(Spam));
 *  @endcode
 *
 *  @par original code by Andrei Alexandrescu
 *  The Loki Library, Copyright (c) 2001 by Andrei Alexandrescu\n
 *  This code (Loki) accompanies the book:\n
 *  Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design Patterns Applied".
 *  Copyright (c) 2001. Addison-Wesley.\n
 *  Permission to use, copy, modify, distribute and sell this software (Loki) for any purpose is
 *  hereby granted without fee, provided that the above copyright notice appear in all copies and
 *  that both that copyright notice and this permission notice appear in supporting documentation.\n
 *  The author or Addison-Wesley Longman make no representations about the suitability of this
 *  software (Loki) for any purpose. It is provided "as is" without express or implied warranty.\n
 */


#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_TYPE_INFO_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_TYPE_INFO_H

#include "util_common.h"
#include "../meta/null_type.h"
#include <typeinfo>

namespace lass
{
namespace util
{

class LASS_DLL_EXPORT TypeInfo
{
public:

	TypeInfo();
	TypeInfo(const std::type_info& iType_info);

	const std::type_info& type_info() const;

	bool before(const TypeInfo& a_other) const;
	const char* name() const;

private:

	const std::type_info* type_info_;

};

LASS_DLL_EXPORT bool operator==(const TypeInfo& iA, const TypeInfo& iB);
LASS_DLL_EXPORT bool operator!=(const TypeInfo& iA, const TypeInfo& iB);
LASS_DLL_EXPORT bool operator<(const TypeInfo& iA, const TypeInfo& iB);
LASS_DLL_EXPORT bool operator>(const TypeInfo& iA, const TypeInfo& iB);
LASS_DLL_EXPORT bool operator<=(const TypeInfo& iA, const TypeInfo& iB);
LASS_DLL_EXPORT bool operator>=(const TypeInfo& iA, const TypeInfo& iB);

LASS_DLL_EXPORT std::ostream& operator<<(std::ostream& iStream, const TypeInfo& iTypeInfo);

}

}

#endif

// EOF
