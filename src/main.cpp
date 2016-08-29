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

typedef __m256d simd_type;
#define NUM_VECS 2

int main(int argc, char * argv[])
{
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

