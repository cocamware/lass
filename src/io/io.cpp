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

#include "io_common.h"
#include "binary_i_stream.h"
#include "binary_o_stream.h"

int main(int argc, char* argv[])
{
	{
	lass::io::BinaryOStream testO("temp.txt");

	testO << (long)5;
	testO << std::string("This is a test");
	}

	{
	lass::io::BinaryIStream testI("temp.txt");
	long t;
	std::string tstr;

	testI >> t;
	testI >> tstr;

	std::cout << t << tstr << std::endl;
	}

	std::cin.ignore();

	return 0;
}
