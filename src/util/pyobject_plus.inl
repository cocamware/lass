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



		inline void PyObjectPlus::__dealloc(PyObject *P)    
		{  
			delete ((PyObjectPlus *) P);  
		};

		inline PyObject*	PyObjectPlus::__repr(PyObject *PyObj) 
		{	
			return pyBuildSimpleObject(((TSelf*)PyObj)->pyRepr());
		}

		inline PyObject*	PyObjectPlus::__str(PyObject *PyObj) 
		{ 
			return pyBuildSimpleObject(((TSelf*)PyObj)->pyStr()); 
		}


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

		namespace impl
		{
            /** @internal
             *  predicate to check of a python method has the correct name.
             */
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

			/** @internal
             *  casts one numerical value to another with range checking.
			 *  implementation detail.
			 *  @note range of In should fully contain range of Out.
			 */
			template <typename In, typename Out>
			int pyNumericCast( In iIn, Out& oV )
			{
                LASS_ASSERT(num::NumTraits<In>::isSigned == num::NumTraits<Out>::isSigned);
                LASS_ASSERT(num::NumTraits<In>::min <= num::NumTraits<Out>::min);
				if (iIn < static_cast<In>(num::NumTraits<Out>::min))
				{
					std::ostringstream buffer;
					buffer << "not a " << num::NumTraits<Out>::name() << ": underflow: " 
						<< iIn << " < " << num::NumTraits<Out>::min;
					PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
					return 1;
				}
                LASS_ASSERT(num::NumTraits<In>::max >= num::NumTraits<Out>::max);
				if (iIn > static_cast<In>(num::NumTraits<Out>::max))
				{
					std::ostringstream buffer;
					buffer << "not a " << num::NumTraits<Out>::name() << ": overflow: " 
						<< iIn << " > " << num::NumTraits<Out>::max;
					PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
					return 1;
				}
				oV = static_cast<Out>(iIn);
                //LASS_ASSERT((oV < 0) == (iIn < 0));
				return 0;
			}

			/** @internal
             *  convert a PyObject to an signed integer with range checking.
			 *  implementation detail.
			 */
			template <typename Integer>
			int pyGetSignedObject( PyObject* iValue, Integer& oV )
			{
				if (PyInt_Check(iValue))
				{
					long temp = PyInt_AS_LONG(iValue);
					return pyNumericCast( temp, oV );
				}
				if (PyLong_Check(iValue))
				{
					long temp = PyLong_AsLong(iValue);
					if (PyErr_Occurred())
					{
						PyErr_Format(PyExc_TypeError, "not a %s: overflow", 
							num::NumTraits<Integer>::name().c_str());
						return 1;
					}
					return pyNumericCast( temp, oV );
				}
				PyErr_Format(PyExc_TypeError, "not a %s", num::NumTraits<Integer>::name().c_str());
				return 1;
			}

			/** @internal
             *  convert a PyObject to an unsigned integer with range checking.
			 *  implementation detail.
			 */
			template <typename Integer>
			int pyGetUnsignedObject( PyObject* iValue, Integer& oV )
			{
				if (PyInt_Check(iValue))
				{
					long temp = PyInt_AS_LONG(iValue);
					if (temp < 0)
					{
						std::ostringstream buffer;
						buffer << "not a " << num::NumTraits<Integer>::name() << ": negative: "
							<< temp << " < 0";
						PyErr_SetString(PyExc_TypeError, buffer.str().c_str());
						return 1;
					}
					return pyNumericCast( static_cast<unsigned long>(temp), oV );
				}
				if (PyLong_Check(iValue))
				{
					unsigned long temp = PyLong_AsUnsignedLong(iValue);
					if (PyErr_Occurred())
					{
						PyErr_Format(PyExc_TypeError, "not a %s: overflow", 
							num::NumTraits<Integer>::name().c_str());
						return 1;
					}
					return pyNumericCast( temp, oV );
				}
				PyErr_Format(PyExc_TypeError, "not a %s", num::NumTraits<Integer>::name().c_str());
				return 1;
			}

			/** @internal
             *  convert a PyObject to a floating point value with range checking.
			 *  implementation detail.
			 */
			template <typename Float>
			int pyGetFloatObject( PyObject* iValue, Float& oV )
			{
				if (PyFloat_Check(iValue))
				{
					double temp = PyFloat_AS_DOUBLE(iValue);
					return pyNumericCast( temp, oV );
				}
				if (PyInt_Check(iValue))
				{
					long temp = PyInt_AS_LONG(iValue);
					oV = static_cast<Float>( temp );
					return 0;
				}
				if (PyLong_Check(iValue))
				{
					double temp = PyLong_AsDouble(iValue);
					if (PyErr_Occurred())
					{
						PyErr_Format(PyExc_TypeError, "not a %s: overflow", 
							num::NumTraits<Float>::name().c_str());
						return 1;
					}
					return pyNumericCast( temp, oV );
				}
				PyErr_Format(PyExc_TypeError, "not a %s", num::NumTraits<Float>::name().c_str());
				return 1;
			}

		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, bool& oV )
		{
			int result = PyObject_IsTrue(iValue);
			if (result == -1)
			{
				PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_ARG_NOT_BOOL); 
				return 1;
			}
			oV = (result != 0);
			return 0;
		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, signed char& oV )
		{
			return impl::pyGetSignedObject( iValue, oV );
		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, unsigned char& oV )
		{
			return impl::pyGetUnsignedObject( iValue, oV );
		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, signed short& oV )
		{
			return impl::pyGetSignedObject( iValue, oV );
		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, unsigned short& oV )
		{
			return impl::pyGetUnsignedObject( iValue, oV );
		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, signed int& oV )
		{
			return impl::pyGetSignedObject( iValue, oV );
		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, unsigned int& oV )
		{
			return impl::pyGetUnsignedObject( iValue, oV );
		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, signed long& oV )
		{
			return impl::pyGetSignedObject( iValue, oV );
		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, unsigned long& oV )
		{
			return impl::pyGetUnsignedObject( iValue, oV );
		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, float& oV )
		{
			return impl::pyGetFloatObject( iValue, oV );
		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, double& oV )
		{
			return impl::pyGetFloatObject( iValue, oV );
		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, long double& oV )
		{
			return impl::pyGetFloatObject( iValue, oV );
		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, std::string& oV )
		{
			if (!PyString_Check(iValue))
			{
				PyErr_SetString(PyExc_TypeError, LASS_PYTHON_ERR_MSG_ARG_NOT_STRING);
				return 1;
			}
			oV = std::string( PyString_AS_STRING( iValue ) );
			return 0;
		}

		/** @ingroup Python
		 */
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

		/** @ingroup Python
		 */
		template<class C> 
		int pyGetSimpleObject( PyObject* iValue, 
                               util::SharedPtr<C, PyObjectStorage, PyObjectCounter>& oV )
		{
			const bool isNone = (iValue == Py_None );
			if (isNone)
				oV = util::SharedPtr<C, PyObjectStorage, PyObjectCounter>();
			else
			{                
				if (!PyType_IsSubtype(iValue->ob_type , &C::Type ))
				{
					PyErr_Format(PyExc_TypeError,"not castable to %s",C::Type.tp_name);
					return 1;
				}
			    oV = lass::python::fromPySharedPtrCast<C>(iValue);
			}
			return 0;
		}

		/** @ingroup Python
		 */
		inline int pyGetSimpleObject( PyObject* iValue, 
									  util::SharedPtr<PyObject, PyObjectStorage, PyObjectCounter>& oV )
		{
			const bool isNone = (iValue == Py_None );
            if (isNone)
            {
                oV = util::SharedPtr<PyObject, PyObjectStorage, PyObjectCounter>();
            }
            else
            {
                oV = lass::python::fromPySharedPtrCast<PyObject>(iValue);
            }
			return 0;
		}

		/** @ingroup Python
		 */
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

		/** @ingroup Python
		 */
		template<class C>
		PyObject* pyBuildSimpleObject( const util::SharedPtr<C, PyObjectStorage, PyObjectCounter>& iV )
		{
			return lass::python::toPySharedPtrCast(iV);
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( bool iV )
		{
			return PyInt_FromLong(static_cast<long>(iV));
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( signed char iV )
		{
			return PyInt_FromLong(static_cast<long>(iV));
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( unsigned char iV )
		{
			return PyInt_FromLong(static_cast<long>(iV));
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( signed short iV )
		{
			return PyInt_FromLong(static_cast<long>(iV));
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( unsigned short iV )
		{
			return PyInt_FromLong(static_cast<long>(iV));
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( signed int iV )
		{
			return PyInt_FromLong(static_cast<long>(iV));
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( unsigned int iV )
		{
			if (iV <= static_cast<unsigned int>(num::NumTraits<long>::max))
			{
				return PyInt_FromLong(static_cast<long>(iV));
			}
			else
			{
				return PyLong_FromUnsignedLong(static_cast<unsigned long>(iV));
			}
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( signed long iV )
		{
			return PyInt_FromLong(static_cast<long>(iV));
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( unsigned long iV )
		{
			if (iV <= static_cast<unsigned int>(num::NumTraits<long>::max))
			{
				return PyInt_FromLong(static_cast<long>(iV));
			}
			else
			{
				return PyLong_FromUnsignedLong(static_cast<unsigned long>(iV));
			}
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( float iV )
		{
			return PyFloat_FromDouble(static_cast<double>(iV));
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( double iV )
		{
			return PyFloat_FromDouble(iV);
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( long double iV )
		{
			return PyFloat_FromDouble(static_cast<long double>(iV));
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( const char* iV )
		{
			return PyString_FromString(iV);
		}

		/** @ingroup Python
		 */
		inline PyObject* pyBuildSimpleObject( const std::string& iV )
		{
			return PyString_FromString(iV.c_str());
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
