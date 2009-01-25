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



/** @class lass::util::Dictionary
 *  @brief a wrapper around a std::map to translate keys in values and back.
 *  @author [Bramz]
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
 *      fbInvalid,
 *      fbEggs,
 *      fbSpam,
 *      fbHam
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
 *      doSomethingSpecial();
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
 *      fbInvalid,
 *      fbEggs,
 *      fbSpam,
 *      fbHam
 *  };
 *  typedef lass::util::Dictionary<std::string, FooBar> TFooBarDictionary;
 *
 *  TFooBarDictionary& fooBarDictionary()
 *  {
 *      return *lass::util::Singleton<TFooBarDictionary>::instance();
 *  }
 *
 *  LASS_EXECUTE_BEFORE_MAIN
 *  (
 *      fooBarDictionary().add("eggs", fbEggs);
 *      fooBarDictionary().add("spam", fbSpam);
 *      fooBarDictionary().add("ham", fbHam);
 *      fooBarDictionary().setDefault("invalid", fbInvalid);
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
#include "call_traits.h"

namespace lass
{
namespace util
{

template
<
	typename KeyType,
	typename ValueType,
	typename KeyLess = std::less<KeyType>,
	typename ValueLess = std::less<ValueType>
>
class Dictionary
{
public:

	typedef KeyType TKey;
	typedef ValueType TValue;
	typedef KeyLess TKeyLess;
	typedef ValueLess TValueLess;
	typedef std::set<KeyType, KeyLess> TKeys;
	typedef std::set<ValueType, ValueLess> TValues;
	typedef typename CallTraits<KeyType>::TParam TKeyParam;
	typedef typename CallTraits<ValueType>::TParam TValueParam;

	Dictionary();
	explicit Dictionary(TKeyLess iKeyLess);
	Dictionary(TKeyLess iKeyLess, TValueLess iValueLess);

	void add(const TKey& iKey, const TValue& iValue);
	void remove(const TKey& iKey, const TValue& iValue);
	void setDefault(const TKey& iKey, const TValue& iValue);
	void clearDefault();
	bool hasDefault() const;
	void enableSuggestions(bool enable = true);

	const TValue& operator[](TKeyParam iKey) const;
	const TKey& key(TValueParam iValue) const;

	TKeys keys() const;
	TValues values() const;

	TKeys keys(TValueParam iValue) const;
	TValues values(TKeyParam iKey) const;

	bool isKey(TKeyParam iKey) const;
	bool isValue(TValueParam iValue) const;

private:

	typedef std::multimap<TKey, TValue, KeyLess> TMap;

	bool equalValues(TValueParam iA, TValueParam iB) const;

	TMap map_;
	TKey defaultKey_;
	TValue defaultValue_;
	TKeyLess keyLess_;
	TValueLess valueLess_;
	bool hasDefault_;
	bool enableSuggestions_;
};



}

}

#include "dictionary.inl"

#endif

// EOF
