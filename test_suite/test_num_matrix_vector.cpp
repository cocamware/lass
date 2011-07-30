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
 *	Copyright (C) 2004-2010 the Initial Developer.
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

#include "../lass/num/vector.h"
#include "../lass/num/matrix.h"
#include "../lass/num/random.h"
#include "../lass/num/distribution.h"

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
	Generator(): random_(), distribution_(random_, -1, 1) {}
	T operator()() { return distribution_(); }
};

template <typename T>
struct Generator<std::complex<T> >
{
	num::RandomMT19937 random_;
	num::DistributionUniform<T, num::RandomMT19937> distribution_;
	Generator(): random_(), distribution_(random_, -1, 1) {}
	std::complex<T> operator()() { return std::complex<T>(distribution_(), distribution_()); }
};

template <typename T>
void fill(num::Matrix<T>& matrix, Generator<T>& rng)
{
	typedef typename num::Matrix<T>::TSize TSize;
	for(TSize i = 0, rows = matrix.rows(); i < rows; ++i)
	{
		for(TSize j = 0, cols = matrix.columns(); j < cols; ++j)
		{
			matrix(i, j) = rng();
		}
	}
}

}

template <typename T>
void testNumVector()
{
	using namespace num;

	typedef Vector<T> TVector;
	typedef typename TVector::TNumTraits TNumTraits;
	typedef typename TVector::TValue TValue;
	typedef typename TVector::TSize TSize;
	typedef typename TNumTraits::baseType TBase;

	const TBase tolerance = 1e-5f;
/*

	typedef Vector<T> TVector;
	TVector a;
	LASS_TEST_CHECK(a.isEmpty());
	LASS_TEST_CHECK(a.isZero());
	LASS_TEST_CHECK_EQUAL(a, a);
	LASS_TEST_CHECK_EQUAL(a.squaredNorm(), NumTraits<T>::zero);

	TVector b(3);
	LASS_TEST_CHECK(!b.isEmpty());
	LASS_TEST_CHECK(b.isZero());
	LASS_TEST_CHECK_EQUAL(b.size(), 3);
	LASS_TEST_CHECK_EQUAL(b, b);
	LASS_TEST_CHECK_EQUAL(b.squaredNorm(), NumTraits<T>::zero);

	b[0] = -35; b[1] = 65; b[2] = 167;
	LASS_TEST_CHECK_EQUAL(b.at(3), b[0]);
	LASS_TEST_CHECK_EQUAL(b.at(-1), b[2]);
	LASS_TEST_CHECK_EQUAL(b.squaredNorm(), T(35 * 35 + 65 * 65 + 167 * 167));

	TVector c = T(2) * b;
	LASS_TEST_CHECK_EQUAL(c.size(), 3);
	LASS_TEST_CHECK_EQUAL(b, c / T(2));
	LASS_TEST_CHECK_EQUAL(c.squaredNorm(), T(4) * b.squaredNorm());

	a = b - c;
	LASS_TEST_CHECK_EQUAL(a.size(), 3);
	LASS_TEST_CHECK_EQUAL(b, a + c);
	LASS_TEST_CHECK_EQUAL(a.squaredNorm(), b.squaredNorm());
	LASS_TEST_CHECK_EQUAL(a, -b);
*/
	// let's go a bit wild :)
	//
	const TSize n = 100;
	TSize i;
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
	LASS_TEST_CHECK_EQUAL(a.size(), n);
	LASS_TEST_CHECK(!a.isEmpty());
	for (i = 0; i < n; ++i)
	{
		a[i] = aRef[i];
		LASS_TEST_CHECK_EQUAL(a[i], aRef[i]);
	}

	// construct some vector from their representation
	//
	Vector<T> b(bRef);
	Vector<T> c(cRef);
	LASS_TEST_CHECK_EQUAL(b.size(), n);
	LASS_TEST_CHECK_EQUAL(c.size(), n);
	LASS_TEST_CHECK(!b.isEmpty());
	LASS_TEST_CHECK(!c.isEmpty());

	// final check
	//
	for (i = 0; i < n; ++i)
	{
		LASS_TEST_CHECK_EQUAL(a[i], aRef[i]);
		LASS_TEST_CHECK_EQUAL(b[i], bRef[i]);
		LASS_TEST_CHECK_EQUAL(c[i], cRef[i]);
	}

	// construct empty vector
	//
	Vector<T> d;
	LASS_TEST_CHECK_EQUAL(d.size(), size_t(0));
	LASS_TEST_CHECK(d.isEmpty());

	d = a;
	LASS_TEST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) LASS_TEST_CHECK_EQUAL(d[i], a[i]);

	d = +b;
	LASS_TEST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) LASS_TEST_CHECK_EQUAL(d[i], b[i]);

	d = -c;
	LASS_TEST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) LASS_TEST_CHECK_EQUAL(d[i], -c[i]);

	d += a;
	LASS_TEST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) LASS_TEST_CHECK_EQUAL(d[i], a[i] - c[i]);

	d -= b;
	LASS_TEST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) LASS_TEST_CHECK_CLOSE(d[i], a[i] - c[i] - b[i], tolerance);

	d = a;
	d *= b;
	LASS_TEST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) LASS_TEST_CHECK_EQUAL(d[i], a[i] * b[i]);

	d /= c;
	LASS_TEST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) LASS_TEST_CHECK_CLOSE(d[i], (a[i] * b[i]) / c[i], tolerance);

	d = a;
	d += T(1);
	LASS_TEST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) LASS_TEST_CHECK_EQUAL(d[i], (a[i] + T(1)));

	d -= T(2);
	LASS_TEST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) LASS_TEST_CHECK_CLOSE(d[i] , (a[i] + T(1)) - T(2), tolerance);

	d = b;
	d *= T(2);
	LASS_TEST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) LASS_TEST_CHECK_EQUAL(d[i], T(2) * b[i]);

	d = b;
	d /= T(4);
	LASS_TEST_CHECK_EQUAL(d.size(), n);
	for (i = 0; i < n; ++i) LASS_TEST_CHECK_EQUAL(d[i], b[i] / T(4));

	LASS_TEST_CHECK_CLOSE(a.sum(), std::accumulate(aRef.begin(), aRef.end(), T(0)), tolerance);
	//LASS_TEST_CHECK_EQUAL(a.min(), *std::min_element(aRef.begin(), aRef.end()));
	//LASS_TEST_CHECK_EQUAL(a.max(), *std::max_element(aRef.begin(), aRef.end()));
	LASS_TEST_CHECK_CLOSE(a.squaredNorm(), (a * a).sum(), tolerance);
	LASS_TEST_CHECK_CLOSE(a.norm(), num::sqrt(a.squaredNorm()), tolerance);
}



template <typename T>
void testNumMatrix()
{
	using namespace num;

	typedef Matrix<T> TMatrix;
	typedef typename TMatrix::TSize TSize;

	TMatrix a;
	LASS_TEST_CHECK(a.isEmpty());
	LASS_TEST_CHECK(a.isZero());
	LASS_TEST_CHECK(a.isIdentity());
	LASS_TEST_CHECK(a.isSquare());
	LASS_TEST_CHECK(a.isDiagonal());
	LASS_TEST_CHECK_EQUAL(a, a);

	a.setIdentity(3);
	LASS_TEST_CHECK(!a.isEmpty());
	LASS_TEST_CHECK(!a.isZero());
	LASS_TEST_CHECK(a.isIdentity());
	LASS_TEST_CHECK(a.isSquare());
	LASS_TEST_CHECK(a.isDiagonal());
	LASS_TEST_CHECK_EQUAL(a, a);

	TMatrix b(3, 4);
	LASS_TEST_CHECK(!b.isEmpty());
	LASS_TEST_CHECK(b.isZero());
	LASS_TEST_CHECK(!b.isIdentity());
	LASS_TEST_CHECK(!b.isSquare());
	LASS_TEST_CHECK(!b.isDiagonal());
	LASS_TEST_CHECK_EQUAL(b.rows(), size_t(3));

	LASS_TEST_CHECK_EQUAL(b.columns(), size_t(4));
	LASS_TEST_CHECK_EQUAL(b, b);

	LASS_TEST_CHECK_THROW(a + b, util::EnforceFailure);

	num_vector::Generator<T> rng;
	num_vector::fill(b, rng);
	LASS_TEST_CHECK(!b.isZero());
	LASS_TEST_CHECK_EQUAL(b, b);

	a.setZero(3, 4);
	LASS_TEST_CHECK(!a.isEmpty());
	LASS_TEST_CHECK(a.isZero());
	LASS_TEST_CHECK_EQUAL(a + b, b);

	num_vector::fill(a, rng);
	TMatrix c = a + b;
	LASS_TEST_CHECK(c != b);
	b += a;
	LASS_TEST_CHECK_EQUAL(b, c);

	b = c - a;
	LASS_TEST_CHECK(c != b);
	c -= a;
	LASS_TEST_CHECK_EQUAL(b, c);

	c = b * 2;
	LASS_TEST_CHECK(c != b);
	LASS_TEST_CHECK_EQUAL(b + b, c);
	b *= 2;
	LASS_TEST_CHECK_EQUAL(b, c);

	b /= 2;
	LASS_TEST_CHECK(c != b);
	LASS_TEST_CHECK_EQUAL(b, c / 2);
}



template <typename T>
void testNumSolve()
{
	using namespace num;

	const int n = 3;
	Matrix<T> a(n, n);
	Vector<T> b(n);

	int v = 0;
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			a(i, j) = static_cast<T>(++v);
		}
		b[i] = static_cast<T>(i);
	}
	a(0, 0) = static_cast<T>(-1);
	b[0] = static_cast<T>(0);

	Vector<T> x = b;
	solve(a, x);
	LASS_TEST_CHECK(num::norm((a * x - b).norm()) < 1e-12f);
}


template <typename T>
void testNumMatrixSolve()
{
	using namespace num;
	
	const int n = 200;
	Matrix<T> a(n, n);
	Matrix<T> b(n, n);

	num_vector::Generator<T> rng;
	num_vector::fill(a, rng);
	num_vector::fill(b, rng);

	Matrix<T> x = b;
	solve(a, x);

	typedef typename NumTraits<T>::baseType TBase;
	typedef std::vector<TBase> TErrors;
	TErrors errors;
	Matrix<T> e = a * x - b;
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			errors.push_back( num::abs( e(i, j) ) );
		}
	}

	typename TErrors::iterator min = errors.begin();
	typename TErrors::iterator max = errors.end() - 1;
	typename TErrors::iterator median = errors.begin() + (errors.size() / 2);
	std::nth_element(errors.begin(), median, errors.end());
	std::nth_element(errors.begin(), min, median);
	std::nth_element(median, max, errors.end());

	typename NumTraits<T>::baseType avg = 0, rms = 0;
	for (typename TErrors::iterator i = errors.begin(), last = errors.end(); i != last; ++i)
	{
		avg += *i;
		rms += num::sqr(*i);
	}
	avg /= errors.size();
	rms = num::sqrt(rms / errors.size());
	
	std::cout << "min=" << *min << ", median=" << *median << ", avg=" << avg << ", rms=" << rms << ", max=" << *max << std::endl;

	LASS_TEST_CHECK(*max < 1000 * NumTraits<TBase>::epsilon );
}

TUnitTest test_num_matrix_vector()
{
	TUnitTest result;
	result.push_back(LASS_TEST_CASE(testNumMatrix<float>));
	result.push_back(LASS_TEST_CASE(testNumMatrix< std::complex<float> >));
	result.push_back(LASS_TEST_CASE(testNumMatrix<double>));
	result.push_back(LASS_TEST_CASE(testNumMatrix< std::complex<double> >));
	result.push_back(LASS_TEST_CASE(testNumVector<float>));
	result.push_back(LASS_TEST_CASE(testNumVector< std::complex<float> >));
	result.push_back(LASS_TEST_CASE(testNumVector<double>));
	result.push_back(LASS_TEST_CASE(testNumVector< std::complex<double> >));
	result.push_back(LASS_TEST_CASE(testNumSolve<double>));
	result.push_back(LASS_TEST_CASE(testNumSolve< std::complex<double> >));

	result.push_back(LASS_TEST_CASE(testNumMatrixSolve<float>));
	result.push_back(LASS_TEST_CASE(testNumMatrixSolve<double>));
	result.push_back(LASS_TEST_CASE(testNumMatrixSolve< std::complex<float> >));
	result.push_back(LASS_TEST_CASE(testNumMatrixSolve< std::complex<double> >));
	return result;
}



}

}

// EOF
