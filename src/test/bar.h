/**	@file
 *  @internal
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_BAR_H
#define LASS_GUARDIAN_OF_INCLUSION_TEST_BAR_H

#include "test_common.h"
#include "../util/pyobject_plus.h"
#include "../util/pyobject_util.h"
#include "foo.h"

#include "../prim/pyobject_util.h"
#include "../prim/aabb_3d.h"
#include "../prim/xyz.h"

namespace lass
{
namespace test
{
	class Bar : public lass::python::PyObjectPlus
	{
		PY_HEADER( lass::python::PyObjectPlus );

		virtual std::string	pyRepr(void);
		virtual std::string	pyStr(void);

		int	privateInt_;
		std::string privateString_;
		PythonFooPtr	ptr2Foo_;
		float coolMember_;

	public:
		Bar();
		Bar( int iA, const std::string& iB );
		virtual ~Bar();

		float aMoreComplexFunction( float iA, float iB );
		void  testAutomaticFunctionExport( int iA, float iB );
        void complexArguments( const std::string& iA );
		prim::Point3D<float> primArguments(const prim::Aabb3D<float>& iAabb, prim::XYZ iAxis);

		void overloaded( int iA );
		void overloaded( const std::string& iB );

		static int aStaticMethod( float ia );

		int	getInt() const;
		void setInt( int iArg );

		const PythonFooPtr& getFoo() const;
		void setFoo( const PythonFooPtr& iFoo);

		const PythonFoo& getFooRef() const;
		void setFooRef( const PythonFoo& iFoo);

		float coolMember() const;
		float& coolMember();

		int publicInt;
	};
	
	void listInfo( PyObject* iObject );


}

}

#endif
