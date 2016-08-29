/*
 * Clause.hpp
 *
 *  Created on: 14.08.2016
 *      Author: hartung
 */

#ifndef CLAUSE_HPP_
#define CLAUSE_HPP_

#include <vector>

#include "ClauseState.hpp"

struct Clause
{
    //ClauseState * clauseState = nullptr;
    std::vector<int32_t> lits;
};



#endif /* CLAUSE_HPP_ */
