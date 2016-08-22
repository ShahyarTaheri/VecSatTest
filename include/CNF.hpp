/*
 * CNF.hpp
 *
 *  Created on: 13.08.2016
 *      Author: hartung
 */

#ifndef CNF_HPP_
#define CNF_HPP_

#include <vector>
#include <string>
#include <fstream>

#include "Lit.hpp"
#include "LitVec.hpp"

enum Sign
{
	IDENT, NEGAT
};

template<size_t n, typename base_vec>
class CNF {
public:

	typedef LitVec<n,base_vec> LitVector;

	CNF(const std::string & file) {

	}
	
	CNF(CNF<n,base_vec> && in)
        {
            std::swap(in._clauses,_clauses);
            std::swap(in._lits,_lits);
        }

	size_t numClauses() const
	{
		return _clauses.size();
	}

	size_t numLits() const
	{
		return _lits.size();
	}
	
	Clause & getClause(const size_t & num)
        {
            return _clauses[num];
        }
        
        LitVector & getLit(const size_t & num)
        {
            return _lits[num];
        }



private:
	std::vector<Clause> _clauses;
	std::vector<Lit<LitVector>> _lits;
};

#endif /* CNF_HPP_ */
