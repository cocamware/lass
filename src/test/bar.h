/**	@file
 *	@author Bram de Greve (bramz@users.sourceforge.net)
 *	@author Tom De Muer (tomdemuer@users.sourceforge.net)
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
 *	Copyright (C) 2004-2007 the Initial Developer.
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_BAR_H
#define LASS_GUARDIAN_OF_INCLUSION_TEST_BAR_H

#include "test_common.h"
#include "../util/python_api.h"
#include "foo.h"

#include "../prim/aabb_3d.h"
#include "../prim/xyz.h"
#include "../prim/transformation_3d.h"

namespace lass
{
namespace test
{
	class Bar : public lass::python::PyObjectPlus
	{
		PY_HEADER( lass::python::PyObjectPlus )
	private:

		int privateInt_;
		std::string privateString_;
		PythonFooPtr    ptr2Foo_;
		float coolMember_;

	public:
		Bar();
		Bar( int iA, const std::string& iB );
		virtual ~Bar();
		virtual std::string doPyRepr(void);
		virtual std::string doPyStr(void);

		bool operator==(const Bar& iOther) { return publicInt==iOther.publicInt;}
		//bool operator==(const Bar const* iOther) { return iOther && (publicInt==iOther->publicInt);}

		virtual float aMoreComplexFunction( float iA, float iB );
		void  testAutomaticFunctionExport( int iA, float iB );
		void complexArguments( const std::string& iA );
		prim::Point3D<float> primArguments(const prim::Aabb3D<float>& iAabb, prim::XYZ iAxis,
			const prim::Transformation3D<double>& iTransformation);

		void overloaded( int iA );
		void overloaded( const std::string& iB );

		int operator()( int iA ) const;
		std::string call(const std::string& iA) const;

		static int aStaticMethod( float ia );

		int getInt() const;
		void setInt( int iArg );

		const PythonFooPtr& getFoo() const;
		void setFoo( const PythonFooPtr& iFoo);

		const PythonFoo& getFooRef() const;
		void setFooRef( const PythonFoo& iFoo);

		float coolMember() const;
		float& coolMember();

		int publicInt;
		std::map<std::string, std::string> writeableMap;
		std::vector< double > writeableVector;
		std::list< double > writeableList;
		std::deque< double > writeableDeque;

		const std::map<std::string, std::string> constMap;
		const std::vector< double > constVector;
		const std::list< double > constList;
		const std::deque< double > constDeque;

		class InnerClass : public lass::python::PyObjectPlus
		{
			PY_HEADER( lass::python::PyObjectPlus )
		public:
			InnerClass(const std::string& iSayWhat);
			std::string talkTo(const std::string& iName);
		private:
			std::string sayWhat_;
		};
	};

	class DerivedBar : public Bar
	{
		PY_HEADER( Bar );

	public:
		DerivedBar() {}
		virtual ~DerivedBar() {}
		virtual float aMoreComplexFunction( float iA, float iB );
	};

	typedef lass::python::PyObjectPtr<Bar>::Type TBarPtr;

	inline TBarPtr testPolymorphism()
	{
		return TBarPtr(new DerivedBar);
	}

	void listInfo( PyObject* iObject );
	void freeMethodA(const Bar& bar, const std::string& a);
	void freeMethodB(Bar* bar, const std::string& a);
	float freeCall(const Bar* bar, float a);
	std::string freeStr(const Bar* bar);
	std::string freeRepr(const Bar* bar);
}

}

#endif
