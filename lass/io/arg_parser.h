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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

	amDefault = amRequired	/**< default mode for arguments. */
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
class LASS_DLL ArgParser
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
class LASS_DLL ArgParameter
{
public:

	virtual ~ArgParameter();

	const std::string& shortName() const;
	const std::string& longName() const;
	int mode() const;
	const std::string format() const;

	bool operator!() const;
	operator num::SafeBool() const;

protected:

	ArgParameter(ArgParser& iParser,
				 const std::string& iShortName,
				 const std::string& iLongName,
				 int iArgMode = amDefault);

	const std::string names() const;
	bool parserIsQuiet() const;
	bool setValue(const std::string& iValue);
	void set();

private:

	friend class ArgParser;

	virtual const std::string doFormat() const;
	virtual bool doSetValue(const std::string& iValue);

	ArgParser& parser_;
	std::string shortName_;
	std::string longName_;
	int mode_;
	bool isSet_;
};



/** @ingroup ArgParser
 */
class LASS_DLL ArgFlag: public ArgParameter
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
class ArgValue: public ArgParameter
{
public:

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef std::vector<TValue> TValues;
	typedef typename TValues::const_iterator TValueIterator;

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

	const TValues& all() const;
	size_t size() const;
	typename ArgValue::TConstReference operator[](size_t iIndex) const;
	typename ArgValue::TConstReference at(size_t iIndex) const;
	TValueIterator begin() const;
	TValueIterator end() const;

private:


	virtual const std::string doFormat() const;
	virtual bool doSetValue(const std::string& iValue);

	void checkMode(int iArgMode) const;

	TValues values_;
	TValues default_;
	std::string description_;
};




/** @ingroup ArgParser
 *  Exception thrown in case of bad arguments.
 */
class ArgBadArgument: public util::ExceptionMixin<ArgBadArgument>
{
public:
	ArgBadArgument(const std::string& msg, const std::string& loc): util::ExceptionMixin<ArgBadArgument>(msg, loc) {}
};




}

}

#include "arg_parser.inl"

#endif

// EOF
