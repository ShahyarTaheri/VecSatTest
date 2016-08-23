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

int main(int argc, char * argv[])
{
        if(argc < 2)
        {
            std::cout << "No CNF-file passed.\n";
        }
        else
        {
            std::string file(argv[1]);
            CNF<1,Vec> cnf(file);
            cnf.predictedMemoryUsage();
            return 0;
            CNFSolver<1,Vec> solver(cnf);
            if(solver.solve())
            {
                std::cout << "SAT" << std::endl;
                cnf.writeSolution();
            }
            else
                std::cout << "UNSAT" << std::endl;
        }
	return 0;
}


