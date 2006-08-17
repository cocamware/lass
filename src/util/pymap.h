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

/** @defgroup Python
 *  @brief interface library to Python
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYMAP_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYMAP_H

#include "util_common.h"
#include "pyobject_plus.h"
#include "string_cast.h"
#include "pyiteratorrange.h"

namespace lass
{
namespace python
{
namespace impl
{
	class PyMapImplBase
	{
	public:
		PyMapImplBase() {};
		virtual ~PyMapImplBase() {};
		virtual int PyMap_Length() = 0;
		virtual PyObject* PyMap_Subscript( PyObject* iKey) = 0;
		virtual int PyMap_AssSubscript( PyObject* iKey, PyObject* iValue) = 0;
		virtual std::string pyStr(void) = 0;
		virtual std::string pyRepr(void) = 0;
		virtual PyObjectPtr<PyObject>::Type keys() const = 0;
		virtual PyObjectPtr<PyObject>::Type values() const = 0;
		virtual PyObject* PyMap_Iter() = 0;

	};

	template<typename M> 
	class PyMapImpl : public PyMapImplBase
	{
	public:
		enum Ownership
		{
			oOwner,
			oBorrowed
		};
		PyMapImpl(M* iMap, Ownership iOwnership = oBorrowed) : map_(iMap), ownership_(iOwnership) {}
		virtual ~PyMapImpl();
		virtual int PyMap_Length();
		virtual PyObject* PyMap_Subscript( PyObject* iKey);
		virtual int PyMap_AssSubscript( PyObject* iKey, PyObject* iValue);
		virtual std::string pyStr(void);
		virtual std::string pyRepr(void);
		virtual PyObjectPtr<PyObject>::Type keys() const;
		virtual PyObjectPtr<PyObject>::Type values() const;
		virtual PyObject* PyMap_Iter();
	private:

		int doPyMap_AssSubscript( PyObject* iKey, PyObject* iValue, meta::True);
		int doPyMap_AssSubscript( PyObject* iKey, PyObject* iValue, meta::False);

		M* map_;
		Ownership ownership_;
		enum { readOnly_ = meta::TypeTraits<M>::isConst };
	};

	/** PyMap.  Object for interfacing maps with Python 
	*/
	class LASS_DLL PyMap : public PyObjectPlus
	{
		PY_HEADER(PyObjectPlus);
		static PyMappingMethods pyMappingMethods;

	public:
		template<typename M> PyMap( M* iStdMap ) 
		{
			initialize();
			this->ob_type = &Type;
			LASS_ASSERT(iStdMap);
			pimpl_ = new PyMapImpl<M>(iStdMap); // also const M*
		}
		template<typename M> PyMap( M& iStdMap ) 
		{
			initialize();
			this->ob_type = &Type;
			pimpl_ = new PyMapImpl<M>(&iStdMap);
		}
		template<typename M> PyMap( const M& iStdMap ) 
		{
			initialize();
			this->ob_type = &Type;
			std::auto_ptr<M> copy(new M(iStdMap));
			pimpl_ = new PyMapImpl<const M>(copy.get(), PyMapImpl<const M>::oOwner);
			copy.release();
		}
		virtual ~PyMap();
		virtual std::string pyStr(void) { return pimpl_->pyStr(); }
		virtual std::string pyRepr(void) { return pimpl_->pyRepr(); }
		virtual PyObjectPtr<PyObject>::Type keys() const { return pimpl_->keys(); }
		virtual PyObjectPtr<PyObject>::Type values() const { return pimpl_->values(); }

		static int PyMap_Length( PyObject* iPO);
		static PyObject* PyMap_Subscript( PyObject* iPO, PyObject* iKey);
		static int PyMap_AssSubscript( PyObject* iPO, PyObject* iKey, PyObject* iValue);

		static PyObject* PyMap_Iter( PyObject* iPO);
		
	private:

		PyMap();
		PyMapImplBase*	pimpl_;
		static void initialize();
		static bool isInitialized;
	};


	template<typename M>
	PyMapImpl<M>::~PyMapImpl()
	{
		LASS_ASSERT(map_);
		if (ownership_ == oOwner)
		{
			delete map_;
		}
	}

	template<typename M>
	int PyMapImpl<M>::PyMap_Length()
	{
		LASS_ASSERT(map_);
		const int size = static_cast<int>(map_->size());
		LASS_ASSERT(size >= 0);
		return size;
	}

	template<typename M>
	PyObject* PyMapImpl<M>::PyMap_Iter()
	{
		PyIteratorRange*	itRange=new PyIteratorRange(map_->begin(), map_->end());
		return itRange;
	}


	template<typename M>
	PyObjectPtr<PyObject>::Type PyMapImpl<M>::keys() const
	{
		LASS_ASSERT(map_);
		std::vector<typename M::key_type> temp;
		for (typename M::const_iterator it=map_->begin();it!=map_->end();++it)
			temp.push_back(it->first);
		return pyBuildList(temp.begin(),temp.end());
	}

	template<typename M>
	PyObjectPtr<PyObject>::Type PyMapImpl<M>::values() const
	{
		LASS_ASSERT(map_);
		std::vector<typename M::mapped_type> temp;
		for (typename M::const_iterator it=map_->begin();it!=map_->end();++it)
			temp.push_back(it->second);
		return pyBuildList(temp.begin(),temp.end());
	}


	template<typename M>
	PyObject* PyMapImpl<M>::PyMap_Subscript( PyObject* iKey)
	{
		LASS_ASSERT(map_);
		typename M::key_type cppKey;
		int r = pyGetSimpleObject( iKey, cppKey );
		if (r)
		{
			PyErr_SetString(PyExc_TypeError, "Cannot convert key to appropriate type");
			return NULL;
		}
		typename M::const_iterator it = map_->find(cppKey);
		if (it==map_->end())
		{
			PyErr_SetObject(PyExc_KeyError, iKey);
			return NULL;
		}
		PyObject* rv = pyBuildSimpleObject( it->second );
		Py_INCREF(rv);
		return rv;
	}
	
	template<typename M>
	int PyMapImpl<M>::PyMap_AssSubscript( PyObject* iKey, PyObject* iData)
	{
		return PyMapImpl<M>::doPyMap_AssSubscript( iKey, iData, meta::Bool<readOnly_>());
	}

	template<typename M>
	int PyMapImpl<M>::doPyMap_AssSubscript( PyObject* iKey, PyObject* iData, meta::True)
	{
		PyErr_SetString(PyExc_TypeError, "Map is not writeable");
		return 1;
	}

	template<typename M>
	int PyMapImpl<M>::doPyMap_AssSubscript( PyObject* iKey, PyObject* iData, meta::False)
	{
		LASS_ASSERT(map_);
		if (iData == NULL)		
		{
			typename M::key_type cppKey;
			int r = pyGetSimpleObject( iKey, cppKey );
			if (!r)
				map_->erase(cppKey);
			else
			{
				PyErr_SetString(PyExc_TypeError, "Cannot convert key to appropriate type");
				return 1;
			}
			return 0;
		}
		else
		{
			typename M::key_type cppKey;
			typename M::mapped_type cppData;
			int rk = pyGetSimpleObject( iKey, cppKey );
			if (rk)
			{
				PyErr_SetString(PyExc_TypeError, "Cannot convert key to appropriate type");
				return 1;
			}
			int rv = pyGetSimpleObject( iData, cppData );
			if (rv)
			{
				PyErr_SetString(PyExc_TypeError, "Cannot convert data to appropriate type");
				return 1;
			}
			map_->insert(typename M::value_type(cppKey,cppData));
			return 0;
		}
	}

	template<typename M>
	std::string PyMapImpl<M>::pyStr( void)
	{
		LASS_ASSERT(map_);
		return  pyRepr();
	}

	template<typename M>
	std::string PyMapImpl<M>::pyRepr( void)
	{
		LASS_ASSERT(map_);
		return util::stringCast<std::string>(*map_);
	}

}



/** @ingroup Python
	*  wrap a "borrowed" const std::map as Python dictionary
	*  @note you build a reference to the std::map, but the map is read-only
	*  @warning holding a reference to the dictionary, while the original map is destroyed is a 
	*	not-so-good-idea.
	*/
template<class K, class V, typename P, typename A>
PyObject* pyBuildSimpleObject( const std::map<K, V, P, A>* iV )
{
	return new impl::PyMap( iV );
}

/** @ingroup Python
	*  wrap a "borrowed" std::map as Python dictionary
	*  @note you build a reference to the std::map, any changes done in Python
	*  will be reflected in the original object, as far as the typesystem allows it of course
	*  @warning holding a reference to the dictionary, while the original map is destroyed is a 
	*	not-so-good-idea.
	*/
template<class K, class V, typename P, typename A>
PyObject* pyBuildSimpleObject( std::map<K, V, P, A>* iV )
{
	return new impl::PyMap( iV );
}



/** @ingroup Python
	*  build a copy of a std::map as a Python dictionary
	*  @note the constructed dictionary is made read only.
	*  @note a completely fresh copy of the std::map is made, so it's perfectly safe to use this
	*	to cast temporary function return values to Python.
	*/
template<class K, class V, typename P, typename A>
PyObject* pyBuildSimpleObject( const std::map<K, V, P, A>& iV )
{
	return new impl::PyMap( iV );
}

/** @ingroup Python
	*  wrap a "borrowed" std::map as Python dictionary
	*  @note you build a reference to the std::map, any changes done in Python
	*  will be reflected in the original object, as far as the typesystem allows it of course
	*  @warning holding a reference to the dictionary, while the original map is destroyed is a 
	*	not-so-good-idea.
	*/
template<class K, class V, typename P, typename A>
PyObject* pyBuildSimpleObject( std::map<K, V, P, A>& iV )
{
	return new impl::PyMap( iV );
}

}

}

#endif
