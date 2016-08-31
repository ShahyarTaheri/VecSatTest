/*
 * LitVecMemory.hpp
 *
 *  Created on: 26.08.2016
 *      Author: hartung
 */

#ifndef INCLUDE_LITVECMEMORY_HPP_
#define INCLUDE_LITVECMEMORY_HPP_

#include "LitVec.hpp"

template<size_t n, typename base_vec>
class LitVecMemory
{
 public:
   LitVecMemory()
         : _schroedShift(0),
           _allLitVecs(nullptr),
           _numAdditionals(0)
   {
   }

   ~LitVecMemory()
   {
      if(_allLitVecs != nullptr)
         delete[] _allLitVecs;
   }

   void init(const size_t & numLits)
   {
      initMemory(numLits);
      initZeroOne();
      initSchroed(numLits);
   }

   const LitVec<n, base_vec> & operator[](const int32_t & index) const
   {
      return _allLitVecs[index + _schroedShift];
   }

   const LitVec<n, base_vec> & one() const
   {
      return _allLitVecs[1];
   }

   const LitVec<n, base_vec> & zero() const
   {
      return _allLitVecs[0];
   }

   size_t numAdditionals() const
   {
      return _numAdditionals;
   }

 private:
   size_t _schroedShift;
   size_t _numAdditionals;
   /**
    * Structure: ar[0] = 0, ar[1] = 1, ar[2] = -schroed[maxNumSchroed] ar[3] = -schroed[maxNumSchroed-1], ....
    *            ar[maxNumSchroed] = schroed[0], ..., ar[2*maxNumSchroed] = schroed[maxNumSchroed]
    */
   LitVec<n, base_vec> * _allLitVecs;

   void initMemory(const size_t & numLits)
   {
      _numAdditionals = numLits % LitVec<n, base_vec>::maxNumSchroedinger();
      size_t aarrSize = 2 * (LitVec<n, base_vec>::maxNumSchroedinger() + _numAdditionals) + 3;
      _allLitVecs = new LitVec<n, base_vec> [aarrSize];
      _schroedShift = LitVec<n, base_vec>::maxNumSchroedinger() + _numAdditionals + 2;
   }

   void initZeroOne()
   {
      _allLitVecs[1] = LitVec<n, base_vec>::slowCreateConstVec(false);
      _allLitVecs[0] = LitVec<n, base_vec>::slowCreateConstVec(true);
   }

   void initSchroed(const size_t & numLits)
   {
      //positive
      for (size_t i = 1; i <= LitVec<n, base_vec>::maxNumSchroedinger(); ++i)
      {
         _allLitVecs[i + _schroedShift].createSchroedMemory(i);
         _allLitVecs[_schroedShift - i] = -_allLitVecs[i + _schroedShift];
      }

      // init schroed for leave nodes
      if (_numAdditionals > 0)
      {
         for (size_t i = 1; i <= _numAdditionals; ++i)
         {
            size_t posIndex = i + _schroedShift + LitVec<n, base_vec>::maxNumSchroedinger();
            size_t negIndex = _schroedShift - LitVec<n, base_vec>::maxNumSchroedinger() -i;
            _allLitVecs[posIndex] = _allLitVecs[i + _schroedShift];
            _allLitVecs[negIndex] = _allLitVecs[_schroedShift -i];

            //set lowerparts zero:
            size_t startI = std::pow(2, _numAdditionals);
            for(size_t j=startI;j<LitVec<n, base_vec>::size();++j)
            {
               _allLitVecs[posIndex].set(j,false);
               _allLitVecs[negIndex].set(j,false);
            }
         }
      }
   }

};

#endif /* INCLUDE_LITVECMEMORY_HPP_ */
