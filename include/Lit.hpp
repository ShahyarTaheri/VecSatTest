/*
 * Lit.hpp
 *
 *  Created on: 14.08.2016
 *      Author: hartung
 */

#ifndef LIT_HPP_
#define LIT_HPP_

#include "LitVec.hpp"

enum LitStatus
{
	UNSET=0, SCHROEDINGER_SET = 1, SIMPLE_SET = 2
};

template<typename LitVal>
struct Lit
{
	LitStatus status;
	bool simpleVal;
	LitVal val;
	std::vector<int32_t> clauses;
};



#endif /* LIT_HPP_ */
