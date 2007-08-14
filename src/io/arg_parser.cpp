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

#include "io_common.h"
#include "arg_parser.h"

#include <locale>

namespace lass
{
namespace io
{

// --- ArgParser ------------------------------------------------------------------------------

/** Construct a quiet parser.
 *  A quiet parser won't write anything to the output (it will still log though), and it will
 *  throw as few exceptions as possible
 */
ArgParser::ArgParser():
	programName_(""),
	programVersion_(""),
	positionals_(""),
	isQuiet_(true)
{
}



/** Construct a not so quiet parser.
 *  On wrong input, it will write info to the standard input and will throw some more exceptions.
 */
ArgParser::ArgParser(const std::string& iProgramName,
					 const std::string& iProgramVersion,
					 const std::string& iPositionalArguments):
	programName_(iProgramName),
	programVersion_(iProgramVersion),
	positionals_(iPositionalArguments),
	isQuiet_(false)
{
}



/** parse arguments
 *  @note in contrary like @c argc/argv , you shouldn't store the program name as the first
 *        argument, since the parser will start with @c iArguments[0] .  Hence, the parser would
 *        see the program name as the first positional if you did so.
 */
bool ArgParser::parse(const TArguments& iArguments, TArguments* oPositionals)
{
	bool result = true;
	bool allPositionals = false;

	for (TArguments::size_type i = 0; i < iArguments.size(); ++i)
	{
		std::string arg = iArguments[i];
		if (arg.length() > 0)
		{
			if (arg[0] == '-' && !allPositionals)
			{
				if (arg.length() > 1)
				{
					if (arg[1] == '-')
					{
						if (arg.length() > 2)
						{
							// --longOption
							result &= parseLong(iArguments, i);
						}
						else
						{
							// --
							allPositionals = true;
						}
					}
					else
					{
						// -shortOptions
						result &= parseShort(iArguments, i);
					}
				}
				else
				{
					// -  what qbout it?
				}
			}
			else
			{
				if (oPositionals)
				{
					oPositionals->push_back(arg);
				}
			}
		}
	}

	return true;
}



/** parse arguments from commandline
 *  @note we suppose @c iArgv[0] is the program name and will skipped by the parser.
 *        the parser will only take care of @c iArgv[1] and the following!
 */
bool ArgParser::parse(int iArgc, char* iArgv[], TArguments* oPositionals)
{
	TArguments arguments;
	std::copy(iArgv + 1, iArgv + iArgc, std::back_inserter(arguments));

	return parse(arguments, oPositionals);
}



/** parse arguments that come by a string.
 *  break string based on whitespace unless it's surrounded by quotes.
 *  @note You shouldn't start the string with the program name like argc/argv do.  The string
 *        will be parsed from the beginning, so the program name would be seen as the first
 *        positional if you did.
 */
bool ArgParser::parse(const std::string& iArguments, TArguments* oPositionals)
{
	const std::string whitespace = " \t\n";
	const std::string quotes = "\"";
	const std::string escapes = "\\";

	//LASS_EVAL(iArguments);

	TArguments result;

	std::string token;
	bool isQuoted = false;
	char quote = '#';
	bool isEscaped = false;
	char escape = '#';

	for (std::string::const_iterator ch = iArguments.begin(); ch != iArguments.end(); ++ch)
	{
		if (whitespace.find(*ch) != std::string::npos && !isQuoted)
		{
			if (!token.empty())
			{
				result.push_back(token);
				token.clear();
			}
		}
		else if (quotes.find(*ch) != std::string::npos)
		{
			if (isEscaped)
			{
				token.append(1, *ch);
				isEscaped = false;
			}
			else
			{
				if (isQuoted)
				{
					if (quote == *ch)
					{
						isQuoted = false;
					}
				}
				else
				{
					quote = *ch;
					isQuoted = true;
				}
			}
		}
		else if (escapes.find(*ch) != std::string::npos)
		{
			if (isEscaped)
			{
				token.append(1, *ch);
				isEscaped = false;
			}
			else
			{
				escape = *ch;
				isEscaped = true;
			}
		}
		else
		{
			if (isEscaped)
			{
				token.append(1, escape);
				isEscaped = false;
			}
			token.append(1, *ch);
		}
	}

	result.push_back(token);
	return parse(result, oPositionals);
}



std::string ArgParser::usage() const
{
	std::ostringstream result;

	result << "usage:";
	if (!programName_.empty())
	{
		result << " " << programName_;
	}

	result << " [-v|--version] [-h|--help]";

	for (TParameters::const_iterator pit = parameters_.begin(); pit != parameters_.end(); ++pit)
	{
		result << " " << (*pit)->format();
	}

	if (!positionals_.empty())
	{
		result << " " << positionals_;
	}

	return result.str();
}




void ArgParser::subscribe(ArgParameter& iParameter)
{
	const std::string& shortName = iParameter.shortName();
	const std::string& longName = iParameter.longName();

	// we need at least a short or a long option name.
	//
	if (shortName.empty() && longName.empty())
	{
		LASS_THROW("Subscribing parameter failed because both the short and the long names are "
			"empty what makes it impossible to identify the parameter.");
	}

	// short names cannot be long and must be alphanumerical
	//
	if (!shortName.empty())
	{
		if (shortName.length() != 1 || !std::isalnum(shortName[0], std::locale()))
		{
			LASS_THROW("Subscribing parameter failed because the short name is not a single "
				"alpha-numeric character.");
		}
	}

	if (!isValidLongName(longName))
	{
		LASS_THROW("Subscribing parameter failed because the long name isn't valid.");
	}

	// both short and long names must be unique (if their not zero length)
	//
	for (TParameters::const_iterator pit = parameters_.begin(); pit != parameters_.end(); ++pit)
	{
		ArgParameter* param = *pit;
		if ((!shortName.empty() && param->shortName() == shortName) ||
			(!longName.empty() && param->longName() == longName))
		{
			LASS_THROW("Subscribing parameter failed because there is already an parameter with "
				"the same short or long name.");
		}
	}

	// all is well, subscribe!
	parameters_.push_back(&iParameter);
}



bool ArgParser::parseShort(const TArguments& iArguments, TSize& ioIndex)
{
	const std::string& arg = iArguments[ioIndex];
	LASS_ASSERT(arg.length() > 1); // arg is of form '-abcdef'

	// first one can have a value.  scan all parameters to see if you can find it
	//
	std::string shortName(arg, 1, 1); // in above example shortName is 'a'
	LASS_ASSERT(shortName.length() == 1);
	LASS_ASSERT(shortName[0] == arg[1]);
	if (writeVersionOrHelp(shortName))
	{
		return false;
	}
	for (TParameters::iterator pit = parameters_.begin(); pit != parameters_.end(); ++pit)
	{
		ArgParameter* param = *pit;
		if (param->shortName() == shortName)
		{
			if (!(param->mode() & amNoValue)) // param can have value?
			{
				std::string value = "";
				if (arg.length() > 2)
				{
					value = arg.substr(2); // the rest of arg is the value
				}
				else
				{
					// if more arguments follow, the following is the value (but it can't start
					// with a '-' !
					//
					if (ioIndex + 1 < iArguments.size())
					{
						std::string candidate = iArguments[ioIndex + 1];
						if (candidate.length() > 0 && candidate[0] != '-')
						{
							value = candidate;
							++ioIndex;
						}
					}
				}
				param->setValue(value);
				return true;
			}
			else
			{
				param->setValue("");
			}
		}
	}

	// if first one couldn't have a value (it was a flag), then other flags may follow
	//
	for (std::string::size_type i = 2; i < arg.length(); ++i)
	{
		shortName = arg.substr(i, 1);
		LASS_ASSERT(shortName.length() == 1);
		LASS_ASSERT(shortName[0] == arg[i]);
		if (writeVersionOrHelp(shortName))
		{
			return false;
		}
		for (TParameters::iterator pit = parameters_.begin(); pit != parameters_.end(); ++pit)
		{
			ArgParameter* param = *pit;
			if (param->shortName() == shortName)
			{
				if (param->mode() & amNoValue) // only params without values may follow
				{
					param->setValue("");
				}
				else
				{
					if (!isQuiet_)
					{
						LASS_COUT << "Bad program arguments: the argument '" << shortName
							<< "' can take a value.  You cannot group it with other arguments like "
							<< "in '" << arg << "'.\n" << usage() << "\n";
					}
					return false;
				}
			}
		}
	}
	return false;
}



bool ArgParser::parseLong(const TArguments& iArguments, TSize iIndex)
{
	bool result = true;

	const std::string& arg = iArguments[iIndex];
	LASS_ASSERT(arg.length() > 2); // arg is of form '--longName=value'

	// split name and value
	//
	std::string longName;
	std::string value;
	std::string::size_type equalPosition = arg.find('=');
	if (equalPosition == std::string::npos)
	{
		longName = arg.substr(2);
		value = "";
	}
	else
	{
		longName = arg.substr(2, equalPosition - 2);
		value = arg.substr(equalPosition + 1);
	}

	// check syntax of name.
	//
	if (!isValidLongName(longName))
	{
		if (!isQuiet_)
		{
			LASS_COUT << "Bad program arguments: the argument '--" << longName << "' is not a "
				<< "valid long parameter name.\n" << usage() << "\n";
		}
		return false;
	}

	if (writeVersionOrHelp(longName))
	{
		return false;
	}

	TParameters::iterator match = parameters_.end(); // no match yet.

	// try to find an exact match
	//
	for (TParameters::iterator pit = parameters_.begin(); pit != parameters_.end(); ++pit)
	{
		ArgParameter* param = *pit;
		if (param->longName() == longName)
		{
			match = pit; // huray, we have one!
			break;
		}
	}

	// otherwise, try to find a unique best match
	//
	if (match == parameters_.end())
	{
		for (std::string::size_type length = longName.length(); length > 0; --length)
		{
			TParameters::iterator candidate = parameters_.end();
			bool isUnique = true;

			for (TParameters::iterator pit = parameters_.begin(); pit != parameters_.end(); ++pit)
			{
				ArgParameter* param = *pit;
				if (param->longName().length() >= length)
				{
					if (param->longName().substr(1, length) == longName.substr(1, length))
					{
						isUnique = candidate == parameters_.end();
						candidate = pit;
					}
				}
			}

			if (candidate != parameters_.end() && isUnique == true)
			{
				match = candidate; // huray too!
				break;
			}
		}
	}

	// by now, we have to have a match
	//
	if (match == parameters_.end())
	{
		if (!isQuiet_)
		{
			LASS_COUT << "Bad program arguments: the argument '--" << longName << "' is not as a "
				<< "long parameter name, nor is it a unique abbrevation of one.\n" << usage()
				<< "\n";
		}
		return false;
	}

	// we have one, let's deal with it.
	//
	(*match)->setValue(value); // even works if no value is expected
	return result;
}



/** long names can only exists of alphanumerics and some extra characters '-'.
 */
bool ArgParser::isValidLongName(const std::string& iLongName) const
{
	std::string extra = "-";

	for (std::string::size_type i = 0; i < iLongName.length(); ++i)
	{
		const char ch = iLongName[i];
		if (!std::isalnum(ch, std::locale()) && extra.find(ch) == std::string::npos)
		{
			return false;
		}
	}
	return true;
}



bool ArgParser::writeVersionOrHelp(const std::string& iArgument) const
{
	if (iArgument == "v" || iArgument == "version")
	{
		writeVersion();
		return true;
	}
	if (iArgument == "h" || iArgument == "help")
	{
		writeHelp();
		return true;
	}
	return false;
}



void ArgParser::writeVersion() const
{
	LASS_COUT << programName_ << " version " << programVersion_ << "\n";
}



void ArgParser::writeHelp() const
{
	writeVersion();
	LASS_COUT << usage() << "\n";
}



// --- ArgParameter -------------------------------------------------------------------------------------

ArgParameter::~ArgParameter()
{
}



const std::string& ArgParameter::shortName() const
{
	return shortName_;
}



const std::string& ArgParameter::longName() const
{
	return longName_;
}



const int ArgParameter::mode() const
{
	return mode_;
}



bool ArgParameter::operator!() const
{
	return !isSet_;
}



ArgParameter::operator num::SafeBool() const
{
	return isSet_ ? num::safeTrue : num::safeFalse;
}



ArgParameter::ArgParameter(ArgParser& iParser,
						   const std::string& iShortName,
						   const std::string& iLongName,
						   int iArgMode):
	parser_(iParser),
	shortName_(iShortName),
	longName_(iLongName),
	mode_(iArgMode),
	isSet_(false)
{
	parser_.subscribe(*this);
}



const std::string ArgParameter::names() const
{
	std::ostringstream result;

	if (!shortName_.empty())
	{
		result << "-" << shortName_;
	}
	if (!longName_.empty())
	{
		if (!shortName_.empty())
		{
			result << "|";
		}
		result << "--" << longName_;
	}

	return result.str();
}



const bool ArgParameter::parserIsQuiet() const
{
	return parser_.isQuiet_;
}



void ArgParameter::set()
{
	isSet_ = true;
}



const std::string ArgParameter::format() const
{
	return doFormat();
}



const bool ArgParameter::setValue(const std::string& iValue)
{
	return doSetValue(iValue);
}



const std::string ArgParameter::doFormat() const
{
	std::ostringstream result;
	result << "[" << names() << "]";
	return result.str();
}



const bool ArgParameter::doSetValue(const std::string& /* iValue */)
{
	//LASS_LOG("parameter '" << names() << "' is set.");
	set();
	return true;
}



// --- ArgFlag -------------------------------------------------------------------------------------

ArgFlag::ArgFlag(ArgParser& iParser,
				 const std::string& iShortName,
				 const std::string& iLongName):
	ArgParameter(iParser, iShortName, iLongName, amNoValue)
{
}



ArgFlag::ArgFlag(ArgParser& iParser,
				 const ArgFormat& iFormat):
	ArgParameter(iParser, iFormat.shortName, iFormat.longName, amNoValue)
{
}



}

}

// EOF
