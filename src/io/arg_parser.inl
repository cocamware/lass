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
	description_(iDescription),
	hasDefault_(false)
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
	description_(iDescription),
	default_(iDefault),
	hasDefault_(true)
{
	checkMode(iArgMode);
}



template <typename T>
ArgValue<T>::ArgValue(ArgParser& iParser,
					  const ArgFormat& iFormat):
	ArgParameter(iParser, iFormat.shortName, iFormat.longName, iFormat.argMode),
	description_(iFormat.description),
	default_(iFormat.hasDefault ? util::stringCast<T>(iFormat.defaultValue) : T()),
	hasDefault_(iFormat.hasDefault)
{
	checkMode(iFormat.argMode);
}



/** return number of values that are stored for this argument.
 */
template <typename T>
size_t ArgValue<T>::size() const
{
	if (values_.size() == 0)
	{
		return hasDefault_ ? 1 : 0;
	}
	else
	{
		return values_.size();
	}
}



/** return iIndex'ed value.
 */
template <typename T>
typename ArgValue<T>::TConstReference ArgValue<T>::operator[](size_t iIndex) const
{
	LASS_ASSERT(iIndex < size());
	if (values_.size() == 0 && hasDefault_ && iIndex == 0)
	{
		return default_;
	}
	return values_[iIndex];
}



/** return iIndex'ed value with range check
 */
template <typename T>
typename ArgValue<T>::TConstReference ArgValue<T>::at(size_t iIndex) const
{
	if (values_.size() == 0 && hasDefault_)
	{
		if (iIndex != 0)
		{
			throw std::out_of_range("iIndex is out of range.");
		}
		return default_;
	}
	return values_.at(iIndex);
}



/** return all values
 */
template <typename T>
typename ArgValue<T>::TValues ArgValue<T>::all() const
{
	if (!values_.empty())
	{
		return values_;
	}
	TValues result;
	if (hasDefault_)
	{
		result.push_back(default_);
	}
	return result;
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
			(parserIsQuiet() ? LASS_CLOG : LASS_COUT)
				<< "Bad program arguments: you didn't provide a value for the parameter '"
				<< names() << "' as required.\n";
			if (!parserIsQuiet()) LASS_THROW_EXCEPTION(ArgBadArgument(names()));
			return false;
		}

		//LASS_LOG("parameter '" << names() << "' is set without value");
	}
	else
	{
		if (values_.size() != 0 && !(mode() & amMultiple))
		{
			(parserIsQuiet() ? LASS_CLOG : LASS_COUT)
				<< "Bad program arguments: parameter '" << names() << "' already has a value '"
				<< values_[0] << "' assigned, you can't assign another value '" << iValue
				<< "' because this parameter cannot be multiple valued.\n";
			if (!parserIsQuiet()) LASS_THROW_EXCEPTION(ArgBadArgument(names()));
			return false;
		}
		else
		{
			try
			{
				values_.push_back(util::stringCast<T>(iValue));
			}
			catch(util::Exception)
			{
				(parserIsQuiet() ? LASS_CLOG : LASS_COUT)
					<< "Bad program arguments: could not interpret '" << iValue
					<< "' as a value of type '" << typeid(T).name() << "'.\n";
				if (!parserIsQuiet()) LASS_THROW_EXCEPTION(ArgBadArgument(names()));
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
