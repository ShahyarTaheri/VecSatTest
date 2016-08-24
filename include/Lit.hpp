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

template<typename LitVal>
struct Lit {
	bool simpleVal = false;
	const ClauseState * clauseState = nullptr;
	LitVal val;
};

#endif /* LIT_HPP_ */
