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



/** @class lass::util::Dictionary
 *  @brief a wrapper around a std::map to translate keys in values and back.
 *  @author [BdG]
 *
 *  This utility class has been written for the purpose of translating enumeration values to 
 *  strings.  Enumerations are a handy concept for the programmer to enumerate different options, 
 *  cases.  However, they only have names in code.  For the compiler these are just numbers, and if 
 *  you want to use these enumerations in your user interface you again stumble on the fact that 
 *  these are nothing but fancy numbers.
 *
 *  This dictionary has been made to make it possible to translate these numbers to text.  Typically 
 *  you'd set up a dictionary that translates each enumeration to a string, and then you use this
 *  dictionary as an interface between your code and your user interface.  It is also possible to set
 *  a default key and value that must be returned in no match can be found during the lookup.  This 
 *  default could indicate for an invalid state.
 *
 *  @code
 *  #include <lass/util/dictionary.h>
 *
 *  enum FooBar 
 *  { 
 *		fbInvalid, 
 *		fbEggs, 
 *		fbSpam, 
 *		fbHam 
 *  };
 *  typedef lass::util::Dictionary<std::string, FooBar> TFooBarDictionary;
 *
 *  TFooBarDictionary fooBarDictionary;
 *  fooBarDictionary.add("eggs", fbEggs);
 *  fooBarDictionary.add("spam", fbSpam);
 *  fooBarDictionary.add("ham", fbHam);
 *  fooBarDictionary.setDefault("invalid", fbInvalid);
 *
 *  // to user interface
 *  //
 *  FooBar fooBar = fbEggs;
 *  LASS_COUT << fooBarDictionary.key(fooBar);
 *
 *  // from user interface.
 *  //
 *  std::string input;
 *  LASS_CIN >> input;
 *  fooBar = fooBarDictionary[input];
 *  if (fooBar == fbInvalid)  
 *  {
 *		doSomethingSpecial();
 *  }
 *  @endcode
 *
 *  To end this discussion, we have one more suggestion to make of this dictionary a singleton.
 *  You create a singleton accessor function and you init the dictionary by using 
 *  LASS_EXECUTE_BEFORE_MAIN.
 *
 *  @code
 *  #include <lass/util/dictionary.h>
 *  #include <lass/util/singleton.h>
 *
 *  enum FooBar 
 *  { 
 *		fbInvalid, 
 *		fbEggs, 
 *		fbSpam, 
 *		fbHam 
 *  };
 *  typedef lass::util::Dictionary<std::string, FooBar> TFooBarDictionary;
 *
 *  TFooBarDictionary& fooBarDictionary()
 *  {
 *		return *lass::util::Singleton<TFooBarDictionary, 1>::instance();
 *  }
 *
 *  LASS_EXECUTE_BEFORE_MAIN
 *  (
 *		fooBarDictionary().add("eggs", fbEggs);
 *		fooBarDictionary().add("spam", fbSpam);
 *		fooBarDictionary().add("ham", fbHam);
 *		fooBarDictionary().setDefault("invalid", fbInvalid);
 *  )
 *
 *  // ...
 *
 *  LASS_COUT << fooBarDictionary().key(fbEggs);
 *  @endcode
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_DICTIONARY_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_DICTIONARY_H

#include "util_common.h"

namespace lass
{
namespace util
{

template
<
	typename KeyType,
	typename ValueType
>
class Dictionary
{
public:

	typedef KeyType TKey;
	typedef ValueType TValue;
	typedef std::set<KeyType> TKeys;
	typedef std::set<ValueType> TValues;
	typedef typename CallTraits<KeyType>::TParam TKeyParam;
	typedef typename CallTraits<ValueType>::TParam TValueParam;

	Dictionary();

	void add(const TKey& iKey, const TValue& iValue);
	void remove(const TKey& iKey, const TValue& iValue);
	void setDefault(const TKey& iKey, const TValue& iValue);
	void clearDefault();

	const TValue& operator[](TKeyParam iKey) const;
	const TKey& key(TValueParam iValue) const;

	TKeys keys() const;
	TValues values() const;

	TKeys keys(TValueParam iValue) const;
	TValues values(TKeyParam iKey) const;

private:

	typedef std::multimap<TKey, TValue> TMap;

	TMap map_;
	TKey defaultKey_;
	TValue defaultValue_;
	bool hasDefault_;
};



}

}

#include "dictionary.inl"

#endif

// EOF
