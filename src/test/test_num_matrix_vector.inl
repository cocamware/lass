/** @file
 *  @internal
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



#ifndef LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_MATRIX_VECTOR_INL
#define LASS_GUARDIAN_OF_INCLUSION_TEST_TEST_NUM_MATRIX_VECTOR_INL

#include "test_common.h"

#include "../num/vector.h"
#include "../num/matrix.h"
#include "../num/random.h"
#include "../num/distribution.h"

namespace lass
{
namespace test
{
namespace num_vector
{

template <typename T>
struct Generator
{
	num::RandomMT19937 random_;
	num::DistributionUniform<T, num::RandomMT19937> distribution_;
	Generator(): random_(), distribution_(random_) {}
	T operator()() { return distribution_(); }
};

template <typename T>
struct Generator<std::complex<T> >
{
	num::RandomMT19937 random_;
	num::DistributionUniform<T, num::RandomMT19937> distribution_;
	Generator(): random_(), distribution_(random_) {}
	std::complex<T> operator()() { return std::complex<T>(distribution_(), distribution_()); }
};

}

template <typename T>
void testNumVector()
{
	using namespace num;
/*

	typedef Vector<T> TVector;
	TVector a;
	BOOST_CHECK(a.isEmpty());
	BOOST_CHECK(a.isZero());
	BOOST_CHECK_EQUAL(a, a);
	BOOST_CHECK_EQUAL(a.squaredNorm(), NumTraits<T>::zero);

	TVector b(3);
	BOOST_CHECK(!b.isEmpty());
	BOOST_CHECK(b.isZero());
	BOOST_CHECK_EQUAL(b.size(), 3);
	BOOST_CHECK_EQUAL(b, b);
	BOOST_CHECK_EQUAL(b.squaredNorm(), NumTraits<T>::zero);

	b[0] = -35; b[1] = 65; b[2] = 167;
	BOOST_CHECK_EQUAL(b.at(3), b[0]);
	BOOST_CHECK_EQUAL(b.at(-1), b[2]);
	BOOST_CHECK_EQUAL(b.squaredNorm(), T(35 * 35 + 65 * 65 + 167 * 167));

	TVector c = T(2) * b;
	BOOST_CHECK_EQUAL(c.size(), 3);
	BOOST_CHECK_EQUAL(b, c / T(2));
	BOOST_CHECK_EQUAL(c.squaredNorm(), T(4) * b.squaredNorm());

	a = b - c;
	BOOST_CHECK_EQUAL(a.size(), 3);
	BOOST_CHECK_EQUAL(b, a + c);
	BOOST_CHECK_EQUAL(a.squaredNorm(), b.squaredNorm());
	BOOST_CHECK_EQUAL(a, -b);
*/
	// let's go a bit wild :)
	//
	unsigned n = 100;
	unsigned i;
	std::vector<T> aRef(n);
	std::vector<T> bRef(n);
	std::vector<T> cRef(n);
	num_vector::Generator<T> generator;
	for (i = 0; i < n; ++i)
	{
		aRef[i] = generator();
		bRef[i] = generator();
		cRef[i] = generator();
	}

	// construct a vector with 1000 elements
	//
	Vector<T> a(n);
	BOOST_CHECK_EQUAL(a.size(), n);
	BOOST_CHECK(!a.isEmpty());
	for (i = 0; i < n; ++i)
	{
		a[i] = aRef[i];
		BOOST_CHECK_EQUAL(a[i], aRef[i]);
	}

	// construct some vector from their representation
	//
	Vector<T> b(bRef);
	Vector<T> c(cRef);
	BOOST_CHECK_EQUAL(b.size(), n);
	BOOST_CHECK_EQUAL(c.size(), n);
	BOOST_CHECK(!b.isEmpty());
	BOOST_CHECK(!c.isEmpty());

	// final check
	//
	for (i = 0; i < n; ++i)
	{
		BOOST_CHECK_EQUAL(a[i], aRef[i]);
		BOOST_CHECK_EQUAL(b[i], bRef[i]);
		BOOST_CHECK_EQUAL(c[i], cRef[i]);
	}

	// construct empty vector
	//
	Vector<T> d;
	BOOST_CHECK_EQUAL(d.size(), 0);
	BOOST_CHECK(d.isEmpty());

	d = a;
	BOOST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) BOOST_CHECK_EQUAL(d[i], a[i]);

	d = +b;
	BOOST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) BOOST_CHECK_EQUAL(d[i], b[i]);

	d = -c;
	BOOST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) BOOST_CHECK_EQUAL(d[i], -c[i]);

	d += a;
	BOOST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) BOOST_CHECK_EQUAL(d[i], a[i] - c[i]);

	d -= b;
	BOOST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) BOOST_CHECK_EQUAL(d[i], a[i] - c[i] - b[i]);

	d = a;
	d *= b;
	BOOST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) BOOST_CHECK_EQUAL(d[i], a[i] * b[i]);

	d /= c;
	BOOST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) BOOST_CHECK_EQUAL(d[i], (a[i] * b[i]) / c[i]);

	d = a;
	d += T(1);
	BOOST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) BOOST_CHECK_EQUAL(d[i], (a[i] + T(1)));

	d -= T(2);
	BOOST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) BOOST_CHECK(num::sqrt(num::norm(d[i] - ((a[i] + T(1)) - T(2)))) < 1e-7);

	d = b;
	d *= T(2);
	BOOST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) BOOST_CHECK_EQUAL(d[i], T(2) * b[i]);

	d = b;
	d /= T(4);
	BOOST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) BOOST_CHECK_EQUAL(d[i], b[i] / T(4));

	BOOST_CHECK_EQUAL(a.sum(), std::accumulate(aRef.begin(), aRef.end(), T(0)));
	//BOOST_CHECK_EQUAL(a.min(), *std::min_element(aRef.begin(), aRef.end()));
	//BOOST_CHECK_EQUAL(a.max(), *std::max_element(aRef.begin(), aRef.end()));
	BOOST_CHECK_EQUAL(a.squaredNorm(), (a * a).sum());
	BOOST_CHECK_EQUAL(a.norm(), num::sqrt(a.squaredNorm()));
}



template <typename T>
void testNumMatrix()
{
	using namespace num;

	typedef Matrix<T> TMatrix;

	TMatrix a;
	BOOST_CHECK(a.isEmpty());
	BOOST_CHECK(a.isZero());
	BOOST_CHECK(a.isIdentity());
	BOOST_CHECK(a.isSquare());
	BOOST_CHECK(a.isDiagonal());
	BOOST_CHECK_EQUAL(a, a);

	TMatrix b(3, 4);
	BOOST_CHECK(!b.isEmpty());
	BOOST_CHECK(b.isZero());
	BOOST_CHECK(!b.isIdentity());
	BOOST_CHECK(!b.isSquare());
	BOOST_CHECK(!b.isDiagonal());
	BOOST_CHECK_EQUAL(b.rows(), 3);
	BOOST_CHECK_EQUAL(b.columns(), 4);
	BOOST_CHECK_EQUAL(b, b);
}



template <typename T>
void testNumSolve()
{
	using namespace num;

	typedef typename Matrix<T>::TSize TSize;

	const TSize n = 3;
	Matrix<T> a(n, n);
	Vector<T> b(n);

	int v = 0;
	for (TSize i = 0; i < n; ++i)
	{
		for (TSize j = 0; j < n; ++j)
		{
			a(i, j) = static_cast<T>(++v);
		}
		b[i] = static_cast<T>(i);
	}
	a(0, 0) = static_cast<T>(-1);
	b[0] = static_cast<T>(0);

	Vector<T> x = b;
	solve(a, x);
	BOOST_CHECK(num::norm((a * x - b).norm()) < 1e-12);
}



}

}

#endif

// EOF
