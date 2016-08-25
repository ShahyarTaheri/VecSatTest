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
#include <algorithm>
#include <cstdint>

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
    return reinterpret_cast<__m64>(_mm_set1_pi32(-1));
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
    return reinterpret_cast<__m128d>(_mm_set1_epi64x(-1));
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
    return reinterpret_cast<__m256d>(_mm256_set1_epi64x(-1));
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


template<typename simd_type>
__m256d* allocateSimd(const size_t & n)
{
	return reinterpret_cast<__m256d*>(_mm_malloc(sizeof(simd_type)*n,sizeof(simd_type)));
}

template<typename simd_type>
bool getSimd(const simd_type & in, const size_t & index)
{
    const uint64_t * p = reinterpret_cast<const uint64_t*>(&in);
    size_t posArr = index/64;
    size_t posVec = index%64;
            
    return (p[posArr] >> posVec) & 1;
}

template<typename simd_type>
void setSimd(simd_type & in, const size_t & index, const bool & val)
{
    uint64_t tval = val;
    uint64_t * p = reinterpret_cast<uint64_t*>(&in);
    size_t posArr = index/64;
    size_t posVec = index%64;
    p[posArr] ^= (-tval ^ p[posArr]) & (1 << posVec);    
}

template<typename simd_type>
void assign(simd_type & out, const simd_type & in)
{
    out = in;
}

template<typename simd_type>
bool not_equal(const simd_type & in1, const simd_type & in2)
{
    return !std::equal(reinterpret_cast<const uint64_t*>(&in1),
                       reinterpret_cast<const uint64_t*>(&in1)+(sizeof(simd_type)/64),
                       reinterpret_cast<const uint64_t*>(&in2));
}

#endif /* VEC_HPP_ */
