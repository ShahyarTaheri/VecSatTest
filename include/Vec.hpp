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
	Vec() {
            _m = (__m256d*)_mm_malloc(32,32);
	}
	
	Vec(const __m256d & in)
            :   Vec()
        {
            *_m = in;
        }
        
        ~Vec()
        {
            if(_m != nullptr)
            {
                _mm_free(_m);
                _m = nullptr;
            }
        }
            

	Vec(const Vec & in) : Vec() {
            *_m = *in._m;
	}
	
	bool operator!=(const Vec & in) const
        {
            const uint64_t * p1= reinterpret_cast<const uint64_t*>(_m);
            const uint64_t * p2= reinterpret_cast<const uint64_t*>(in._m);
            for(size_t i=0;i<size()/64;++i)
                if(p1[i] != p2[i])
                    return true;
            return false;
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
		*_m = _mm256_and_pd(*_m, *in._m);
		return *this;
	}

	Vec & orOp(const Vec& in) {
		*_m = _mm256_or_pd(*_m, *in._m);
		return *this;
	}

	Vec & andNotOp(const Vec& in) {
		*_m = _mm256_andnot_pd(*_m, *in._m);
		return *this;
	}

	Vec & notOp() {
		*_m = _mm256_xor_pd(*one()._m, *_m);
		return *this;
	}

	Vec & orNotOp(const Vec& in) {

		return orOp(in).notOp();
	}
	
	void set(const size_t & index, const bool & val)
        {
            setSlow(index,val);
        }
	
	void setSlow(const size_t & index, const bool & val)
        {
            uint64_t tval = val;
            uint64_t * p = reinterpret_cast<uint64_t*>(&_m);
            size_t posArr = index/64;
            size_t posVec = index%64;
            p[posArr] ^= (-tval ^ p[posArr]) & (1 << posVec);
        }
        
        bool get(const size_t & index) const
        {
            return getSlow(index);
        }
        
        bool getSlow(const size_t & index) const
        {
            const uint64_t * p = reinterpret_cast<const uint64_t*>(&_m);
            size_t posArr = index/64;
            size_t posVec = index%64;
            
            return (p[posArr] >> posVec) & 1;
        }

	static Vec slowZero() {
            __m256d res;
            uint64_t * p = reinterpret_cast<uint64_t *>(&res);
            uint64_t nullv = 0;
            for(size_t i=0;i<sizeof(__m256d)/sizeof(uint64_t);++i)
                p[i] = nullv;
            return Vec(res);
	}
	static Vec slowOne() {
            __m256d res;
            uint64_t * p = reinterpret_cast<uint64_t *>(&res);
            uint64_t eins = std::numeric_limits<uint64_t>::max();
            for(size_t i=0;i<sizeof(__m256d)/sizeof(uint64_t);++i)
                p[i] = eins;
            return Vec(res);
	}
	
	static const Vec & zero() {
            return zeroVec;
        }
	
	static const Vec & one() {
            return oneVec;
        }

	static size_t size()
	{
            return 256u;
	}

private:
	__m256d * _m;
        
        static const Vec zeroVec;
        static const Vec oneVec;
};

const Vec Vec::zeroVec = Vec::slowZero();
const Vec Vec::oneVec = Vec::slowOne();


#endif /* VEC_HPP_ */
