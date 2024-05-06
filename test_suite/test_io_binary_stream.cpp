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
 *	Copyright (C) 2004-2024 the Initial Developer.
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

#include "../lass/io/binary_i_file.h"
#include "../lass/io/binary_o_file.h"
#include "../lass/io/binary_i_memory_map.h"
#include "../lass/io/binary_i_memory_block.h"

#if LASS_COMPILER_TYPE == LASS_COMPILER_TYPE_MSVC
#	pragma warning(disable: 4996) // 'fopen': This function or variable may be unsafe. Consider using fopen_s instead
#endif


namespace lass
{
namespace test
{

void testIoStreamBase()
{
	io::BinaryIFile stream; // cannot directly create an io::StreamBase
	LASS_TEST_CHECK_EQUAL(stream.rdstate(), std::ios_base::goodbit);
	LASS_TEST_CHECK(stream.good());
	LASS_TEST_CHECK(!stream.eof());
	LASS_TEST_CHECK(!stream.fail());
	LASS_TEST_CHECK(!stream.bad());
	LASS_TEST_CHECK_EQUAL(static_cast<bool>(stream), true);
	LASS_TEST_CHECK_EQUAL(!stream, false);

	stream.clear(std::ios_base::eofbit);
	LASS_TEST_CHECK_EQUAL(stream.rdstate(), std::ios_base::eofbit);
	LASS_TEST_CHECK(stream);
	LASS_TEST_CHECK(!stream.good());
	LASS_TEST_CHECK(stream.eof());
	LASS_TEST_CHECK(!stream.fail());
	LASS_TEST_CHECK(!stream.bad());
	LASS_TEST_CHECK_EQUAL(static_cast<bool>(stream), true); // eof is still "true" here.
	LASS_TEST_CHECK_EQUAL(!stream, false);

	stream.clear(std::ios_base::failbit);
	LASS_TEST_CHECK_EQUAL(stream.rdstate(), std::ios_base::failbit);
	LASS_TEST_CHECK(!stream.good());
	LASS_TEST_CHECK(!stream.eof());
	LASS_TEST_CHECK(stream.fail());
	LASS_TEST_CHECK(!stream.bad());
	LASS_TEST_CHECK_EQUAL(static_cast<bool>(stream), false);
	LASS_TEST_CHECK_EQUAL(!stream, true);

	stream.clear(std::ios_base::badbit);
	LASS_TEST_CHECK_EQUAL(stream.rdstate(), std::ios_base::badbit);
	LASS_TEST_CHECK(!stream);
	LASS_TEST_CHECK(!stream.good());
	LASS_TEST_CHECK(!stream.eof());
	LASS_TEST_CHECK(stream.fail()); // bad is also fail
	LASS_TEST_CHECK(stream.bad());
	LASS_TEST_CHECK_EQUAL(static_cast<bool>(stream), false);
	LASS_TEST_CHECK_EQUAL(!stream, true);

	stream.clear();
	LASS_TEST_CHECK(stream.rdstate() == std::ios_base::goodbit);

	stream.setstate(std::ios_base::eofbit);
	LASS_TEST_CHECK_EQUAL(stream.rdstate(), std::ios_base::eofbit);
	stream.setstate(std::ios_base::failbit | std::ios_base::badbit);
	LASS_TEST_CHECK(!stream.good());
	LASS_TEST_CHECK(stream.eof());
	LASS_TEST_CHECK(stream.fail());
	LASS_TEST_CHECK(stream.bad());

	stream.setstate(std::ios_base::goodbit); // this doesn't help :-)
	LASS_TEST_CHECK(!stream.good()); // it's still bad
}


void testIoBinaryStream()
{
	using namespace io;

	const num::Tint8 a = -5;
	const num::Tuint8 b = 6;
	const num::Tint16 c = -700;
	const num::Tuint16 d = 800;
	const num::Tint32 e = -900000;
	const num::Tuint32 f = 1000000;
	const num::Tint64 g = -123456789123456789;
	const num::Tuint64 h = 123456789123456789;
	num::Tfloat32 i = 3.14f;
	num::Tfloat64 j = 1.23456789012345;
	std::vector<std::string> k = { "This", "is", "a", "test", "" };
	std::complex<double> l(3., 4.);
	bool m = true;
	std::string n = std::string{ "null" } + '\0' + "included";
	const char* o = "This is a string";
	const void* p = o;
#if LASS_HAVE_WCHAR_SUPPORT
	const wchar_t* v = L"\x2653\x212e\x0142\x029f\x263a \x0428\x263a\x0491\x2113\x1e13\x203c";
	const std::wstring w = v;
#endif

	{
		BinaryOFile testO("temp.txt");
		testO << a << b << c << d << e << f << g << h << i << j << k << l << m << n << o << p;
#if LASS_HAVE_WCHAR_SUPPORT
		testO << v << w;
#endif
	}

	{
		BinaryIFile testI("temp.txt");
		num::Tint8 a2;
		num::Tuint8 b2;
		num::Tint16 c2;
		num::Tuint16 d2;
		num::Tint32 e2;
		num::Tuint32 f2;
		num::Tint64 g2;
		num::Tuint64 h2;
		num::Tfloat32 i2;
		num::Tfloat64 j2;
		std::vector<std::string> k2;
		std::complex<double> l2;
		bool m2;
		std::string n2;
		std::string o2;
		void* p2;

		testI >> a2 >> b2 >> c2 >> d2 >> e2 >> f2 >> g2 >> h2 >> i2 >> j2 >> k2 >> l2 >> m2 >> n2 >> o2 >> p2;
		LASS_TEST_CHECK(testI.good());
		LASS_TEST_CHECK_EQUAL(a2, a);
		LASS_TEST_CHECK_EQUAL(b2, b);
		LASS_TEST_CHECK_EQUAL(c2, c);
		LASS_TEST_CHECK_EQUAL(d2, d);
		LASS_TEST_CHECK_EQUAL(e2, e);
		LASS_TEST_CHECK_EQUAL(f2, f);
		LASS_TEST_CHECK_EQUAL(g2, g);
		LASS_TEST_CHECK_EQUAL(h2, h);
		LASS_TEST_CHECK_EQUAL(i2, i);
		LASS_TEST_CHECK_EQUAL(j2, j);
		LASS_TEST_CHECK_EQUAL(k2, k);
		LASS_TEST_CHECK_EQUAL(l2, l);
		LASS_TEST_CHECK_EQUAL(m2, m);
		LASS_TEST_CHECK_EQUAL(n2, n);
		LASS_TEST_CHECK_EQUAL(o2, o);
		LASS_TEST_CHECK_EQUAL(p2, p);
#if LASS_HAVE_WCHAR_SUPPORT
		std::wstring v2;
		std::wstring w2;
		testI >> v2 >> w2;
		LASS_TEST_CHECK(testI.good());
		LASS_TEST_CHECK(v2 == v);
		LASS_TEST_CHECK(w2 == w);
#endif
	}
}


namespace
{

typedef num::Tuint32 TData;

constexpr size_t N = 1000;

std::pair<void*, size_t> memoryBlock()
{
	static std::vector<TData> data;
	if (data.empty())
	{
		for (TData i = 0; i < N; ++i)
		{
			data.push_back(i);
		}
	}
	void* begin = &data[0];
	const size_t size = N * sizeof(TData);
	return std::make_pair(begin, size);
}



void testIoBinaryIStreamHelper(lass::io::BinaryIStream& stream, [[maybe_unused]] lass::io::BinaryIStream::pos_type max_pos)
{
	using pos_type = lass::io::BinaryIStream::pos_type;
	using off_type = lass::io::BinaryIStream::off_type;

	// read some values from start
	for (TData i = 0; i < 5; ++i)
	{
		TData i2;
		stream >> i2;
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK(i2 == i);
	}
	LASS_TEST_CHECK_EQUAL(stream.tellg(), 5 * sizeof(TData));

	// skip ahead some values
	stream.seekg(5 * sizeof(TData), std::ios_base::cur);
	LASS_TEST_CHECK_EQUAL(stream.tellg(), 10 * sizeof(TData));
	for (TData i = 10; i < 15; ++i)
	{
		TData i2;
		stream >> i2;
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK(i2 == i);
	}
	LASS_TEST_CHECK_EQUAL(stream.tellg(), 15 * sizeof(TData));

	// skip back some values
	stream.seekg(-10 * static_cast<off_type>(sizeof(TData)), std::ios_base::cur);
	LASS_TEST_CHECK_EQUAL(stream.tellg(), 5 * sizeof(TData));
	for (TData i = 5; i < 10; ++i)
	{
		TData i2;
		stream >> i2;
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK(i2 == i);
	}
	LASS_TEST_CHECK_EQUAL(stream.tellg(), 10 * sizeof(TData));

	// seek from start
	stream.seekg(20 * sizeof(TData), std::ios_base::beg);
	LASS_TEST_CHECK_EQUAL(stream.tellg(), 20 * sizeof(TData));
	for (TData i = 20; i < 25; ++i)
	{
		TData i2;
		stream >> i2;
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK(i2 == i);
	}
	LASS_TEST_CHECK_EQUAL(stream.tellg(), 25 * sizeof(TData));

	// seek from end
	stream.seekg(-5 * static_cast<off_type>(sizeof(TData)), std::ios_base::end);
	LASS_TEST_CHECK_EQUAL(stream.tellg(), (N - 5) * sizeof(TData));
	for (TData i = N - 5; i < N; ++i)
	{
		TData i2;
		stream >> i2;
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK(i2 == i);
	}

	// read past end
	{
		LASS_TEST_CHECK_EQUAL(stream.tellg(), N * sizeof(TData));
		TData i2 = 666;
		stream >> i2;
		LASS_TEST_CHECK(!stream.good());
		LASS_TEST_CHECK(stream.eof());
		LASS_TEST_CHECK_EQUAL(i2, static_cast<TData>(666));

		// fix by seeking back to start
		stream.seekg(0);
		stream >> i2;
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK_EQUAL(i2, static_cast<TData>(0));
	}

	// seek past end
	{
		stream.seekg(0);
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK_EQUAL(stream.tellg(), static_cast<pos_type>(0));
		stream.seekg((N + 5) * sizeof(TData));
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK_EQUAL(stream.tellg(), (N + 5) * sizeof(TData));
		TData i2 = 666;
		stream >> i2;
		LASS_TEST_CHECK(!stream.good());
		LASS_TEST_CHECK(stream.eof());
		LASS_TEST_CHECK_EQUAL(i2, static_cast<TData>(666));
	}

	// seek past end, from beg
	{
		stream.seekg(0);
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK_EQUAL(stream.tellg(), static_cast<pos_type>(0));
		stream.seekg((N + 5) * sizeof(TData), std::ios_base::beg);
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK_EQUAL(stream.tellg(), (N + 5) * sizeof(TData));
		TData i2 = 666;
		stream >> i2;
		LASS_TEST_CHECK(!stream.good());
		LASS_TEST_CHECK(stream.eof());
		LASS_TEST_CHECK_EQUAL(i2, static_cast<TData>(666));
	}

	// seek past end, from cur
	{
		stream.seekg(5 * sizeof(TData));
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK_EQUAL(stream.tellg(), 5 * sizeof(TData));
		stream.seekg(N * sizeof(TData), std::ios_base::cur);
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK_EQUAL(stream.tellg(), (N + 5) * sizeof(TData));
		TData i2 = 666;
		stream >> i2;
		LASS_TEST_CHECK(!stream.good());
		LASS_TEST_CHECK(stream.eof());
		LASS_TEST_CHECK_EQUAL(i2, static_cast<TData>(666));
	}

	// seek past end, from end
	{
		stream.seekg(0 );
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK_EQUAL(stream.tellg(), static_cast<pos_type>(0));
		stream.seekg(5 * sizeof(TData), std::ios_base::end);
		LASS_TEST_CHECK(stream.good());
		LASS_TEST_CHECK_EQUAL(stream.tellg(), (N + 5) * sizeof(TData));
		TData i2 = 666;
		stream >> i2;
		LASS_TEST_CHECK(!stream.good());
		LASS_TEST_CHECK(stream.eof());
		LASS_TEST_CHECK_EQUAL(i2, static_cast<TData>(666));
	}

	// bad seek before start, from begin
	stream.seekg(0);
	LASS_TEST_CHECK(stream.good());
	stream.seekg(-5 * static_cast<off_type>(sizeof(TData)), std::ios_base::beg);
	LASS_TEST_CHECK(stream.fail());
	stream.clear();

	// bad seek before start, from cur
	stream.seekg(5 * sizeof(TData));
	LASS_TEST_CHECK(stream.good());
	stream.seekg(-10 * static_cast<off_type>(sizeof(TData)), std::ios_base::cur);
	LASS_TEST_CHECK(stream.fail());
	stream.clear();

	// bad seek before start, from end
	stream.seekg(0);
	LASS_TEST_CHECK(stream.good());
	stream.seekg(-static_cast<off_type>(N + 5) * static_cast<off_type>(sizeof(TData)), std::ios_base::end);
	LASS_TEST_CHECK(stream.fail());
	stream.clear();

	/* 
	This doesn't work consistently over different OSes:
	seekg(max_pos) will fail on Linux but succeed on Windows

	// bad seek beyond max, from cur
	stream.seekg(max_pos);
	LASS_TEST_CHECK(stream.good());
	LASS_TEST_CHECK_EQUAL(stream.tellg(), max_pos);
	stream.seekg(1, std::ios_base::cur);
	LASS_TEST_CHECK(stream.fail());
	stream.clear();
	*/
}


}


void testIoBinaryIFile()
{
	{
		auto memory = memoryBlock();
		FILE* fp = fopen("temp.bin", "wb");
		LASS_TEST_CHECK_EQUAL(fwrite(memory.first, 1, memory.second, fp), memory.second);
		LASS_TEST_CHECK_EQUAL(fclose(fp), 0);
	}

	lass::io::BinaryIFile stream("temp.bin");
	testIoBinaryIStreamHelper(stream, num::NumTraits<lass::io::BinaryIStream::off_type>::max);

	/*
	This doesn't work consistently in all build targets

	// bad seek beyond max, from end. only fails for BinaryIFile
	stream.seekg(0);
	LASS_TEST_CHECK(stream.good());
	LASS_TEST_CHECK_EQUAL(stream.tellg(), static_cast<lass::io::BinaryIStream::pos_type>(0));
	//stream.seekg(num::NumTraits<lass::io::BinaryIStream::off_type>::max, std::ios_base::end);
	LASS_TEST_CHECK(stream.fail());
	stream.clear();
	*/
}


void testIoBinaryIMemoryMap()
{
	{
		auto memory = memoryBlock();
		FILE* fp = fopen("temp.bin", "wb");
		LASS_TEST_CHECK_EQUAL(fwrite(memory.first, 1, memory.second, fp), memory.second);
		LASS_TEST_CHECK_EQUAL(fclose(fp), 0);
	}

	lass::io::BinaryIMemoryMap stream("temp.bin");
	testIoBinaryIStreamHelper(stream, num::NumTraits<lass::io::BinaryIStream::pos_type>::max);
}


void testIoBinaryIMemoryBlock()
{
	auto memory = memoryBlock();
	lass::io::BinaryIMemoryBlock stream(memory.first, memory.second);
	testIoBinaryIStreamHelper(stream, num::NumTraits<lass::io::BinaryIStream::pos_type>::max);
}


TUnitTest test_io_binary_stream()
{
	return TUnitTest{
		LASS_TEST_CASE(testIoStreamBase),
		LASS_TEST_CASE(testIoBinaryStream),
		LASS_TEST_CASE(testIoBinaryIFile),
		LASS_TEST_CASE(testIoBinaryIMemoryMap),
		LASS_TEST_CASE(testIoBinaryIMemoryBlock),
	};
}

}

}

// EOF
