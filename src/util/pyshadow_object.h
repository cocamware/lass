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
 *
 *	@note 
 *		This header bundles the _EXPERIMENTAL_ code for quasi automatic 
 *		wrapping of C++ object hierarchies into python shadow objects.  This
 *		code is still heavily under development and not ready for production use.
 */


namespace lass
{
	namespace python
	{
		template<typename T, typename P  >
		class PyObjectPlusPlus : public P
		{
			PY_HEADER( P );

			typedef T	TPyClassSelf;
			typedef P	TPyClassParent;
			typedef PyObjectPlusPlus<T,P>	TSelf;

		public:
            virtual std::string	pyRepr(void) { return "PyObjectPlusPlus object at " + util::stringCast<std::string>(this); }
			virtual std::string	pyStr(void) { return "PyObjectPlusPlus object string at " + util::stringCast<std::string>(this); }

			PyObjectPlusPlus(): P() { /*ob_type = &Type; _Py_NewReference();*/ }
			virtual ~PyObjectPlusPlus() {}
		};

		template<typename T, typename P  >
		class PyShadowObject : public PyObjectPlusPlus<T,P>
		{
		public:
			typedef T	RealObject;
			T*	value_;

			PyShadowObject() : value_( new T() ) {}
			operator T() { return *value_; }
			virtual ~PyShadowObject() {}
		};
/*
        template <typename CppBase>
        class PyShadowBase: public PyObjectPlus
        {
            PY_HEADER(PyObjectPlus)
        public:
            typedef CppBase TCppBase;
            virtual ~PyShadowBase() { if (ownership_ == oOwner) delete value_; value_ = 0; }
            TCppBase* value() const { return value_; }

        protected:
            enum Ownership { oOwner, oBorrowed }
            PyShadowBase(CppBase* iValue, Ownership iOwnership, PyTypeObject* iType): 
                value_(iValue), ownership_(iOwnership), PyObjectPlus(iType) {}

        private:
            TCppBase* value_;
            Ownership ownership_;
        };

        template <typename CppClass, typename PyShadowParent = PyShadowBase<CppClass> >
        class PyShadowObject: public PyShadowParent
        {
             PY_HEADER(PyShadowParent)
       public:
            typedef CppClass RealObject;

            PyShadowObject(const CppClass& iByCopy): 
                PyShadowParent(new CppClass(iByCopy), oOwner, &Type) {}

            PyShadowObject(CppClass& iByBorrowedReference):
                PyShadowParent(&iByBorrowedReference, oBorrowed, &Type) {}

            PyShadowObject(CppClass* iByBorrowedPointer): 
                PyShadowParent(iByBorrowedPointer, oBorrowed, &Type) {}

            PyShadowObject(std::auto_ptr<CppClass> iBySinkedPointer): 
                PyShadowParent(iBySinkedPointer, oOwner, &Type) {}

        protected:
            PyShadowObject(CppBase* iValue, Ownership iOwnership, PyTypeObject* iType):
                 PyShadowParent(iValue, iOwnership, iType) {}
        };
*/
#ifdef LASS_NO_TEMPLATE_PARTIAL_SPECIALIZATION
		template< typename Q >
		struct IsShadowType
		{
			template< typename T, typename P >	static meta::True isShadow( meta::Type2Type< PyShadowObject<T,P> > iDontExist );
            //template <typename CppClass> static meta::True isShadow( PyShadowBase<CppBase>* );
			static meta::False  isShadow( ... );

            enum { value = sizeof(isShadow(meta::Type2Type<Q>()))==sizeof(meta::True) };

		private:
			template< int I > struct ResolveShadowRealObjectType
			{
				template< typename R >	struct Binder
				{
					typedef R::RealObject	Type;
				};
			};
			template<> struct ResolveShadowRealObjectType<0>
			{
				template< typename R >	struct Binder
				{
					typedef R	Type;
				};
			};
		
		public:
			typedef ResolveShadowRealObjectType< value >::Binder< Q >::Type RealObjectType;
		};
#else
		template< typename T >	
		struct IsShadowType 
		{
			enum { value = 0 };
			typedef T	RealObjectType;
		};

		template< typename T, typename P >  
		struct IsShadowType< PyShadowObject<T, P> > 
		{
			enum { value = 1 };
			typedef typename T::RealObject	RealObjectType;
		};
#endif

	}
}



#define PY_SHADOW_CLASS_METHOD_EX( Object, method, pyFunction, pyName, pyDoc )\
	inline PyObject* pyFunction( PyObject* iObject, PyObject* iArgs )\
	{\
		Object* object = static_cast<Object*>(iObject);\
		return lass::python::impl::PyCallMethod<Object ::RealObject>::call( iArgs,  ((Object ::RealObject*) (*object)), Object ::RealObject:: method );\
	}\
	EXECUTE( Object::Methods.insert(Object::Methods.begin(),\
		lass::python::createPyMethodDef(pyName, (PyCFunction) pyFunction, METH_VARARGS , pyDoc)); )

/*
#define PY_DECLARE_SHADOW_CLASS(CppClass__, PyShadowParent__)\
namespace lass\
{\
namespace python\
{\
inline PyObject* pyBuildSimpleObject( std::auto_ptr<CppClass__> iBySinkedPointer )\
{\
    return new PyShadowObject<CppClass__, PyShadowParent__>( iBySinkedPointer );\
}\
inline PyObject* pyBuildSimpleObject( CppClass__* iByBorrowedPointer )\
{\
    return new PyShadowObject<CppClass__, PyShadowParent__>( iByBorrowedPointer );\
}\
inline int pyGetSimpleObject( PyObject* iValue, CppClass__*& oByBorrowedPointer )\
{\
    oByBorrowedPointer = static_cast<CppClass__*>( shadowObject->value() );\
}\
inline PyObject* pyBuildSimpleObject( const CppClass__& iByCopy )\
{\
    return new PyShadowObject<CppClass__, PyShadowParent__>( iByCopy );\
}\
inline int pyGetSimpleObject( PyObject* iValue, CppClass__& oByCopy )\
{\
    CppClass__* realObject;\
    const int error = pyGetSimpleObject( iValue, realObject );\
    if (!error)\
    {\
        oByCopy = *realObject;\
    }\
    return error;\
}
*/

