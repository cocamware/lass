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

/** @defgroup Python
 *  @brief interface library to Python
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYMAP_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYMAP_H

#include "python_common.h"
#include "pyshadow_object.h"
#include "pyiteratorrange.h"
#include "container.h"
#include "pyobject_util.h"
#include <map>
#include "../stde/access_iterator.h"
#include "../meta/type_traits.h"

namespace lass
{
namespace python
{
namespace impl
{
	class LASS_DLL PyMapImplBase: public ContainerImplBase
	{
	public:
		virtual std::auto_ptr<PyMapImplBase> copy() const = 0; 
		virtual PyObject* subscript(PyObject* key) const = 0;
		virtual int assSubscript(PyObject* key, PyObject* value) = 0;
		virtual PyObject* keys() const = 0;
		virtual PyObject* values() const = 0;
	};

	template<typename Container> 
	class PyMapImpl: public ContainerImpl<Container, PyMapImplBase>
	{
	public:
		typedef ContainerImpl<Container, PyMapImplBase> TBase;
		typedef typename TBase::TContainerPtr TContainerPtr;
		typedef typename TBase::TConstContainerPtr TConstContainerPtr;
		typedef typename TBase::TContainerTraits TContainerTraits;

		PyMapImpl(const TContainerPtr& container, bool readOnly = false): 
			TBase(container, readOnly)
		{
		}
		std::auto_ptr<PyMapImplBase> copy() const
		{
			TContainerPtr copy = TContainerTraits::copy(this->container());
			return std::auto_ptr<PyMapImplBase>(new PyMapImpl(copy));
		}
		PyObject* subscript(PyObject* key) const
		{
			LockGIL LASS_UNUSED(lock);
			typename Container::key_type k;
			if (pyGetSimpleObject(key, k) != 0)
			{
				PyErr_SetObject(PyExc_KeyError, key);
				return 0;
			}
			typename Container::const_iterator it = this->container().find(k);
			if (it == this->container().end())
			{
				PyErr_SetObject(PyExc_KeyError, key);
				return 0;
			}
			return pyBuildSimpleObject(it->second);
		}
		int assSubscript(PyObject* key, PyObject* value)
		{
			LockGIL LASS_UNUSED(lock);
			if (!this->checkWritable())
			{
				return -1;
			}
			typename Container::key_type k;
			if (pyGetSimpleObject(key, k) != 0)
			{
				impl::addMessageHeader("key");
				return -1;
			}
			if (value)
			{
				typename Container::mapped_type v;
				if (pyGetSimpleObject(value, v) != 0)
				{
					impl::addMessageHeader("value");
					return -1;
				}
				this->container()[k] = v;
			}
			else
			{
				typename Container::iterator it = this->container().find(k);
				if (it == this->container().end())
				{
					PyErr_SetObject(PyExc_KeyError, key);
					return -1;
				}
				this->container().erase(it);
			}
			return 0;
		}
		PyObject* keys() const
		{
			return new PyIteratorRange(
				stde::first_iterator(this->container().begin()), stde::first_iterator(this->container().end()));
		}
		PyObject* values() const
		{
			return new PyIteratorRange(
				stde::second_iterator(this->container().begin()), stde::second_iterator(this->container().end()));
		}
		const TPyObjPtr asNative() const
		{
			return pyBuildMap(this->container().begin(), this->container().end());
		}
	};

	class Map;
	typedef PyObjectPtr<Map>::Type TMapPtr;

	/** Map.  Object for interfacing maps with Python 
	*/
	class LASS_DLL Map : public PyObjectPlus, util::NonCopyable
	{
		PY_HEADER(PyObjectPlus);
		static PyMappingMethods pyMappingMethods;
	public:
		template <typename Container> Map( const util::SharedPtr<Container>& container )
		{
			std::auto_ptr<PyMapImplBase> pimpl(
				new PyMapImpl<Container>(LASS_ENFORCE_POINTER(container)));
			init(pimpl);
		}
		template<typename Container> Map( const util::SharedPtr<const Container>& container ) 
		{
			std::auto_ptr<PyMapImplBase> pimpl(
				new PyMapImpl<Container>(LASS_ENFORCE_POINTER(container).template constCast<Container>(), true));
			init(pimpl);
		}
		template<typename Container> Map( const Container& container )
		{
			util::SharedPtr<Container> p(new Container(container));
			std::auto_ptr<PyMapImplBase> pimpl(new PyMapImpl<Container>(p, true));
			init(pimpl);
		}
		~Map();

		std::string doPyStr(void);
		std::string doPyRepr(void);
		const TPyObjPtr keys() const;
		const TPyObjPtr values() const;
		const TPyObjPtr items() const;
		const TPyObjPtr iter() const;
		const TPyObjPtr get(const TPyObjPtr& key, const TPyObjPtr& defaultValue) const;
		const TPyObjPtr getOrNone(const TPyObjPtr& key) const;
		void clear();
		const TMapPtr copy() const;
		const TPyObjPtr asDict() const;

		const std::type_info& type() const;
		void* raw(bool writable) const;

	private:
		Map(std::auto_ptr<PyMapImplBase> pimpl);
		void init(std::auto_ptr<PyMapImplBase> pimpl);
		static void initializeType();

		static Py_ssize_t length(PyObject* self);
		static PyObject* subscript(PyObject* self, PyObject* key);
		static int assSubscript(PyObject* self, PyObject* key, PyObject* value);

		util::ScopedPtr<PyMapImplBase> pimpl_;
		static bool isInitialized;
	};

	template <>
	struct ShadowTraits<Map>: public ShadowTraitsContainer< Map, ShadowTraits<Map> >
	{
		template <typename Container> static int getObjectImpl(PyObject* obj, util::SharedPtr<Container>& value, bool writable)
		{
			if (!PyMapping_Check(obj))
			{
				PyErr_SetString(PyExc_TypeError, "not a map");
				return 1;
			}

			// check if we have our own Sequence object, then take a shortcut
			if (obj->ob_type == Map::_lassPyClassDef.type())
			{
				const Map* const map = static_cast<Map*>(obj);
				void* const raw = map->raw(writable);
				if (raw && typeid(value) == map->type())
				{
					value = *static_cast< util::SharedPtr<Container>* >(raw);
					return 0;
				}
			}

			const util::SharedPtr<Container> result(new Container);
			const Py_ssize_t size = PyMapping_Length(obj);
			//TPyObjPtr items(PyMapping_Items(obj));
			TPyObjPtr items(PyObject_CallMethod(obj, (char*)"items", 0)); // own "implementation" of PyMapping_Items to avoid cast warning
			if (!items)
			{
				return 1;
			}
			LASS_ASSERT(PySequence_Size(items.get()) == size);
			PyObject** pairs = PySequence_Fast_ITEMS(items.get());
			for (Py_ssize_t i = 0; i < size; ++i)
			{
				typename Container::key_type key;
				typename Container::mapped_type mapped;
				if (decodeTuple(pairs[i], key, mapped) != 0)
				{
					impl::addMessageHeader("map");
					return 1;
				}
				(*result)[key] = mapped;
			}
			value = result;
			return 0;
		}
	};
}

template <typename ContainerType>
struct ShadoweeTraitsMap: meta::True
{
	typedef impl::Map TShadow;
	typedef impl::ShadowTraits<impl::Map> TShadowTraits;
	typedef SharedPointerTraits<ContainerType> TPointerTraits;
};

/**	@ingroup Python
 *	@internal
 */
template< typename K, typename T, typename C, typename A>
struct ShadoweeTraits< std::map<K, T, C, A> >:
	public ShadoweeTraitsMap< std::map<K, T, C, A> >
{
};

}

}

#endif

#ifdef LASS_GUARDIAN_OF_INCLUSION_STDE_VECTOR_MAP_H

namespace lass
{
namespace python
{
/**	@ingroup Python
 *	@internal
 */
template< typename K, typename T, typename C, typename A>
struct ShadoweeTraits< stde::vector_map<K, T, C, A> >:
	public ShadoweeTraitsMap< stde::vector_map<K, T, C, A> >
{
};
}
}

#endif

// EOF
