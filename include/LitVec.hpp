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

template<size_t n, typename base_vec>
class LitVec {
public:
	LitVec() :
			LitVec(true) {

	}

	LitVec(const bool & empty)
		: _vec((empty) ? nullptr : new base_vec[n])
	{

	}

	LitVec(const LitVec<n,base_vec> & in)
		: _vec(new base_vec[n])
	{
		std::copy(in._vec,in._vec+n,_vec);
	}

	LitVec(LitVec<n,base_vec> && in)
	{
		std::swap(_vec, in._vec);
	}

	~LitVec()
	{
		if(_vec != nullptr)
			delete[] _vec;
	}

	LitVec<n,base_vec> & operator =(const LitVec<n,base_vec> & in)
	{
		for(size_t i=0;i<n;++i)
			_vec[i] = in._vec[i];
		return *this;
	}

	LitVec<n,base_vec> & operator ^=(const LitVec<n,base_vec> & in)
	{
		for(size_t i=0;i<n;++i)
			_vec[i] ^= in._vec[i];
		return *this;
	}

	LitVec<n,base_vec> & operator |=(const LitVec<n,base_vec> & in)
	{
		for(size_t i=0;i<n;++i)
			_vec[i] |= in._vec[i];
		return *this;
	}


	LitVec<n,base_vec> operator ^(const LitVec<n,base_vec> & in) const
	{
		LitVec<n,base_vec> res(*this);
		res ^= in;
		return *this;
	}

	LitVec<n,base_vec> operator |(const LitVec<n,base_vec> & in) const
	{
		LitVec<n,base_vec> res(*this);
		res ^= in;
		return *this;
	}

	LitVec<n,base_vec> operator -() const
	{
		LitVec<n,base_vec> res(*this);
		for(size_t i=0;i<n;++i)
			res._vec[i] = -res._vec[i];
		return res;
	}

	bool isEmpty() const
	{
		return _vec == nullptr;
	}

	static size_t maxNumSchroedinger()
	{
		return std::log2(n*base_vec::size());
	}
	
	static const LitVec<n,base_vec> & one()
        {
            return oneVec;
        }
        
        static const LitVec<n,base_vec> & zero()
        {
            return zeroVec;
        }

private:
	base_vec * _vec;
        
        static const LitVec<n, base_vec> zeroVec;
        static const LitVec<n, base_vec> oneVec;
};

template<size_t n, typename base_vec>
LitVec<n,base_vec> createConstVec(const bool val)
{
    //TODO also do schroedinger set up!!!
}

#endif /* LITVEC_HPP_ */
