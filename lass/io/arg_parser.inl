/**	@file
 *	@author Bram de Greve (bram@cocamware.com)
 *	@author Tom De Muer (tom@cocamware.com)
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
 *	Copyright (C) 2004-2009 the Initial Developer.
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

#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_ARG_PARSER_INL
#define LASS_GUARDIAN_OF_INCLUSION_IO_ARG_PARSER_INL

#include "io_common.h"
#include "arg_parser.h"

#include "../util/string_cast.h"

namespace lass
{
namespace io
{

// --- ArgValue ------------------------------------------------------------------------------------

template <typename T>
ArgValue<T>::ArgValue(ArgParser& iParser,
					  const std::string& iShortName,
					  const std::string& iLongName,
					  const std::string& iDescription,
					  int iArgMode):
	ArgParameter(iParser, iShortName, iLongName, iArgMode),
	description_(iDescription)
{
	checkMode(iArgMode);
}



template <typename T>
ArgValue<T>::ArgValue(ArgParser& iParser,
					  const std::string& iShortName,
					  const std::string& iLongName,
					  const std::string& iDescription,
					  int iArgMode,
					  TParam iDefault):
	ArgParameter(iParser, iShortName, iLongName, iArgMode),
	default_(1, iDefault),
	description_(iDescription)
{
	checkMode(iArgMode);
}



template <typename T>
ArgValue<T>::ArgValue(ArgParser& iParser,
					  const ArgFormat& iFormat):
	ArgParameter(iParser, iFormat.shortName, iFormat.longName, iFormat.argMode),
	description_(iFormat.description)
{
	if (iFormat.hasDefault)
	{
		default_.push_back(util::stringCast<T>(iFormat.defaultValue));
	}
	checkMode(iFormat.argMode);
}



/** return all values
 */
template <typename T>
const typename ArgValue<T>::TValues& ArgValue<T>::all() const
{
	return !values_.empty() ? values_ : default_;
}



/** return number of values that are stored for this argument.
 */
template <typename T>
size_t ArgValue<T>::size() const
{
	return all().size();
}



/** return iIndex'ed value.
 */
template <typename T>
typename ArgValue<T>::TConstReference ArgValue<T>::operator[](size_t iIndex) const
{
	LASS_ASSERT(iIndex < size());
	return all()[iIndex];
}



/** return iIndex'ed value with range check
 */
template <typename T>
typename ArgValue<T>::TConstReference ArgValue<T>::at(size_t iIndex) const
{
	return all().at(iIndex);
}



/** return iterator to first value
 */
template <typename T>
typename ArgValue<T>::TValueIterator ArgValue<T>::begin() const
{
	return all().begin();
}



/** return iterator to last value
 */
template <typename T>
typename ArgValue<T>::TValueIterator ArgValue<T>::end() const
{
	return all().end();
}



/** check if mode is valid
 *  amNoValue can't be set, and exactly one of amOptional or amRequired must be set.
 */
template <typename T>
void ArgValue<T>::checkMode(int iArgMode) const
{
	if (iArgMode & amNoValue)
	{
		LASS_THROW("You can't set the mode 'amNoValue' for a ArgValue '" << names() << "'.");
	}
	const int mask = amOptional | amRequired;
	const int field = iArgMode & mask;
	if (field != amOptional && field != amRequired)
	{
		LASS_THROW("For a ArgValue '" << names() << "' you must choose between mode 'amOptional' "
			<< "or 'amRequired'.  At least one of these must be set, but not both.");
	}
}



template <typename T>
const std::string ArgValue<T>::doFormat() const
{
	LASS_ASSERT(!(mode() & amNoValue));

	std::ostringstream result;
	result << "[" << names() << " ";

	if (mode() & amOptional)
	{
		result << "[";
	}
	result << "<" << (description_.empty() ? "value" : description_) << ">";
	if (mode() & amMultiple)
	{
		result << " ...";
	}
	if (mode() & amOptional)
	{
		result << "]";
	}

	result << "]";
	return result.str();
}



template <typename T>
const bool ArgValue<T>::doSetValue(const std::string& iValue)
{
	LASS_ASSERT(!(mode() & amNoValue));

	if (iValue == "")
	{
		if (mode() & amRequired)
		{
			if (!parserIsQuiet())
			{
				LASS_THROW_EX(ArgBadArgument,
					"Bad program arguments: you didn't provide a value for the parameter '"
					<< names() << "' as required.\n");
			}
			return false;
		}

		//LASS_LOG("parameter '" << names() << "' is set without value");
	}
	else
	{
		if (values_.size() != 0 && !(mode() & amMultiple))
		{
			if (!parserIsQuiet())
			{
				LASS_THROW_EX(ArgBadArgument,
					"Bad program arguments: parameter '" << names() << "' already has a value '"
					<< values_[0] << "' assigned, you can't assign another value '" << iValue
					<< "' because this parameter cannot be multiple valued.\n");
			}
			return false;
		}
		else
		{
			try
			{
				values_.push_back(util::stringCast<T>(iValue));
			}
			catch (const util::BadStringCast&)
			{
				if (!parserIsQuiet())
				{
					LASS_THROW_EX(ArgBadArgument,
						"Bad program arguments: could not interpret '" << iValue
						<< "' as a value of type '" << typeid(T).name() 
						<< "' for parameter '" << names() << ".\n");
				}
				return false;
			}

			//LASS_LOG("parameter '" << names() << "' is set, value == '" << values_.back() << "'");
		}
	}

	set();
	return true;
}



}

}

#endif

// EOF
