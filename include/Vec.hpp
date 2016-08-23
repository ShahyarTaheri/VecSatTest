/*
 * Vec.hpp
 *
 *  Created on: 09.08.2016
 *      Author: hartung
 */

#ifndef VEC_HPP_
#define VEC_HPP_

#include <immintrin.h>
#include <limits>

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

	static __m256d slowZero() {
            __m256d res;
            uint64_t * p = reinterpret_cast<int64_t *>(&res);
            uint64_t nullv = 0;
            for(size_t i=0;i<sizeof(__m256d)/sizeof(uint64_t);++i)
                p[i] = nullv;
            return res;
	}
	static __m256d slowOne() {
            __m256d res;
            uint64_t * p = reinterpret_cast<int64_t *>(&res);
            uint64_t eins = std::numeric_limits<uint64_t>::max();
            for(size_t i=0;i<sizeof(__m256d)/sizeof(uint64_t);++i)
                p[i] = eins;
            return res;
	}
	
	static const __m256d & zero() {
            return zeroVec;
        }
	
	static const __m256d & one() {
            return oneVec;
        }

	static size_t size()
	{
            return 256u;
	}

private:
	__m256d _m;
        
        static const __m256d zeroVec;
        static const __m256d oneVec;
};

const __m256d Vec::zeroVec = Vec::slowZero();
const __m256d Vec::zeroVec = Vec::slowOne();


#endif /* VEC_HPP_ */
