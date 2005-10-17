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

/** @defgroup Python
 *  @brief interface library to Python
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_UTIL_PYMAP_H
#define LASS_GUARDIAN_OF_INCLUSION_UTIL_PYMAP_H

#include "util_common.h"
#include "pyobject_plus.h"
#include "string_cast.h"

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
		virtual PyObject* keys() const = 0;
		virtual PyObject* values() const = 0;
	};

	template<typename M> 
	class PyMapImpl : public PyMapImplBase
	{
	public:
		PyMapImpl(M* iMap, bool iReadOnly = false) : map_(iMap), readOnly_(iReadOnly) {}
		virtual ~PyMapImpl() {}
		virtual int PyMap_Length();
		virtual PyObject* PyMap_Subscript( PyObject* iKey);
		virtual int PyMap_AssSubscript( PyObject* iKey, PyObject* iValue);
		virtual std::string pyStr(void);
		virtual std::string pyRepr(void);
		virtual PyObject* keys() const;
		virtual PyObject* values() const;
	private:
		M* map_;
		bool readOnly_;
	};

	/** PyMap.  Object for interfacing maps with Python 
	*/
	class LASS_DLL PyMap : public PyObjectPlus
	{
		PY_HEADER(PyObjectPlus);
		static PyMappingMethods pyMappingMethods;

	public:
		template<typename M> PyMap( M& iStdMap ) : PyObjectPlus(&Type)
		{
            pimpl_ = new PyMapImpl<M>(&iStdMap);
			initialize();
		}
		template<typename M> PyMap( const M& iStdMap ) : PyObjectPlus(&Type)
		{
            pimpl_ = new PyMapImpl<M>(const_cast<M*>(&iStdMap), true);
			initialize();
		}
		virtual ~PyMap();
		virtual std::string pyStr(void) { return pimpl_->pyStr(); }
		virtual std::string pyRepr(void) { return pimpl_->pyRepr(); }
		virtual PyObject* keys() const { return pimpl_->keys(); }
		virtual PyObject* values() const { return pimpl_->values(); }

		static int PyMap_Length( PyObject* iPO);
		static PyObject* PyMap_Subscript( PyObject* iPO, PyObject* iKey);
		static int PyMap_AssSubscript( PyObject* iPO, PyObject* iKey, PyObject* iValue);
		
	private:
		PyMap();
		PyMapImplBase*	pimpl_;
		static void initialize();
		static bool isInitialized;
	};


	template<typename M>
	int PyMapImpl<M>::PyMap_Length()
	{
		return map_->size();
	}

	template<typename M>
	PyObject* PyMapImpl<M>::keys() const
	{
		std::vector<M::key_type> temp;
		for (M::const_iterator it=map_->begin();it!=map_->end();++it)
			temp.push_back(it->first);
		return pyBuildList<std::vector<M::key_type> >(temp.begin(),temp.end());
	}

	template<typename M>
	PyObject* PyMapImpl<M>::values() const
	{
		std::vector<M::mapped_type> temp;
		for (M::const_iterator it=map_->begin();it!=map_->end();++it)
			temp.push_back(it->second);
		return pyBuildList<std::vector<M::key_type> >(temp.begin(),temp.end());
	}


	template<typename M>
	PyObject* PyMapImpl<M>::PyMap_Subscript( PyObject* iKey)
	{
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
		if (readOnly_)
		{
			PyErr_SetString(PyExc_TypeError, "Map is not writeable");
			return 1;
		}

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
			map_->insert(M::value_type(cppKey,cppData));
			return 0;
		}
	}

	template<typename M>
	std::string PyMapImpl<M>::pyStr( void)
	{
		return  pyRepr();
	}

	template<typename M>
	std::string PyMapImpl<M>::pyRepr( void)
	{
		return util::stringCast<std::string>(*map_);
	}

}

}

}

#endif