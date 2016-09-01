/*
 * testVec.hpp
 *
 *  Created on: 31.08.2016
 *      Author: hartung
 */

#include <iostream>

#include "../LitVec.hpp"
#include "../LitVecMemory.hpp"

template<size_t n, typename base_vec>
class TestLitVec
{
 public:
   typedef LitVec<n, base_vec> Vector;

   TestLitVec()
   {
      _mem.init(Vector::maxNumSchroedinger());
   }

   bool test()
   {
      bool btest= true;
      Vector one = _mem.one(), zero = _mem.zero();
      if (-one != zero)
         btest = false;
      if ((one ^= zero) != zero)
         btest = false;
      if ((zero ^= one) != zero)
         btest = false;
      if ((zero ^= zero) != zero)
         btest = false;
      if ((one ^= one) != one)
         btest = false;
      if(!btest)
      {
         std::cout << "'^=' - operator is buggy." << std::endl;
         return false;
      }
      one = _mem.one(); zero = _mem.zero();
      if ((one |= zero) != one)
         btest = false;
      if ((one |= one) != one)
         btest = false;
      if ((zero |= one) != one)
         btest = false;
      if ((zero |= zero) != zero)
         btest = false;
      if(!btest)
      {
         std::cout << "'|=' - operator is buggy." << std::endl;
         return false;
      }

      one = _mem.one(); zero = _mem.zero();
      if(zero != -one)
         btest = false;
      if(-zero != one)
         btest = false;
      if(one != -zero)
         btest = false;
      if(-one != zero)
         btest = false;
      if(-(-zero) != zero)
         btest = false;
      if(-(-one) != one)
         btest = false;
      if(!btest)
      {
         std::cout << "'-' - operator is buggy." << std::endl;
         return false;
      }

      Vector schroed;
      for(size_t i=1;i<=Vector::maxNumSchroedinger();++i)
      {
         schroed.createSchroedMemory(i);
         if(schroed != -(-schroed))
            btest = false;
         if(schroed != -_mem[-i])
            btest = false;
         if(-schroed != _mem[-i])
            btest = false;

         if(!btest)
         {
            std::cout << "schroed creation is buggy." << std::endl;
            return false;
         }

      }
      return btest;
   }

 private:
   LitVecMemory<n, base_vec> _mem;
};
