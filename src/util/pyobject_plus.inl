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



		inline PyMethodDef createPyMethodDef(  char	*ml_name, 
										PyCFunction  ml_meth,
                                        int		 ml_flags,
                                        char	*ml_doc )
		{
			PyMethodDef temp;
			temp.ml_name = ml_name;
			temp.ml_meth = ml_meth;
			temp.ml_flags = ml_flags;
			temp.ml_doc = ml_doc;
			return temp;
		}

		inline PyGetSetDef createPyGetSetDef ( char* name, 
											  getter get,
											  setter set,
											  char* doc,
											void* closure  )
		{
			PyGetSetDef temp;
			temp.name = name;
			temp.get = get;
			temp.set = set;
			temp.doc = doc;
			temp.closure = closure;
			return temp;
		}

		class PyMethodEqual
		{
		public:
			PyMethodEqual( const char* iName ): 
				name_(iName) 
			{
			}
			bool operator()(const PyMethodDef& iMethod) const 
			{
				return iMethod.ml_name && strcmp(iMethod.ml_name, name_) == 0;
			}
		private:
			const char* name_;
		};

		inline int pyGetSimpleObject( PyObject* iValue, PyObject*& oV )
		{
			if ( iValue == Py_None )
				oV = NULL;
			else
			{
				oV = iValue;
				Py_INCREF( oV );
			}
			return 0; 
		}
		template<class C> 
		inline int pyGetSimpleObject( PyObject* iValue, 
									  util::SharedPtr<C, PyObjectStorage, PyObjectCounter>& oV )
		{
			typedef util::SharedPtr<C, PyObjectStorage, PyObjectCounter> TPyPtr;
			bool isNone = (iValue == Py_None );
			if (isNone)
				oV = util::SharedPtr<C, PyObjectStorage, PyObjectCounter>();
			else
			oV = lass::python::fromPySharedPtrCast< typename TPyPtr::TPointee >(iValue);
			return 0;
		}
		inline int pyGetSimpleObject( PyObject* iValue, bool& oV )
		{
			if (!PyInt_Check(iValue))
			{
				PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_ARG_NOT_BOOL); 
				return 1;
			}
			long temp;
			temp = PyInt_AS_LONG( iValue );
			oV = (temp!=0);
			return 0; 
		}
		inline int pyGetSimpleObject( PyObject* iValue, int& oV )
		{
			if (!PyInt_Check(iValue))
			{
				PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_ARG_NOT_INT ); 
				return 1;
			}
			oV = PyInt_AS_LONG( iValue );
			return 0; // ok
		}
        inline int pyGetSimpleObject( PyObject* iValue, unsigned& oV )
        {
            int value;
            const int result = pyGetSimpleObject( iValue, value );
            if (!result)
            {
                oV = (unsigned) value;
            }
            return result;
        }
		inline int pyGetSimpleObject( PyObject* iValue, long& oV )
		{
			if (!PyInt_Check(iValue))
			{
				PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_ARG_NOT_LONG ); 
				return 1;
			}
			oV = PyInt_AS_LONG( iValue );
			return 0; // ok
		}
		inline int pyGetSimpleObject( PyObject* iValue, float& oV )
		{
			if (PyFloat_Check(iValue))
			{
				oV = static_cast<float>(PyFloat_AsDouble( iValue ));
				return 0;
			}
			if (PyInt_Check(iValue))
			{
				oV = static_cast<float>(PyInt_AsLong( iValue ));
				return 0;
			}
			if (PyLong_Check(iValue))
			{
				oV = static_cast<float>(PyLong_AsDouble( iValue ));
				return 0;
			}
			PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_ARG_NOT_FLOAT ); 
			return 1; // failure
		}
		inline int pyGetSimpleObject( PyObject* iValue, double& oV )
		{
			if (PyFloat_Check(iValue))
			{
				oV = static_cast<double>(PyFloat_AsDouble( iValue ));
				return 0;
			}
			if (PyInt_Check(iValue))
			{
				oV = static_cast<double>(PyInt_AsLong( iValue ));
				return 0;
			}
			if (PyLong_Check(iValue))
			{
				oV = static_cast<double>(PyLong_AsDouble( iValue ));
				return 0;
			}
			PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_ARG_NOT_DOUBLE ); 
			return 1; // failure
		}
		inline int pyGetSimpleObject( PyObject* iValue, std::string& oV )
		{
			if (!PyString_Check(iValue))
				return 1;
			oV = std::string( PyString_AS_STRING( iValue ) );
			return 0;
		}

		inline PyObject* pyBuildSimpleObject( PyObject& iV )
		{
			PyObject*	newOne = new PyObject( iV );
			return pyBuildSimpleObject( newOne );
		}

		inline PyObject* pyBuildSimpleObject( PyObject* iV )
		{
			if (iV==NULL)
			{
				Py_INCREF( Py_None );
				return Py_None;
			}
			Py_INCREF(iV);
			return iV;
		}

		template<class C>
		PyObject* pyBuildSimpleObject( const util::SharedPtr<C, PyObjectStorage, PyObjectCounter>& iV )
		{
			return lass::python::toPySharedPtrCast(iV);
		}

		inline PyObject* pyBuildSimpleObject( bool iV )
		{
			return Py_BuildValue("i",(int)(iV!=0));
		}
		inline PyObject* pyBuildSimpleObject( long iV )
		{
			return Py_BuildValue("i",iV);
		}
		inline PyObject* pyBuildSimpleObject( int iV )
		{
			return Py_BuildValue("i",iV);
		}
		inline PyObject* pyBuildSimpleObject( unsigned iV )
		{
			return pyBuildSimpleObject( (int) iV );
		}
		inline PyObject* pyBuildSimpleObject( float iV )
		{
			return Py_BuildValue("f",iV);
		}
		inline PyObject* pyBuildSimpleObject( double iV )
		{
			return Py_BuildValue("f",iV);
		}
		inline PyObject* pyBuildSimpleObject( const std::string& iV )
		{
			return Py_BuildValue("s",iV.c_str());
		}

		/*
		// using this gives trouble defining other templates when there is no partial template specialisation
		inline int pyParseObject( PyObject *args, PyObject *& oObj)
		{
			PyObject *pyObjPtr;
			int ok = PyArg_ParseTuple( args, "O", &pyObjPtr);
			if (!ok)
				PyErr_SetString( PyExc_AttributeError, "argument is not a PyObject" );
			else
				oObj = pyObjPtr;
			return ok;
		}
		

		// pyParseString.  This function parses the function arguments assuming there is
		//	only one and it is a string of length lower than 4096 characters.  This method
		//	is not thread safe!
		//
		inline int pyParseString( PyObject *args, std::string& oStr)
		{
			char *pyCharp;
			int ok = PyArg_ParseTuple( args, "s", &pyCharp);
			if (!ok)
				PyErr_SetString( PyExc_AttributeError, "argument is not a string" );
			else
				oStr = std::string(pyCharp);
			return ok;
		}

		inline int pyParseLong( PyObject *args, long& oObj)
		{
			long pyLong;
			int ok = PyArg_ParseTuple( args, "i", &pyLong);
			if (!ok)
				PyErr_SetString( PyExc_AttributeError, "argument is not a long" );
			else
				oObj = pyLong;
			return ok;
		}

		inline int pyParseFloat( PyObject *args, float& oObj)
		{
			long pyFloat;
			int ok = PyArg_ParseTuple( args, "f", &pyFloat);
			if (!ok)
				PyErr_SetString( PyExc_AttributeError, "argument is not a float" );
			else
				oObj = static_cast<float>(pyFloat);
			return ok;
		}

		inline int pyParseDouble( PyObject *args, double& oObj)
		{
			long pyDouble;
			int ok = PyArg_ParseTuple( args, "d", &pyDouble);
			if (!ok)
				PyErr_SetString( PyExc_AttributeError, "argument is not a double" );
			else
				oObj = pyDouble;
			return ok;
		}
		*/

		inline bool streq(const char *A, const char *B)// define "streq"
		{ return strcmp(A,B) == 0;};
