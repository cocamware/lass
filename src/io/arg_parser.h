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


/** @defgroup ArgParser ArgParser
 *  @brief library to parse GNU style program arguments
 *  @author Bram de Greve [BdG]
 */



#ifndef LASS_GUARDIAN_OF_INCLUSION_IO_ARG_PARSER_H
#define LASS_GUARDIAN_OF_INCLUSION_IO_ARG_PARSER_H

#include "io_common.h"
#include "../util/call_traits.h"
#include "../num/safe_bool.h"

namespace lass
{
namespace io
{

class ArgParameter;



/** @ingroup ArgParser
 */
enum ArgMode
{
	amNoValue = 1,          /**< argument takes no value (argument is flag) */
	amRequired = 2,         /**< if argument is used, a value is required */
	amOptional = 4,         /**< if argument is used, it can take a value, but is not required */
	amMultiple = 8,         /**< argument can have multiple values. */

	amDefault = amRequired, /**< default mode for arguments. */
};



/** @ingroup ArgParser
 *  @brief structure to store ArgFlag and ArgValue formats
 */
struct ArgFormat
{
	std::string shortName;      /**< short name of ArgFlag or ArgValue */
	std::string longName;       /**< long name of ArgFlag or ArgValue */
	std::string description;    /**< set description of value, unused by ArgFlag */
	int argMode;                /**< set argument mode for value, unused by ArgFlag */
	bool hasDefault;            /**< set true if ArgValue has default value, unused by ArgFlag */
	std::string defaultValue;   /**< only valid if @e hasDefault is true, unused by ArgFlag */
};



/** @ingroup ArgParser
 *  @brief the parser itself
 */
class LASS_DLL_EXPORT ArgParser
{
public:

	typedef std::vector<std::string> TArguments;

	ArgParser();
	ArgParser(const std::string& iProgramName,
			  const std::string& iVersion = "",
			  const std::string& iPositionalArguments = "");

	bool parse(const std::string& iArguments, TArguments* oPositionals = 0);
	bool parse(const TArguments& iArguments, TArguments* oPositionals = 0);
	bool parse(int iArgc, char* iArgv[], TArguments* oPositionals = 0);

	std::string usage() const;

private:

	friend class ArgParameter;

	typedef std::vector<ArgParameter*> TParameters;
	typedef TParameters::size_type TSize;

	void subscribe(ArgParameter& iArg);
	bool parseShort(const TArguments& iArguments, TSize& ioIndex);
	bool parseLong(const TArguments& iArguments, TSize iIndex);
	bool isValidLongName(const std::string& iLongName) const;
	bool writeVersionOrHelp(const std::string& iArgument) const;
	void writeVersion() const;
	void writeHelp() const;

	std::string programName_;
	std::string programVersion_;
	std::string positionals_;
	TParameters parameters_;
	bool isQuiet_;
};



/** @ingroup ArgParser
 */
class LASS_DLL_EXPORT ArgParameter
{
public:

	virtual ~ArgParameter();

	const std::string& shortName() const;
	const std::string& longName() const;
	const int mode() const;

	bool operator!() const;
	operator num::SafeBool() const;

	virtual std::string format() const;

protected:

	ArgParameter(ArgParser& iParser,
				 const std::string& iShortName,
				 const std::string& iLongName,
				 int iArgMode = amDefault);

	std::string names() const;
	bool parserIsQuiet() const;
	virtual bool setValue(const std::string& iValue);

	bool isSet_;

private:

	friend class ArgParser;


	ArgParser& parser_;
	std::string shortName_;
	std::string longName_;
	int mode_;
};



/** @ingroup ArgParser
 */
class LASS_DLL_EXPORT ArgFlag: public ArgParameter
{
public:

	ArgFlag(ArgParser& iParser,
			const std::string& iShortName,
			const std::string& iLongName);
	ArgFlag(ArgParser& iParser,
			const ArgFormat& iFormat);
};



/** @ingroup ArgParser
 */
template <typename T>
class LASS_DLL_EXPORT ArgValue: public ArgParameter
{
public:

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef std::vector<TValue> TValues;

	ArgValue(ArgParser& iParser,
			 const std::string& iShortName,
			 const std::string& iLongName,
			 const std::string& iDescription = "",
			 int iArgMode = amDefault);
	ArgValue(ArgParser& iParser,
			 const std::string& iShortName,
			 const std::string& iLongName,
			 const std::string& iDescription,
			 int iArgMode,
			 TParam iDefault);
	ArgValue(ArgParser& iParser,
			 const ArgFormat& iFormat);

	size_t size() const;
	typename ArgValue::TConstReference operator[](size_t iIndex) const;
	typename ArgValue::TConstReference at(size_t iIndex) const;
	TValues all() const;

	virtual std::string format() const;

private:

	virtual bool setValue(const std::string& iValue);

	void checkMode(int iArgMode) const;

	TValues values_;
	TValue default_;
	std::string description_;
	bool hasDefault_;
};




/** @ingroup ArgParser
 *  Exception thrown in case of bad arguments.
 */
class ArgBadArgument: public std::runtime_error
{
public:
	ArgBadArgument(const std::string& iArgument):
		runtime_error("Bad program argument '" + iArgument +
			"': see standard output for more details.")
	{
	}
};




}

}

#include "arg_parser.inl"

#endif

// EOF
