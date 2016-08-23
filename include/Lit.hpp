/*
 * Lit.hpp
 *
 *  Created on: 14.08.2016
 *      Author: hartung
 */

#ifndef LIT_HPP_
#define LIT_HPP_

#include "LitVec.hpp"
#include "ClauseState.hpp"

enum LitStatus
{
	UNSET=0, SCHROEDINGER_SET = 1, SIMPLE_SET = 2
};

template<typename LitVal>
struct Lit
{
	bool simpleVal = false;
        bool touched = false;
        short neg = 0;
        short pos = 0;
        const ClauseState * clauseState = nullptr;
	LitVal val;
	std::vector<int32_t> clauses;
        
        bool isSet() const
        {
            return clauseState != nullptr;
        }
        
        void unset()
        {
            simpleVal = false;
            touched = false;
            neg = 0;
            pos = 0;
            clauseState = nullptr;
        }
};



#endif /* LIT_HPP_ */
