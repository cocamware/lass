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
 *	Copyright (C) 2024 the Initial Developer.
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

#include "test_common.h"

#include "../lass/prim/xy.h"
#include "../lass/prim/xyz.h"
#include "../lass/prim/xyzw.h"

namespace lass
{
namespace test
{

void testPrimXY()
{
	using lass::prim::XY;

	XY xy;
	LASS_TEST_CHECK_EQUAL(xy.axis(), 'x');
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xy), 0);
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(xy), 0);

	xy = XY('x');
	LASS_TEST_CHECK_EQUAL(xy.axis(), 'x');
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xy), 0);
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(xy), 0);

	xy = XY('y');
	LASS_TEST_CHECK_EQUAL(xy.axis(), 'y');
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xy), 1);
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(xy), 1);

	xy = XY('X');
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	xy = XY('Y');
	LASS_TEST_CHECK_EQUAL(xy, 'y');

	LASS_TEST_CHECK_THROW(XY('a'), lass::util::Exception);

	xy = XY(std::string("x"));
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	xy = XY(std::string("y"));
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	xy = XY(std::string("X"));
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	xy = XY(std::string("Y"));
	LASS_TEST_CHECK_EQUAL(xy, 'y');

	LASS_TEST_CHECK_THROW(XY(std::string("a")), lass::util::Exception);
	LASS_TEST_CHECK_THROW(XY(std::string("xx")), lass::util::Exception);

	xy = XY(static_cast<char>(0));
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	xy = XY(static_cast<char>(1));
	LASS_TEST_CHECK_EQUAL(xy, 'y');

	xy = XY(static_cast<int>(0));
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xy), 0);
	xy = XY(static_cast<int>(1));
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xy), 1);
	xy = XY(static_cast<int>(2));
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xy), 0);
	xy = XY(static_cast<int>(-1));
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xy), 1);

	xy = 'x';
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	xy = 'y';
	LASS_TEST_CHECK_EQUAL(xy, 'y');

	LASS_TEST_CHECK_THROW(xy = 'a', lass::util::Exception);

	xy = 'x';
	XY xy2 = ++xy;
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	LASS_TEST_CHECK_EQUAL(xy2, 'y');
	xy2 = ++xy;
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	LASS_TEST_CHECK_EQUAL(xy2, 'x');
	xy2 = --xy;
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	LASS_TEST_CHECK_EQUAL(xy2, 'y');
	xy2 = --xy;
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	LASS_TEST_CHECK_EQUAL(xy2, 'x');
	xy2 = xy++;
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	LASS_TEST_CHECK_EQUAL(xy2, 'x');
	xy2 = xy++;
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	LASS_TEST_CHECK_EQUAL(xy2, 'y');
	xy2 = xy--;
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	LASS_TEST_CHECK_EQUAL(xy2, 'x');
	xy2 = xy--;
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	LASS_TEST_CHECK_EQUAL(xy2, 'y');

	xy = 'x';
	xy += 1;
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	xy += 2;
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	xy += 3;
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	xy -= 1;
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	xy -= 2;
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	xy -= 3;
	LASS_TEST_CHECK_EQUAL(xy, 'x');

	xy = xy + 1;
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	xy = xy + 2;
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	xy = xy + 3;
	LASS_TEST_CHECK_EQUAL(xy, 'x');
	xy = xy - 1;
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	xy = xy - 2;
	LASS_TEST_CHECK_EQUAL(xy, 'y');
	xy = xy - 3;
	LASS_TEST_CHECK_EQUAL(xy, 'x');

	xy = 'y';
	xy2 = 'y';
	LASS_TEST_CHECK(xy == xy2);
	LASS_TEST_CHECK(xy == 1);
	LASS_TEST_CHECK(xy == int(1));
	LASS_TEST_CHECK(xy == size_t(1));
	LASS_TEST_CHECK(xy == 'y');
	LASS_TEST_CHECK(xy == std::string("y"));
	LASS_TEST_CHECK(1 == xy);
	LASS_TEST_CHECK(int(1) == xy);
	LASS_TEST_CHECK(size_t(1) == xy);
	LASS_TEST_CHECK('y' == xy);
	LASS_TEST_CHECK(std::string("y") == xy);

	xy2 = 'x';
	LASS_TEST_CHECK(xy != xy2);
	LASS_TEST_CHECK(xy != 0);
	LASS_TEST_CHECK(xy != int(0));
	LASS_TEST_CHECK(xy != size_t(0));
	LASS_TEST_CHECK(xy != 'x');
	LASS_TEST_CHECK(xy != std::string("x"));
	LASS_TEST_CHECK(0 != xy);
	LASS_TEST_CHECK(int(0) != xy);
	LASS_TEST_CHECK(size_t(0) != xy);
	LASS_TEST_CHECK('x' != xy);
	LASS_TEST_CHECK(std::string("x") != xy);

	std::ostringstream stream;
	xy = 'x';
	stream << xy;
	LASS_TEST_CHECK_EQUAL(stream.str(), std::string("x"));
	stream.str("");
	xy = 'y';
	stream << xy;
	LASS_TEST_CHECK_EQUAL(stream.str(), std::string("y"));
}


void testPrimXYZ()
{
	using lass::prim::XYZ;

	XYZ xyz;
	LASS_TEST_CHECK_EQUAL(xyz.axis(), 'x');
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyz), 0);
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(xyz), 0);

	xyz = XYZ('x');
	LASS_TEST_CHECK_EQUAL(xyz.axis(), 'x');
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyz), 0);
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(xyz), 0);

	xyz = XYZ('y');
	LASS_TEST_CHECK_EQUAL(xyz.axis(), 'y');
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyz), 1);
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(xyz), 1);

	xyz = XYZ('z');
	LASS_TEST_CHECK_EQUAL(xyz.axis(), 'z');
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyz), 2);
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(xyz), 2);

	xyz = XYZ('X');
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	xyz = XYZ('Y');
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	xyz = XYZ('Z');
	LASS_TEST_CHECK_EQUAL(xyz, 'z');

	LASS_TEST_CHECK_THROW(XYZ('a'), lass::util::Exception);

	xyz = XYZ(std::string("x"));
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	xyz = XYZ(std::string("y"));
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	xyz = XYZ(std::string("z"));
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	xyz = XYZ(std::string("X"));
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	xyz = XYZ(std::string("Y"));
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	xyz = XYZ(std::string("Z"));
	LASS_TEST_CHECK_EQUAL(xyz, 'z');

	LASS_TEST_CHECK_THROW(XYZ(std::string("a")), lass::util::Exception);
	LASS_TEST_CHECK_THROW(XYZ(std::string("xx")), lass::util::Exception);

	xyz = XYZ(static_cast<char>(0));
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	xyz = XYZ(static_cast<char>(1));
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	xyz = XYZ(static_cast<char>(2));
	LASS_TEST_CHECK_EQUAL(xyz, 'z');

	xyz = XYZ(static_cast<int>(0));
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyz), 0);
	xyz = XYZ(static_cast<int>(1));
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyz), 1);
	xyz = XYZ(static_cast<int>(2));
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyz), 2);
	xyz = XYZ(static_cast<int>(3));
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyz), 0);
	xyz = XYZ(static_cast<int>(-1));
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyz), 2);

	xyz = 'x';
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	xyz = 'y';
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	xyz = 'z';
	LASS_TEST_CHECK_EQUAL(xyz, 'z');

	LASS_TEST_CHECK_THROW(xyz = 'a', lass::util::Exception);

	xyz = 'x';
	XYZ xyz2 = ++xyz;
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	LASS_TEST_CHECK_EQUAL(xyz2, 'y');
	xyz2 = ++xyz;
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	LASS_TEST_CHECK_EQUAL(xyz2, 'z');
	xyz2 = ++xyz;
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	LASS_TEST_CHECK_EQUAL(xyz2, 'x');
	xyz2 = --xyz;
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	LASS_TEST_CHECK_EQUAL(xyz2, 'z');
	xyz2 = --xyz;
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	LASS_TEST_CHECK_EQUAL(xyz2, 'y');
	xyz2 = --xyz;
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	LASS_TEST_CHECK_EQUAL(xyz2, 'x');
	xyz2 = xyz++;
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	LASS_TEST_CHECK_EQUAL(xyz2, 'x');
	xyz2 = xyz++;
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	LASS_TEST_CHECK_EQUAL(xyz2, 'y');
	xyz2 = xyz++;
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	LASS_TEST_CHECK_EQUAL(xyz2, 'z');
	xyz2 = xyz--;
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	LASS_TEST_CHECK_EQUAL(xyz2, 'x');
	xyz2 = xyz--;
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	LASS_TEST_CHECK_EQUAL(xyz2, 'z');
	xyz2 = xyz--;
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	LASS_TEST_CHECK_EQUAL(xyz2, 'y');

	xyz = 'x';
	xyz += 1;
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	xyz += 2;
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	xyz += 3;
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	xyz += 5;
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	xyz -= 1;
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	xyz -= 2;
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	xyz -= 3;
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	xyz -= 5;
	LASS_TEST_CHECK_EQUAL(xyz, 'x');

	xyz = xyz + 1;
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	xyz = xyz + 2;
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	xyz = xyz + 3;
	LASS_TEST_CHECK_EQUAL(xyz, 'x');
	xyz = xyz + 5;
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	xyz = xyz - 1;
	LASS_TEST_CHECK_EQUAL(xyz, 'y');
	xyz = xyz - 2;
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	xyz = xyz - 3;
	LASS_TEST_CHECK_EQUAL(xyz, 'z');
	xyz = xyz - 5;
	LASS_TEST_CHECK_EQUAL(xyz, 'x');

	xyz = 'y';
	xyz2 = 'y';
	LASS_TEST_CHECK(xyz == xyz2);
	LASS_TEST_CHECK(xyz == 1);
	LASS_TEST_CHECK(xyz == int(1));
	LASS_TEST_CHECK(xyz == size_t(1));
	LASS_TEST_CHECK(xyz == 'y');
	LASS_TEST_CHECK(xyz == std::string("y"));
	LASS_TEST_CHECK(1 == xyz);
	LASS_TEST_CHECK(int(1) == xyz);
	LASS_TEST_CHECK(size_t(1) == xyz);
	LASS_TEST_CHECK('y' == xyz);
	LASS_TEST_CHECK(std::string("y") == xyz);

	xyz2 = 'x';
	LASS_TEST_CHECK(xyz != xyz2);
	LASS_TEST_CHECK(xyz != 0);
	LASS_TEST_CHECK(xyz != int(0));
	LASS_TEST_CHECK(xyz != size_t(0));
	LASS_TEST_CHECK(xyz != 'x');
	LASS_TEST_CHECK(xyz != std::string("x"));
	LASS_TEST_CHECK(0 != xyz);
	LASS_TEST_CHECK(int(0) != xyz);
	LASS_TEST_CHECK(size_t(0) != xyz);
	LASS_TEST_CHECK('x' != xyz);
	LASS_TEST_CHECK(std::string("x") != xyz);

	std::ostringstream stream;
	xyz = 'x';
	stream << xyz;
	LASS_TEST_CHECK_EQUAL(stream.str(), std::string("x"));
	stream.str("");
	xyz = 'y';
	stream << xyz;
	LASS_TEST_CHECK_EQUAL(stream.str(), std::string("y"));
	stream.str("");
	xyz = 'z';
	stream << xyz;
	LASS_TEST_CHECK_EQUAL(stream.str(), std::string("z"));
}



void testPrimXYZW()
{
	using lass::prim::XYZW;

	XYZW xyzw;
	LASS_TEST_CHECK_EQUAL(xyzw.axis(), 'x');
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyzw), 0);
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(xyzw), 0);

	xyzw = XYZW('x');
	LASS_TEST_CHECK_EQUAL(xyzw.axis(), 'x');
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyzw), 0);
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(xyzw), 0);

	xyzw = XYZW('y');
	LASS_TEST_CHECK_EQUAL(xyzw.axis(), 'y');
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyzw), 1);
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(xyzw), 1);

	xyzw = XYZW('z');
	LASS_TEST_CHECK_EQUAL(xyzw.axis(), 'z');
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyzw), 2);
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(xyzw), 2);

	xyzw = XYZW('w');
	LASS_TEST_CHECK_EQUAL(xyzw.axis(), 'w');
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyzw), 3);
	LASS_TEST_CHECK_EQUAL(static_cast<size_t>(xyzw), 3);

	xyzw = XYZW('X');
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	xyzw = XYZW('Y');
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	xyzw = XYZW('Z');
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	xyzw = XYZW('W');
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');

	LASS_TEST_CHECK_THROW(XYZW('a'), lass::util::Exception);

	xyzw = XYZW(std::string("x"));
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	xyzw = XYZW(std::string("y"));
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	xyzw = XYZW(std::string("z"));
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	xyzw = XYZW(std::string("w"));
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');
	xyzw = XYZW(std::string("X"));
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	xyzw = XYZW(std::string("Y"));
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	xyzw = XYZW(std::string("Z"));
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	xyzw = XYZW(std::string("W"));
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');

	LASS_TEST_CHECK_THROW(XYZW(std::string("a")), lass::util::Exception);
	LASS_TEST_CHECK_THROW(XYZW(std::string("xx")), lass::util::Exception);

	xyzw = XYZW(static_cast<char>(0));
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	xyzw = XYZW(static_cast<char>(1));
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	xyzw = XYZW(static_cast<char>(2));
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	xyzw = XYZW(static_cast<char>(3));
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');

	xyzw = XYZW(static_cast<int>(0));
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyzw), 0);
	xyzw = XYZW(static_cast<int>(1));
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyzw), 1);
	xyzw = XYZW(static_cast<int>(2));
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyzw), 2);
	xyzw = XYZW(static_cast<int>(3));
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyzw), 3);
	xyzw = XYZW(static_cast<int>(4));
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyzw), 0);
	xyzw = XYZW(static_cast<int>(-1));
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');
	LASS_TEST_CHECK_EQUAL(static_cast<int>(xyzw), 3);

	xyzw = 'x';
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	xyzw = 'y';
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	xyzw = 'z';
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	xyzw = 'w';
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');

	LASS_TEST_CHECK_THROW(xyzw = 'a', lass::util::Exception);

	xyzw = 'x';
	XYZW xyzw2 = ++xyzw;
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'y');
	xyzw2 = ++xyzw;
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'z');
	xyzw2 = ++xyzw;
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'w');
	xyzw2 = ++xyzw;
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'x');
	xyzw2 = --xyzw;
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'w');
	xyzw2 = --xyzw;
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'z');
	xyzw2 = --xyzw;
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'y');
	xyzw2 = --xyzw;
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'x');
	xyzw2 = xyzw++;
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'x');
	xyzw2 = xyzw++;
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'y');
	xyzw2 = xyzw++;
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'z');
	xyzw2 = xyzw++;
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'w');
	xyzw2 = xyzw--;
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'x');
	xyzw2 = xyzw--;
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'w');
	xyzw2 = xyzw--;
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'z');
	xyzw2 = xyzw--;
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	LASS_TEST_CHECK_EQUAL(xyzw2, 'y');

	xyzw = 'x';
	xyzw += 1;
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	xyzw += 2;
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');
	xyzw += 3;
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	xyzw += 5;
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');
	xyzw -= 1;
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	xyzw -= 2;
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	xyzw -= 3;
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	xyzw -= 5;
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');

	xyzw = xyzw + 1;
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	xyzw = xyzw + 2;
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');
	xyzw = xyzw + 3;
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	xyzw = xyzw + 5;
	LASS_TEST_CHECK_EQUAL(xyzw, 'w');
	xyzw = xyzw - 1;
	LASS_TEST_CHECK_EQUAL(xyzw, 'z');
	xyzw = xyzw - 2;
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');
	xyzw = xyzw - 3;
	LASS_TEST_CHECK_EQUAL(xyzw, 'y');
	xyzw = xyzw - 5;
	LASS_TEST_CHECK_EQUAL(xyzw, 'x');

	xyzw = 'y';
	xyzw2 = 'y';
	LASS_TEST_CHECK(xyzw == xyzw2);
	LASS_TEST_CHECK(xyzw == 1);
	LASS_TEST_CHECK(xyzw == int(1));
	LASS_TEST_CHECK(xyzw == size_t(1));
	LASS_TEST_CHECK(xyzw == 'y');
	LASS_TEST_CHECK(xyzw == std::string("y"));
	LASS_TEST_CHECK(1 == xyzw);
	LASS_TEST_CHECK(int(1) == xyzw);
	LASS_TEST_CHECK(size_t(1) == xyzw);
	LASS_TEST_CHECK('y' == xyzw);
	LASS_TEST_CHECK(std::string("y") == xyzw);

	xyzw2 = 'x';
	LASS_TEST_CHECK(xyzw != xyzw2);
	LASS_TEST_CHECK(xyzw != 0);
	LASS_TEST_CHECK(xyzw != int(0));
	LASS_TEST_CHECK(xyzw != size_t(0));
	LASS_TEST_CHECK(xyzw != 'x');
	LASS_TEST_CHECK(xyzw != std::string("x"));
	LASS_TEST_CHECK(0 != xyzw);
	LASS_TEST_CHECK(int(0) != xyzw);
	LASS_TEST_CHECK(size_t(0) != xyzw);
	LASS_TEST_CHECK('x' != xyzw);
	LASS_TEST_CHECK(std::string("x") != xyzw);

	std::ostringstream stream;
	xyzw = 'x';
	stream << xyzw;
	LASS_TEST_CHECK_EQUAL(stream.str(), std::string("x"));
	xyzw = 'y';
	stream.str("");
	stream << xyzw;
	LASS_TEST_CHECK_EQUAL(stream.str(), std::string("y"));
	xyzw = 'z';
	stream.str("");
	stream << xyzw;
	LASS_TEST_CHECK_EQUAL(stream.str(), std::string("z"));
	xyzw = 'w';
	stream.str("");
	stream << xyzw;
	LASS_TEST_CHECK_EQUAL(stream.str(), std::string("w"));
}


TUnitTest test_prim_xyz()
{
	return TUnitTest{
		LASS_TEST_CASE(testPrimXY),
		LASS_TEST_CASE(testPrimXYZ),
		LASS_TEST_CASE(testPrimXYZW)
	};
}



}

}

// EOF
