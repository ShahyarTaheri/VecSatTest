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

struct Lit
{
   short memPos = 0;
   size_t tokenNum;
   const ClauseState * clauseState;

   Lit()
         : memPos(0),
           tokenNum(0),
           clauseState(nullptr)
   {
   }

   Lit(const short & memPos, const size_t & tokenNum, const ClauseState & clauseState)
         : memPos(memPos),
           tokenNum(tokenNum),
           clauseState(&clauseState)
   {
   }
};

#endif /* LIT_HPP_ */
