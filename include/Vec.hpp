/*
 * Vec.hpp
 *
 *  Created on: 09.08.2016
 *      Author: hartung
 */

#ifndef VEC_HPP_
#define VEC_HPP_

#include <immintrin.h>

class Vec {
public:
	Vec() :
			_m(_mm256_set1_pd(0)) {

	}

	Vec(const Vec & in) :
			_m(in._m) {

	}

	Vec & operator^=(const Vec & in) {
		return andOp(in);
	}

	Vec & operator|=(const Vec & in) {
		return orOp(in);
	}

	Vec operator-() const
	{
		Vec res(*this);
		return res.notOp();
	}

	Vec operator^(const Vec & in) const
	{
		Vec res(*this);
		return res.andOp(in);
	}

	Vec operator|(const Vec & in) const
	{
		Vec res(*this);
		return res.orOp(in);
	}

	Vec & andOp(const Vec& in) {
		_m = _mm256_and_pd(_m, in._m);
		return *this;
	}

	Vec & orOp(const Vec& in) {
		_m = _mm256_or_pd(_m, in._m);
		return *this;
	}

	Vec & andNotOp(const Vec& in) {
		_m = _mm256_andnot_pd(_m, in._m);
		return *this;
	}

	Vec & notOp() {
		_m = _mm256_xor_pd(one(), _m);
		return *this;
	}

	Vec & orNotOp(const Vec& in) {

		return orOp(in).notOp();
	}

	static __m256d zero() {
		return _mm256_set_pd(0, 0, 0, 0); //TODO
	}
	static __m256d one() {
		return _mm256_set_pd(1, 1, 1, 1); //TODO
	}

	static size_t size()
	{
		return 256u;
	}

private:
	__m256d _m;
};

#endif /* VEC_HPP_ */
