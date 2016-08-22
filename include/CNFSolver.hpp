/*
 * CNFSolver.hpp
 *
 *  Created on: 14.08.2016
 *      Author: hartung
 */

#ifndef CNFSOLVER_HPP_
#define CNFSOLVER_HPP_

#include <vector>
#include <cstdint>

#include "CNF.hpp"
#include "LitSetGraph.hpp"

enum SolverStatus
{
	CHOOSE_LIT,
};

template<size_t n, typename base_vec>
class CNFSolver {
public:
	CNFSolver() {

	}

	std::vector<bool> solve() {
		bool run = true;
		while(run)
		{
			// CHOOSE LIT ->> graph -> schroedinger??

			// SET ACTIVE CLAUSES

			// LOOK FOR CLAUSE EVALUATIONS

			// VECTOR SMALL? -> REDUCE
			// OR ZERO? Use other branch
		}


		std::vector<bool> res(_cnf.numLits());
		return res;
	}
private:
	CNF<n, base_vec> _cnf;
	LitSetGraph _g;
	std::vector<uint32_t> _activeClauses;
	unsigned short _usedSchroedinger;

	void updateActiveClauses(const uint32_t & litId)
	{

	}


};

#endif /* CNFSOLVER_HPP_ */
