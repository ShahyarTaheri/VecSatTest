/*
 * CNFSolver.hpp
 *
 *  Created on: 14.08.2016
 *      Author: hartung
 */

#ifndef CNFSOLVER_HPP_
#define CNFSOLVER_HPP_

#include <vector>
#include <list>
#include <cstdint>

#include "LitVecMemory.hpp"

template<size_t n, typename base_vec>
class CNFSolver
{
 private:
   typedef LitVec<n, base_vec> LitVector;

   struct BranchToken
   {
      ClauseState clauseState;
      LitVector litState;

      BranchToken()
            : litState(LitVector())
      {
      }

      BranchToken(const size_t & numCurClauses)
            : clauseState(numCurClauses),
              litState(LitVector())
      {
      }

      BranchToken(const BranchToken & in)
            : clauseState(in.clauseState.posClause),
              litState(in.litState)
      {
      }
   };

 public:
   CNFSolver(CNF<n, base_vec> & cnf)
         : _posInTokenStack(0),
           _solutionFound(false),
           _cnf(&cnf)
   {
      init();
   }

   bool solve()
   {
      while (solveStep())
      {
      }

      return _solutionFound;
   }

   void init()
   {
      _vecMem.init();
      setUpMemory();
      setUpClauseOrder();
      setUpLitOrder();
   }

   bool hasSolution() const
   {
      return _solutionFound;
   }

   void dumpBytesUsed()
   {
      uint64_t numBytes = 0;
      numBytes += (sizeof(BranchToken) + LitVector::size()) * _tokenStack.size();
      numBytes += (sizeof(uint32_t) + sizeof(Clause)) * _clauseStack.size() + _cnf->countNumLitsInClause() * sizeof(int32_t);
      numBytes += sizeof(Lit) * _cnf->numLits();
      std::cout << "Allocating not less than " << (double)numBytes / (1024 * 1024) << " MB" << std::endl;
   }

 private:

   size_t _posInTokenStack;
   bool _solutionFound;
   const CNF<n, base_vec> * _cnf;
   std::vector<Lit> _lits;
   std::vector<uint32_t> _clauseStack;
   std::vector<BranchToken> _tokenStack;

   LitVecMemory<n, base_vec> _vecMem;

   void setUpMemory()
   {
      _clauseStack.resize(_cnf->numClauses(), 0);
      _tokenStack.resize(std::ceil(((double) _cnf->numLits()) / LitVector::maxNumSchroedinger()) + 1);
      _lits.resize(_cnf->numLits() + 1);
   }

   void setUpClauseOrder()
   {
      for (size_t i = 0; i < _cnf->numClauses(); ++i)
         _clauseStack[i] = i;
   }

   void setUpLitOrder()
   {
      size_t counter = 0;
      std::vector<bool> isSet(_cnf->numLits(), false);
      for (size_t i = 0; i < _clauseStack.size(); ++i)
      {
         const Clause & clause = _cnf->getClause(_clauseStack[i]);
         for (size_t j=0;j<clause.lits.size();++j)
         {
            const uint32_t & litId = std::abs(clause.lits[j]);
            if (!isSet[litId])
            {
               isSet[litId] = true;
               _lits[litId] = Lit(++counter, _posInTokenStack, _tokenStack[_posInTokenStack].clauseState);
            }
            if (counter == LitVector::maxNumSchroedinger())
            {
               _tokenStack[++_posInTokenStack] = BranchToken(i+1);
               counter = 0;
            }
         }
      }
      _tokenStack.back().clauseState.posClause = _clauseStack.size();
      _posInTokenStack = 0;
   }

   bool solveStep()
   {
      bool res = true;

      if (evaluateClauses())
      {
         // evaluate true:
         if (!pushToken())
         {
            // no new token possible => sat
            res = false;
            _solutionFound = true;
         }
      } else
      {
         // evaluate false
         if (!shiftLine())  // try get next line of token
         {
            // no next line -> popToken
            if (!popToken())
            {
               // backstep not possible => unsat
               res = false;
               _solutionFound = false;
            }
         }

      }

      return res;
   }

   bool evaluateClauses()
   {
      LitVector tmp;
      LitVector & curValue = _tokenStack[_posInTokenStack].litState;
      curValue = _vecMem.one();
      for (size_t i = _tokenStack[_posInTokenStack].clauseState.posClause; i < _tokenStack[_posInTokenStack + 1].clauseState.posClause; ++i)
      {
         if (curValue != _vecMem.zero())
         {
            tmp = _vecMem.zero();
            for (const int32_t & lit : _cnf->getClause(i).lits)
               tmp |= getValueOfLit(lit);
            curValue ^= tmp;
         } else
            return false;
      }

      return true;
   }

   bool shiftLine()
   {
      ClauseState & clauseState = _tokenStack[_posInTokenStack].clauseState;
      const auto & litVec = _tokenStack[_posInTokenStack].litState;

      while (++clauseState.posInVec < LitVector::size() && !litVec.get(clauseState.posInVec))
         ;  // TODO more sophisticated approach possible O(VecSize) => O(log(VecSize)) => O(1)

      if (clauseState.posInVec == LitVector::size())
         return false;
      else
         return true;
   }

   bool pushToken()
   {
      return ++_posInTokenStack < _tokenStack.size() - 1;
   }

   bool popToken()
   {
      _tokenStack[_posInTokenStack].clauseState.posInVec = 0;
      if (_posInTokenStack == 0)
         return false;
      else
      {
         --_posInTokenStack;
         return true;
      }
   }

   const LitVector & getValueOfLit(const int32_t & in)
   {
      Lit & tmpRef = _lits[std::abs(in)];
      if (tmpRef.tokenNum < _posInTokenStack)
         return ((_vecMem[tmpRef.memPos].get(tmpRef.clauseState->posInVec)) ? _vecMem.zero() : _vecMem.one());
      else
      {
         int32_t index = ((in < 0) ? -tmpRef.memPos : tmpRef.memPos);
         return _vecMem[index];
      }
   }

};

#endif /* CNFSOLVER_HPP_ */
