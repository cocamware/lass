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
 *	Copyright (C) 2004-2025 the Initial Developer.
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
#include "../lass/python/python_api.h"
#include "foo.h"

#include "../lass/prim/aabb_3d.h"
#include "../lass/prim/xyz.h"
#include "../lass/prim/transformation_3d.h"
#include "../lass/prim/color_rgba.h"

#include "../lass/stde/vector_map.h"
#include "../lass/stde/static_vector.h"

namespace lass
{
namespace test
{
	enum EnumMadness
	{
		emIsThis = 0x3,
		emAnEnum = 0x7
	};

	class Bar : public lass::python::PyObjectPlus
	{
		PY_HEADER( lass::python::PyObjectPlus )
	private:

		int privateInt_;
		std::string privateString_;
		PythonFooPtr    ptr2Foo_;
		float coolMember_;

		typedef std::map<std::string, std::string> TMap;
		TMap map_;

	public:
		enum class Shape: short
		{
			Circle,
			Square,
			Triangle
		};

		Bar();
		Bar( int iA, const std::string& iB );
		virtual ~Bar();

		virtual float aMoreComplexFunction( float iA, float iB );
		std::string testAutomaticFunctionExport( int iA, float iB );
		std::string complexArguments( const std::string& iA );
		std::string primArguments(const prim::Aabb3D<float>& iAabb, prim::XYZ iAxis,
			const prim::Transformation3D<double>& iTransformation);
		prim::ColorRGBA rgba(const prim::ColorRGBA& c) noexcept;

		void overloaded( int iA );
		void overloaded( const std::string& iB );

		int operator()( int iA ) const;
		std::string call(const std::string& iA) const;

		static int aStaticMethod( float ia );

		static Shape passShape(Shape shape);
		static Shape getShape();
		static Shape badShape();
		static bool isTriangle(Shape shape);

		int getInt() const noexcept;
		int getInt() noexcept;
		void setInt( int iArg ) noexcept;

		const PythonFooPtr& getFoo() const;
		void setFoo( const PythonFooPtr& iFoo);

		const PythonFoo& getFooRef() const;
		void setFooRef( const PythonFoo& iFoo);

		float coolMember() const;
		float& coolMember();

		int publicInt;
		util::SharedPtr< std::map<std::string, std::string> > writeableMap;
		util::SharedPtr< stde::vector_map<std::string, std::string> > writeableVectorMap;
		util::SharedPtr< std::vector< double > > writeableVector;
		util::SharedPtr< std::list< double > > writeableList;
		util::SharedPtr< std::deque< double > > writeableDeque;
		util::SharedPtr< stde::static_vector< double, 128 > > writeableStaticVector;

		const std::map<std::string, std::string> constMap;
		const std::vector< double > constVector;
		const std::list< double > constList;
		const std::deque< double > constDeque;

		std::map<std::string, std::string> testConstMap(const std::map<std::string, std::string>&);

		class InnerClass : public lass::python::PyObjectPlus
		{
			PY_HEADER( lass::python::PyObjectPlus )
		public:
			InnerClass(const std::string& iSayWhat);
			std::string talkTo(const std::string& iName);
		private:
			std::string sayWhat_;
		};

		virtual void* opaquePointer() const;

		bool operator==(const Bar& other) const;
		bool operator<(const Bar& other) const;
	
		// map protocol
		std::string getItem(const std::string& key) const;
		void setItem(const std::string& key, const std::string& value);
		void setItem2(const std::string& key, int value);
		void delItem(const std::string& key);
		void delItem2(int index);
		bool contains(const std::string& key);
		size_t size() const;
		lass::python::PyIteratorRange* iter();

		static std::unique_ptr<Bar> makeUniquePtr();
	};

	bool operator!=(const Bar& a, const Bar& b);
	bool operator>(const Bar& a, const Bar& b);
	bool operator<=(const Bar& a, const Bar& b);
	bool operator>=(const Bar& a, const Bar& b);

	class DerivedBar : public Bar
	{
		PY_HEADER( Bar )

	public:
		DerivedBar() {}
		~DerivedBar() {}
		float aMoreComplexFunction( float iA, float iB ) override;
	};

	typedef lass::python::PyObjectPtr<Bar>::Type TBarPtr;

	inline TBarPtr testPolymorphism()
	{
		return TBarPtr(new DerivedBar);
	}

	void listInfo( const python::TPyObjPtr& iObject );
	std::string freeMethodA(const Bar& bar, const std::string& a);
	std::string freeMethodB(Bar& bar, const std::string& a);
	float freeCall(const Bar& bar, float a);
	std::string freeStr(const Bar& bar);
	std::string freeRepr(const Bar& bar);
}

}

PY_SHADOW_STR_ENUM(LASS_DLL_EXPORT, lass::test::Bar::Shape);

#endif
