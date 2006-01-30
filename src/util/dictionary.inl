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



#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_DICTIONARY_INL
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_DICTIONARY_INL

#include "util_common.h"
#include "dictionary.h"

namespace lass
{
namespace util
{

// --- public --------------------------------------------------------------------------------------

template <typename K, typename V, typename KL, typename VL>
Dictionary<K, V, KL, VL>::Dictionary():
	map_(),
	hasDefault_(false)
{
}



template <typename K, typename V, typename KL, typename VL>
Dictionary<K, V, KL, VL>::Dictionary(TKeyLess iKeyLess):
	map_(),
	keyLess_(iKeyLess),
	hasDefault_(false)
{
}



template <typename K, typename V, typename KL, typename VL>
Dictionary<K, V, KL, VL>::Dictionary(TKeyLess iKeyLess, TValueLess iValueLess):
	map_(),
	keyLess_(iKeyLess),
	valueLess_(iValueLess),
	hasDefault_(false)
{
}



/** add a key and value to dictionary as new entry.
 */
template <typename K, typename V, typename KL, typename VL>
void Dictionary<K, V, KL, VL>::add(const TKey& iKey, const TValue& iValue)
{
	std::pair<typename TMap::iterator, typename TMap::iterator> range = map_.equal_range(iKey);
	for (typename TMap::iterator i = range.first; i != range.second; ++i)
	{
		if (equalValues(i->second, iValue))
		{
			return;
		}
	}
	map_.insert(typename TMap::value_type(iKey, iValue));
}



/** remove a key and value from the dictionary.
 */
template <typename K, typename V, typename KL, typename VL>
void Dictionary<K, V, KL, VL>::remove(const TKey& iKey, const TValue& iValue)
{
	std::pair<typename TMap::iterator, typename TMap::iterator> range = map_.equal_range(iKey);
	for (typename TMap::iterator i = range.first; i != range.second; ++i)
	{
		if (equalValues(i->second, iValue))
		{
			map_.erase(i);
			return;
		}
	}
}



/** set key and value to be used as default ones.
 *  Once @e default key and value are set, these will be returned on occasions you try to look up a
 *  key or value but it can't be found.
 */
template <typename K, typename V, typename KL, typename VL>
void Dictionary<K, V, KL, VL>::setDefault(const TKey& iKey, const TValue& iValue)
{
	defaultKey_ = iKey;
	defaultValue_ = iValue;
	hasDefault_ = true;
}



/** clear default key and value.
 *  If no @e default key and value are set, and exception will be thrown on occasions you try to look
 *  up a key or value but it can't be found.
 */
template <typename K, typename V, typename KL, typename VL>
void Dictionary<K, V, KL, VL>::clearDefault()
{
	hasDefault_ = false;
}



/** return true if dictionary has value
 */
template <typename K, typename V, typename KL, typename VL>
bool Dictionary<K, V, KL, VL>::hasDefault() const
{
	return hasDefault_;
}



/** look up a value by key
 *  @return
 *      - the value that belongs to the key.
 *      - if such a value can not be found and an @e default value is set, then this @e default
 *        will be returned.
 *  @note
 *      if multiple entries with the same key are found, only one is returned.  It is not
 *      specified which one this will be.  If you want all entries with this key, use @c values(iKey).
 *  @throw
 *      if no value can be found that belongs to the key and no @e default value is set, then
 *      an exception will be thrown.
 */
template <typename K, typename V, typename KL, typename VL>
const typename Dictionary<K, V, KL, VL>::TValue&
Dictionary<K, V, KL, VL>::operator[](TKeyParam iKey) const
{
	typename TMap::const_iterator i = map_.find(iKey);
	if (i == map_.end())
	{
		if (!hasDefault_)
		{
			LASS_THROW("No key '" << iKey << "' is found in dictionary.");
		}
		return defaultValue_;
	}
	return i->second;
}



/** look up a key by value
 *  @return
 *      - the key to which the value belongs
 *      - if such a key can not be found and an @e default key is set, then this @e default will
 *        be returned.
 *  @note
 *      if multiple entries with the same value are found, only one is returned.  It is not
 *      specified which one this will be.  If you want all entries with this value, use @c keys(iValue).
 *  @throw if no key can be found to wich the value belongs and no @e default key is set, then an
 *         exception will be thrown.
 */
template <typename K, typename V, typename KL, typename VL>
const typename Dictionary<K, V, KL, VL>::TKey&
Dictionary<K, V, KL, VL>::key(TValueParam iValue) const
{
	for (typename TMap::const_iterator i = map_.begin(); i != map_.end(); ++i)
	{
		if (equalValues(i->second, iValue))
		{
			return i->first;
		}
	}
	if (!hasDefault_)
	{
		LASS_THROW("No value '" << iValue << "' is found in dictionary.");
	}
	return defaultKey_;
}



/** return all keys in the dictionary
 */
template <typename K, typename V, typename KL, typename VL>
typename Dictionary<K, V, KL, VL>::TKeys
Dictionary<K, V, KL, VL>::keys() const
{
	TKeys result;
	for (typename TMap::const_iterator i = map_.begin(); i != map_.end(); ++i)
	{
		result.insert(i->first);
	}
	return result;
}



/** return all values in the dictionary
 */
template <typename K, typename V, typename KL, typename VL>
typename Dictionary<K, V, KL, VL>::TValues
Dictionary<K, V, KL, VL>::values() const
{
	TValues result;
	for (typename TMap::const_iterator i = map_.begin(); i != map_.end(); ++i)
	{
		result.insert(i->second);
	}
	return result;
}



/** return all keys that have value @a iValue.
 */
template <typename K, typename V, typename KL, typename VL>
typename Dictionary<K, V, KL, VL>::TKeys
Dictionary<K, V, KL, VL>::keys(TValueParam iValue) const
{
	TKeys result;
	for (typename TMap::const_iterator i = map_.begin(); i != map_.end(); ++i)
	{
		if (equalValues(i->second, iValue))
		{
			result.insert(i->first);
		}
	}
	return result;
}



/** return all values in the dictionary
 */
template <typename K, typename V, typename KL, typename VL>
typename Dictionary<K, V, KL, VL>::TValues
Dictionary<K, V, KL, VL>::values(TKeyParam iKey) const
{
	TValues result;
	std::pair<typename TMap::const_iterator, typename TMap::const_iterator> range = map_.equal_range(iKey);
	for (typename TMap::const_iterator i = range.first; i != range.second; ++i)
	{
		result.insert(i->second);
	}
	return result;
}



/** return true if @a iKey is a key of dictionary
 */
template <typename K, typename V, typename KL, typename VL>
bool Dictionary<K, V, KL, VL>::isKey(TKeyParam iKey) const
{
	return map_.find(iKey) != map_.end();
}



/** return true if @a iValue is a value of dictionary
 */
template <typename K, typename V, typename KL, typename VL>
bool Dictionary<K, V, KL, VL>::isValue(TValueParam iValue) const
{
	for (typename TMap::const_iterator i = map_.begin(); i != map_.end(); ++i)
	{
		if (equalValues(i->second, iValue))
		{
			return true;
		}
	}
	return false;
}



// --- protected -----------------------------------------------------------------------------------



// --- private -------------------------------------------------------------------------------------

template <typename K, typename V, typename KL, typename VL>
bool Dictionary<K, V, KL, VL>::equalValues(TValueParam iA, TValueParam iB) const
{
	return !valueLess_(iA, iB) && !valueLess_(iB, iA);
}



// --- free ----------------------------------------------------------------------------------------



}

}

#endif

// EOF
