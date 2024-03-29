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
 *	Copyright (C) 2004-2011 the Initial Developer.
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



/** @class lass::num::Matrix
 *  @brief a dynamic sized n-dimensional matrix with expression templates
 *  @author Bram de Greve [BdG]
 */

#ifndef LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_H
#define LASS_GUARDIAN_OF_INCLUSION_NUM_MATRIX_H

#include "num_common.h"
#include "num_traits.h"
#include "impl/matrix_expressions.h"
#include "../util/call_traits.h"
#include "../stde/index_iterator.h"

namespace lass
{
namespace num
{

template
<
	typename T,
	typename S = impl::MStorage<T>
>
class Matrix
{
public:

	typedef Matrix<T, S> TSelf;
	typedef S TStorage;
	typedef typename TStorage::TSize TSize;
	typedef typename num::NumTraits<TSize>::signedType TSigned;

	typedef typename util::CallTraits<T>::TValue TValue;
	typedef typename util::CallTraits<T>::TParam TParam;
	typedef typename util::CallTraits<T>::TReference TReference;
	typedef typename util::CallTraits<T>::TConstReference TConstReference;
	typedef num::NumTraits<T> TNumTraits;
	typedef TValue* TPointer;

	template <typename U> struct Rebind
	{
		typedef Matrix<U, S> Type;
	};

	class Row
	{
	public:
		typedef stde::index_iterator_t<Row, T, T*, T&, TSize, TSigned> iterator;
		T& operator[](TSize column) { return matrix_(row_, column); }
		const T& operator[](TSize column) const { return matrix_(row_, column); }
		T& at(TSigned column) { LASS_ASSERT(static_cast<TSigned>(row_) >= 0); return matrix_.at(static_cast<TSigned>(row_), column); }
		const T& at(TSigned column) const { LASS_ASSERT(static_cast<TSigned>(row_) >= 0); return matrix_.at(static_cast<TSigned>(row_), column); }
		const TSize size() const { return matrix_.columns(); }
		iterator begin() { return iterator(*this, 0); }
		iterator end() { return iterator(*this, size()); }
	private:
		friend class Matrix<T, S>;
		Row(Matrix<T, S>& matrix, TSize row): matrix_(matrix), row_(row) {}
		Matrix<T, S>& matrix_;
		TSize row_;
	};

	class ConstRow
	{
	public:
		ConstRow(const Row& other): matrix_(other.matrix_), row_(other.row_) {}
		const T& operator[](TSize column) const { return matrix_(row_, column); }
		const T& at(TSigned column) const { LASS_ASSERT(static_cast<TSigned>(row_) >= 0); return matrix_.at(static_cast<TSigned>(row_), column); }
		const TSize size() const { return matrix_.columns(); }
	private:
		friend class Matrix<T, S>;
		ConstRow(const Matrix<T, S>& matrix, TSize row): matrix_(matrix), row_(row) {}
		const Matrix<T, S>& matrix_;
		TSize row_;
	};

	class Column
	{
	public:
		typedef stde::index_iterator_t<Column, T, T*, T&, TSize, TSigned> iterator;
		T& operator[](TSize row) { return matrix_(row, column_); }
		const T& operator[](TSize row) const { return matrix_(row, column_); }
		T& at(TSigned row) { LASS_ASSERT(static_cast<TSigned>(column_) >= 0); return matrix_.at(row, static_cast<TSigned>(column_)); }
		const T& at(TSigned row) const { LASS_ASSERT(static_cast<TSigned>(column_) >= 0); return matrix_.at(row, static_cast<TSigned>(column_)); }
		const TSize size() const { return matrix_.rows(); }
		iterator begin() { return iterator(*this, 0); }
		iterator end() { return iterator(*this, matrix_.columns()); }
	private:
		friend class Matrix<T, S>;
		Column(Matrix<T, S>& matrix, TSize column): matrix_(matrix), column_(column) {}
		Matrix<T, S>& matrix_;
		TSize column_;
	};

	class ConstColumn
	{
	public:
		ConstColumn(const Column& other): matrix_(other.matrix_), column_(other.column_) {}
		const T& operator[](TSize row) const { return matrix_(row, column_); }
		const T& at(TSigned row) const { LASS_ASSERT(static_cast<TSigned>(column_) >= 0); return matrix_.at(row, static_cast<TSigned>(column_)); }
		const TSize size() const { return matrix_.rows(); }
	private:
		friend class Matrix<T, S>;
		ConstColumn(const Matrix<T, S>& matrix, TSize column): matrix_(matrix), column_(column) {}
		const Matrix<T, S>& matrix_;
		TSize column_;
	};

	Matrix();
	explicit Matrix(TSize rows, TSize columns);
	explicit Matrix(const TStorage& storage);
	template <typename T2, typename S2> Matrix(const Matrix<T2, S2>& other);

	template <typename T2, typename S2> Matrix<T, S>& operator=(const Matrix<T2, S2>& other);

	const TSize rows() const;
	const TSize columns() const;

	const TValue operator()(TSize row, TSize column) const;
	TReference operator()(TSize row, TSize column);
	const TValue at(TSigned row, TSigned column) const;
	TReference at(TSigned row, TSigned column);

	ConstRow row(TSigned index) const;
	Row row(TSigned index);
	ConstColumn column(TSigned index) const;
	Column column(TSigned index);

	const Matrix<T, S>& operator+() const;
	const Matrix<T, impl::MNeg<T, S> > operator-() const;

	template <typename S2> Matrix<T, S>& operator+=(const Matrix<T, S2>& other);
	template <typename S2> Matrix<T, S>& operator-=(const Matrix<T, S2>& other);
	Matrix<T, S>& operator*=(TParam scalar);
	Matrix<T, S>& operator/=(TParam scalar);

	void setZero(TSize rows, TSize columns);
	void setIdentity(TSize size);

	bool isEmpty() const;
	bool isZero() const;
	bool isIdentity() const;
	bool isDiagonal() const;
	bool isSquare() const;

	const Matrix<T, impl::MTrans<T, S> > transposed() const;
	void invert();

	const TStorage& storage() const;
	TStorage& storage();
	void swap(Matrix<T, S>& other);

private:

	TStorage storage_;
};

template <typename T, typename S1, typename S2>
bool operator==(const Matrix<T, S1>& a, const Matrix<T, S2>& b);
template <typename T, typename S1, typename S2>
inline bool operator!=(const Matrix<T, S1>& a, const Matrix<T, S2>& b);

template <typename T, typename S1, typename S2>
const Matrix<T, impl::MAdd<T, S1, S2> > operator+(const Matrix<T, S1>& a, const Matrix<T, S2>& b);
template <typename T, typename S1, typename S2>
const Matrix<T, impl::MSub<T, S1, S2> > operator-(const Matrix<T, S1>& a, const Matrix<T, S2>& b);
template <typename T, typename S1, typename S2>
const Matrix<T, impl::MProd<T, S1, S2> > operator*(const Matrix<T, S1>& a, const Matrix<T, S2>& b);

template <typename T, typename S>
const Matrix<T, impl::MAdd<T, impl::MScalar<T>, S> > operator+(const T& a, const Matrix<T, S>& b);
template <typename T, typename S>
const Matrix<T, impl::MSub<T, impl::MScalar<T>, S> > operator-(const T& a, const Matrix<T, S>& b);
template <typename T, typename S>
const Matrix<T, impl::MMul<T, impl::MScalar<T>, S> > operator*(const T& a, const Matrix<T, S>& b);

template <typename T, typename S>
const Matrix<T, impl::MAdd<T, S, impl::MScalar<T> > > operator+(const Matrix<T, S>& a, typename Matrix<T, S>::TParam b);
template <typename T, typename S>
const Matrix<T, impl::MAdd<T, S, impl::MScalar<T> > > operator-(const Matrix<T, S>& a, typename Matrix<T, S>::TParam b);
template <typename T, typename S>
const Matrix<T, impl::MMul<T, S, impl::MScalar<T> > > operator*(const Matrix<T, S>& a, typename Matrix<T, S>::TParam b);
template <typename T, typename S>
const Matrix<T, impl::MMul<T, S, impl::MScalar<T> > > operator/(const Matrix<T, S>& a, typename Matrix<T, S>::TParam b);

template <typename T, typename S, typename S2>
void solve(const Matrix<T, S>& a, Matrix<T, S2>& bx, bool improve = true);


template <typename T, typename S, typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& stream, const Matrix<T, S>& matrix);

}

}

#include "matrix.inl"

#ifdef LASS_GUARDIAN_OF_INCLUSION_NUM_VECTOR_H
#include "matrix_vector.h"
#endif

#endif

// EOF
