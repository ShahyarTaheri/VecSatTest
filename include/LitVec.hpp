/*
 * LitVec.hpp
 *
 *  Created on: 09.08.2016
 *      Author: hartung
 */

#ifndef LITVEC_HPP_
#define LITVEC_HPP_

#include <cstddef>
#include <cmath>
#include <algorithm>
#include <memory>

template<size_t n, typename base_vec>
class LitVec {
public:
	LitVec() :
			LitVec(true) {

	}

	LitVec(const bool & empty) :
			_vec((empty) ? nullptr : new base_vec[n]) {

	}

	LitVec(const LitVec<n, base_vec> & in) :
			_vec(new base_vec[n]) {
		std::copy(in._vec, in._vec + n, _vec);
	}

	LitVec(LitVec<n, base_vec> && in) {
		std::swap(_vec, in._vec);
	}

	~LitVec() {
		if (_vec != nullptr)
			delete[] _vec;
	}
	
	bool operator!=(const LitVec<n, base_vec> & in) const {
            for(size_t i=0;i<n;++i)
                if(_vec[i] != in._vec[i])
                    return true;
            return false;
        }

	LitVec<n, base_vec> & operator =(const LitVec<n, base_vec> & in) {
		for (size_t i = 0; i < n; ++i)
			_vec[i] = in._vec[i];
		return *this;
	}

	LitVec<n, base_vec> & operator ^=(const LitVec<n, base_vec> & in) {
		for (size_t i = 0; i < n; ++i)
			_vec[i] ^= in._vec[i];
		return *this;
	}

	LitVec<n, base_vec> & operator |=(const LitVec<n, base_vec> & in) {
		for (size_t i = 0; i < n; ++i)
			_vec[i] |= in._vec[i];
		return *this;
	}

	LitVec<n, base_vec> operator ^(const LitVec<n, base_vec> & in) const {
		LitVec<n, base_vec> res(*this);
		res ^= in;
		return *this;
	}

	LitVec<n, base_vec> operator |(const LitVec<n, base_vec> & in) const {
		LitVec<n, base_vec> res(*this);
		res ^= in;
		return *this;
	}

	LitVec<n, base_vec> operator -() const {
		LitVec<n, base_vec> res(*this);
		for (size_t i = 0; i < n; ++i)
			res._vec[i] = -res._vec[i];
		return res;
	}
	
	bool get(const size_t & index) const
	{
            return _vec[index/n].get(index%base_vec::size());
        }

	bool isEmpty() const {
		return _vec == nullptr;
	}

	static size_t size()
	{
		return n*base_vec::size();
	}

	static size_t maxNumSchroedinger() {
		return _numSchroedinger;
	}

	static size_t slowMaxNumSchroedinger() {
		return std::log2(n * base_vec::size());
	}

	static const LitVec<n, base_vec> & one() {
		return _oneVec;
	}

	static const LitVec<n, base_vec> & zero() {
		return _zeroVec;
	}

	static const LitVec<n, base_vec> slowCreateConstVec(const bool & v) {
		LitVec<n, base_vec> res(false);
                if(v)
                {
                    const base_vec & a = base_vec::zero();
                    for (size_t i = 0; i < n; ++i)
			res._vec[i] = a;
		
                }
                else
                {
                    const base_vec & a = base_vec::one();
                    for (size_t i = 0; i < n; ++i)
			res._vec[i] = a;
		
                }
		return res;
	}

	static const LitVec<n, base_vec> & schroedinger(
			const size_t & index) {
		return _schroedingerArray[index];
	}

	static std::shared_ptr<const LitVec<n, base_vec>> createSchroedMemory() {
		LitVec<n, base_vec> * res = new LitVec<n, base_vec> [_numSchroedinger];
		for (size_t i = 0; i < _numSchroedinger; ++i)
			res[i]._vec = new base_vec[n];

		for(size_t num = 0;num < _numSchroedinger;++num)
		{
			size_t changeEvery = std::pow(2,num+1);
			size_t counter = 0;
			bool cur = false;
			for(size_t i=0;i<size();++i)
			{
				res[num]._vec[i%n].set(i%n,cur);
				if(++counter == changeEvery)
				{
					counter = 0;
					cur = -cur;
				}
			}
		}

		return std::shared_ptr<const LitVec<n, base_vec>>(res);
	}

private:
	base_vec * _vec;

	static const LitVec<n, base_vec> _zeroVec;
	static const LitVec<n, base_vec> _oneVec;

	static const size_t _numSchroedinger;
	static std::shared_ptr<const LitVec<n, base_vec>> _schroedMemory;
	static const LitVec<n, base_vec> * _schroedingerArray;
};

template<size_t n, typename base_vec>
const LitVec<n, base_vec> LitVec<n, base_vec>::_zeroVec =
		LitVec<n, base_vec>::slowCreateConstVec(false);
template<size_t n, typename base_vec>
const LitVec<n, base_vec> LitVec<n, base_vec>::_oneVec =
		LitVec<n, base_vec>::slowCreateConstVec(true);
template<size_t n, typename base_vec>
const size_t LitVec<n, base_vec>::_numSchroedinger =
		LitVec<n, base_vec>::slowMaxNumSchroedinger();
template<size_t n, typename base_vec>
std::shared_ptr<const LitVec<n, base_vec>> LitVec<n, base_vec>::_schroedMemory =
		LitVec<n, base_vec>::createSchroedMemory();
template<size_t n, typename base_vec>
const LitVec<n, base_vec> * LitVec<n, base_vec>::_schroedingerArray = LitVec<n,
		base_vec>::_schroedMemory.get();

#endif /* LITVEC_HPP_ */
