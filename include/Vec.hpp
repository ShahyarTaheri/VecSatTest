/*
 * Vec.hpp
 *
 *  Created on: 09.08.2016
 *      Author: hartung
 */

#ifndef VEC_HPP_
#define VEC_HPP_

#include <immintrin.h>
#include <emmintrin.h>
#include <mmintrin.h>

#include <limits>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <array>

//#include <bitset>

/////////////////////////////////
//////////// Template ///////////
/////////////////////////////////

template<typename simd_type>
simd_type zeroSimd(const simd_type &)
{
   throw std::runtime_error("Simd type not supported.");
}

template<typename simd_type>
simd_type oneSimd(const simd_type &)
{
   throw std::runtime_error("Simd type not supported.");
}

template<typename simd_type>
void andAssign(simd_type & out, const simd_type & in)
{
   throw std::runtime_error("Simd type not supported.");
}

template<typename simd_type>
void orAssign(simd_type & out, const simd_type & in)
{
   throw std::runtime_error("Simd type not supported.");
}

template<typename simd_type>
void neq(simd_type & out)
{
   throw std::runtime_error("Simd type not supported.");
}

/////////////////////////////////
////////////// bool//////////////
/////////////////////////////////

template<>
bool zeroSimd(const bool &)
{
   return false;
}

template<>
bool oneSimd(const bool &)
{
   return true;
}

template<>
void andAssign(bool & out, const bool & in)
{
   out = out && in;
}

template<>
void orAssign(bool & out, const bool & in)
{
   out = out || in;
}

template<>
void neq(bool & out)
{
   out = !out;
}

/////////////////////////////////
////////////// MMX //////////////
/////////////////////////////////

template<>
__m64 zeroSimd(const __m64 &)
{
   return _mm_set1_pi32(0);
}

template<>
__m64 oneSimd(const __m64 &)
{
   return reinterpret_cast<__m64 >(_mm_set1_pi32(-1));
}

template<>
void andAssign(__m64 & out, const __m64 & in)
{
   out = _mm_and_si64(out, in);
}

template<>
void orAssign(__m64 & out, const __m64 & in)
{
   out = _mm_or_si64(out, in);
}

template<>
void neq(__m64 & out)
{
   out = _mm_xor_si64(oneSimd(out), out);
}

/////////////////////////////////
////////////// SSE //////////////
/////////////////////////////////

template<>
__m128d zeroSimd(const __m128d &)
{
   return _mm_setzero_pd();
}

template<>
__m128d oneSimd(const __m128d &)
{
   return reinterpret_cast<__m128d >(_mm_set1_epi64x(-1));
}

template<>
void andAssign(__m128d & out, const __m128d & in)
{
   out = _mm_and_pd(out, in);
}

template<>
void orAssign(__m128d & out, const __m128d & in)
{
   out = _mm_or_pd(out, in);
}

template<>
void neq(__m128d & out)
{
   out = _mm_xor_pd(oneSimd(out), out);
}

/////////////////////////////////
////////////// AVX //////////////
/////////////////////////////////

template<>
__m256d zeroSimd(const __m256d &)
{
   return _mm256_setzero_pd();
}

template<>
__m256d oneSimd(const __m256d &)
{
   return reinterpret_cast<__m256d >(_mm256_set1_epi64x(-1));
}

template<>
void andAssign(__m256d & out, const __m256d & in)
{
   out = _mm256_and_pd(out, in);
}

template<>
void orAssign(__m256d & out, const __m256d & in)
{
   out = _mm256_or_pd(out, in);
}

template<>
void neq(__m256d & out)
{
   out = _mm256_xor_pd(oneSimd(out), out);
}

/*
 /////////////////////////////////
 //////////// AVX 512 ////////////
 /////////////////////////////////

 template<>
 __m512d zeroSimd(const __m512d &)
 {
 return _mm512_setzero_pd();
 }

 template<>
 __m512d oneSimd(const __m512d &)
 {
 return reinterpret_cast<__m512d>(_mm512_set1_epi64(-1));
 }

 template<>
 void andAssign(__m512d & out, const __m512d & in)
 {
 out = _mm512_and_pd(out, in);
 }

 template<>
 void orAssign(__m512d & out, const __m512d & in)
 {
 out = _mm512_or_pd(out, in);
 }

 template<>
 void neq(__m512d & out)
 {
 out = _mm512_xor_pd(oneSimd(out), out);
 }
 */
/////////////////////////////////
//////////// Template ///////////
/////////////////////////////////
typedef uint32_t shift_type;
bool GfastAccessInited = false;

std::vector<std::array<shift_type, 2>> GfastAccessArray;

template<typename simd_type>
simd_type* allocateSimd(const size_t & n)
{
   return reinterpret_cast<simd_type*>(_mm_malloc(sizeof(simd_type) * n, sizeof(simd_type)));
}

template<typename simd_type>
void deallocateSimd(simd_type * vec)
{
   _mm_free(vec);
}

template<typename simd_type>
void setFalse(simd_type * in, const size_t & posArr, const size_t & posVec)
{
   shift_type * p = reinterpret_cast<shift_type*>(in);
   p[posArr] &= ~(1 << posVec);
}

template<typename simd_type>
void setTrue(simd_type * in, const size_t & posArr, const size_t & posVec)
{
   shift_type * p = reinterpret_cast<shift_type*>(in);
   p[posArr] |= 1 << posVec;
}

template<typename simd_type>
bool getSimd(const simd_type * in, const size_t & index, const size_t & n)
{
   const shift_type * p = reinterpret_cast<const shift_type*>(in);
   if (!GfastAccessInited)
   {
      GfastAccessArray = std::vector<std::array<shift_type, 2>>(n * sizeof(simd_type) * 8);
      for (size_t i = 0; i < n * sizeof(simd_type) * 8; ++i)
      {
         GfastAccessArray[i][0] = i / (sizeof(shift_type) * 8);  //i / (n * 64);
         GfastAccessArray[i][1] = i % (sizeof(shift_type) * 8);  //i / (n * 64);
      }
      GfastAccessInited = true;
   }
   shift_type posArr = GfastAccessArray[index][0];
   shift_type posVec = GfastAccessArray[index][1];

   return p[posArr] & (1 << posVec);
   //return (p[posArr] >> posVec) & 1;
   //std::bitset<sizeof(shift_type)*8> bset(p[posArr]);
   //std::cout << "Bitset: " << bset << "\n";
   //return bset[posVec];
}

template<typename simd_type>
void setSimd(simd_type * in, const size_t & index, const size_t & n, const bool & val)
{
   if (!GfastAccessInited)
   {
      GfastAccessArray = std::vector<std::array<shift_type, 2>>(n * sizeof(simd_type) * 8);
      for (size_t i = 0; i < n * sizeof(simd_type) * 8; ++i)
      {
         GfastAccessArray[i][0] = i / (sizeof(shift_type) * 8);  //i / (n * 64);
         GfastAccessArray[i][1] = i % (sizeof(shift_type) * 8);  //i / (n * 64);
      }
      GfastAccessInited = true;
   }
   shift_type posArr = GfastAccessArray[index][0];
   shift_type posVec = GfastAccessArray[index][1];
   if (val)
      setTrue(in, posArr, posVec);
   else
      setFalse(in, posArr, posVec);
}

template<typename simd_type>
void assign(simd_type & out, const simd_type & in)
{
   out = in;
}

template<typename simd_type>
bool not_equal(const simd_type & in1, const simd_type & in2)
{

   const uint64_t * p1 = reinterpret_cast<const uint64_t*>(&in1);
   const uint64_t * p2 = reinterpret_cast<const uint64_t*>(&in2);
   for (size_t i = 0; i < sizeof(simd_type) / 8; ++i)
      if (p1[i] != p2[i])
         return true;
   return false;
}

template<typename simd_type>
constexpr size_t simdSize()
{
   return sizeof(simd_type) * 8;
}

/////////////////////////////////
//////// Template (Bool)/////////
/////////////////////////////////

template<>
bool* allocateSimd(const size_t & n)
{
   return new bool[n];
}

template<>
void deallocateSimd(bool * vec)
{
   delete[] vec;
}

template<>
bool getSimd(const bool * in, const size_t & index, const size_t & n)
{
   return in[index];
}

template<>
void setSimd(bool * in, const size_t & index, const size_t & n, const bool & val)
{
   in[index] = val;
}

template<>
void assign(bool & out, const bool & in)
{
   out = in;
}

template<>
bool not_equal(const bool & in1, const bool & in2)
{
   return in1 != in2;
}

template<>
constexpr size_t simdSize<bool>()
{
   return 1;
}

#endif /* VEC_HPP_ */
