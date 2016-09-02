/*
 * main.cpp
 *
 *  Created on: 12.08.2016
 *      Author: hartung
 */

#include <iostream>

#include "../include/Vec.hpp"
#include "../include/LitVec.hpp"
#include "../include/CNF.hpp"
#include "../include/CNFSolver.hpp"

#include "../include/Test/TestLitVec.hpp"

//typedef bool simd_type;
//typedef __m64 simd_type;
//typedef __m128d simd_type;
typedef __m256d simd_type;
#define NUM_VECS 1

int main(int argc, char * argv[])
{
   TestLitVec<NUM_VECS,simd_type> test;
   if(!test.test())
      throw std::runtime_error("Simd implementation isn't working.");
   if (argc < 2)
   {
      std::cout << "No CNF-file passed.\n";
      return 0;
   } else
   {
      std::string file(argv[1]);
      CNF<NUM_VECS, simd_type> cnf(file);
      CNFSolver<NUM_VECS, simd_type> solver(cnf);
      solver.dumpBytesUsed();
      if (solver.solve())
      {
         std::cout << "SAT" << std::endl;
         //cnf.writeSolution();
      } else
         std::cout << "UNSAT" << std::endl;
   }
   return 0;
}

