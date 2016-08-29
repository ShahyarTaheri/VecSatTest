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

constexpr unsigned Log2(unsigned n, unsigned p = 0) {
    return (n <= 1) ? p : Log2(n / 2, p + 1);
}

template<size_t n, typename base_vec>
class LitVec
{
 public:

   LitVec()
   {

   }

   LitVec(const LitVec<n, base_vec> & in)
   {
      std::copy(in._vec, in._vec + n, _vec);
   }

   LitVec(LitVec<n, base_vec> && in)
   {
      std::swap(_vec, in._vec);
   }

   ~LitVec()
   {
   }

   bool operator!=(const LitVec<n, base_vec> & in) const
   {
      for (size_t i = 0; i < n; ++i)
         if (not_equal(_vec[i], in._vec[i]))
            return true;
      return false;
   }

   LitVec<n, base_vec> & operator =(const LitVec<n, base_vec> & in)
   {
      for (size_t i = 0; i < n; ++i)
         assign(_vec[i], in._vec[i]);
      return *this;
   }

   LitVec<n, base_vec> & operator ^=(const LitVec<n, base_vec> & in)
   {
      for (size_t i = 0; i < n; ++i)
         andAssign(_vec[i], in._vec[i]);
      return *this;
   }

   LitVec<n, base_vec> & operator |=(const LitVec<n, base_vec> & in)
   {
      for (size_t i = 0; i < n; ++i)
         orAssign(_vec[i], in._vec[i]);
      return *this;
   }

   LitVec<n, base_vec> operator ^(const LitVec<n, base_vec> & in) const
   {
      LitVec<n, base_vec> res(*this);
      res ^= in;
      return *this;
   }

   LitVec<n, base_vec> operator |(const LitVec<n, base_vec> & in) const
   {
      LitVec<n, base_vec> res(*this);
      res ^= in;
      return *this;
   }

   LitVec<n, base_vec> operator -() const
   {
      LitVec<n, base_vec> res(*this);
      for (size_t i = 0; i < n; ++i)
         neq(res._vec[i]);
      return res;
   }

   bool get(const size_t & index) const
   {
      return getSimd(_vec[index / sizeof(base_vec)], index % sizeof(base_vec));
   }

   void set(const size_t & index, const bool & val)
   {
      setSimd(_vec[index / sizeof(base_vec)], index % sizeof(base_vec), val);
   }

   static constexpr size_t size()
   {
      return _size;
   }

   static constexpr size_t maxNumSchroedinger()
   {
      return _numSchroedinger;
   }

   static const LitVec<n, base_vec> slowCreateConstVec(const bool & v)
   {
      LitVec<n, base_vec> res;
      if (v)
      {
         base_vec a = zeroSimd<base_vec>(res._vec[0]);
         for (size_t i = 0; i < n; ++i)
            assign(res._vec[i], a);

      } else
      {
         base_vec a = oneSimd(res._vec[0]);
         for (size_t i = 0; i < n; ++i)
            assign(res._vec[i], a);

      }
      return res;
   }

   void createSchroedMemory(const size_t & num)
   {
      size_t changeEvery = std::pow(2, num + 1);
      size_t counter = 0;
      bool cur = false;
      for (size_t i = 0; i < size(); ++i)
      {
         set(i, cur);
         if (++counter == changeEvery)
         {
            counter = 0;
            cur = -cur;
         }
      }
   }

 private:
   base_vec _vec[n];

   static constexpr size_t _size = n * sizeof(base_vec)*8;
   static constexpr size_t _numSchroedinger = Log2(size());
};

template<size_t n, typename base_vec>
constexpr size_t LitVec<n,base_vec>::_size;
template<size_t n, typename base_vec>
constexpr size_t LitVec<n,base_vec>::_numSchroedinger;

#endif /* LITVEC_HPP_ */
