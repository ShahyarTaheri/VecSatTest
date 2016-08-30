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
#include <map>
#include <cstdint>
#include <ctime>

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
           _cnf(&cnf),
           _backStep(false),
           _printedEstimation(false),
           _zeroCounter(0),
           _zeroFuture(10000000),
           _lowestBackstepLvl(std::numeric_limits<size_t>::max()),
           _highestLvl(0),
           _startTime(0)
   {
      init();
   }

   bool solve()
   {
      _startTime = clock();
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
      //optimizeClauseOrder();
      //setUpLitOrder();
      //optimizeClauseOrder(false);
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
      std::cout << "Allocating not less than " << (double) numBytes / (1024 * 1024) << " MB" << std::endl;
      std::cout << "Setting " << LitVector::maxNumSchroedinger() << " in one block" << std::endl;
      for(size_t i=0;i<_tokenStack.size()-1;++i)
         std::cout << i << ". " << _tokenStack[i+1].clauseState.posClause -_tokenStack[i].clauseState.posClause
                   << " range (" << _tokenStack[i].clauseState.posClause << "," << _tokenStack[i+1].clauseState.posClause << ") to evaluate" << std::endl;
   }

 private:

   size_t _posInTokenStack;
   bool _solutionFound;
   const CNF<n, base_vec> * _cnf;
   std::vector<Lit> _lits;
   std::vector<uint32_t> _clauseStack;
   std::vector<BranchToken> _tokenStack;

   LitVecMemory<n, base_vec> _vecMem;
   bool _backStep;
   bool _printedEstimation;

   size_t _zeroCounter;
   size_t _zeroFuture;
   size_t _lowestBackstepLvl;
   size_t _highestLvl;
   uint64_t _startTime;

   void setUpMemory()
   {
      _clauseStack.resize(_cnf->numClauses(), 0);
      _tokenStack.resize(std::ceil(((double) _cnf->numLits()) / LitVector::maxNumSchroedinger()) + 1);
      std::cout << "NumTokens: " << _tokenStack.size() << std::endl;
      _lits.resize(_cnf->numLits() + 1);
   }

   void setUpClauseOrder()
   {
      for (size_t i = 0; i < _cnf->numClauses(); ++i)
         _clauseStack[i] = i;

      std::srand ( unsigned ( std::time(0) ) );
      random_shuffle(_clauseStack.begin(),_clauseStack.end());
   }

   void setUpLitOrder()
   {
      size_t counter = 0;
      std::vector<bool> isSet(_cnf->numLits(), false);
      size_t maxClausesInStep = 0;
      size_t maxIndex = 0;
      for (size_t i = 0; i < _clauseStack.size(); ++i)
      {
         const Clause & clause = _cnf->getClause(_clauseStack[i]);
         for (size_t j = 0; j < clause.lits.size(); ++j)
         {
            const uint32_t & litId = std::abs(clause.lits[j]);
            if (!isSet[litId])
            {
               isSet[litId] = true;
               _lits[litId] = Lit(++counter, _posInTokenStack, _tokenStack[_posInTokenStack].clauseState);
            }
            if (counter == LitVector::maxNumSchroedinger())
            {
               if (_posInTokenStack > 0 && maxClausesInStep < i - _tokenStack[_posInTokenStack - 1].clauseState.posClause)
               {
                  maxClausesInStep = i - _tokenStack[_posInTokenStack - 1].clauseState.posClause;
                  maxIndex = _posInTokenStack;
               }
               _tokenStack[++_posInTokenStack] = BranchToken(i + 1);
               counter = 0;
            }
         }
      }
      if (maxClausesInStep < _clauseStack.size() - _tokenStack[_tokenStack.size() - 2].clauseState.posClause)
      {
         maxClausesInStep = _clauseStack.size() - _tokenStack[_tokenStack.size() - 2].clauseState.posClause;
         maxIndex = _tokenStack.size() - 2;
      }
      //std::cout << "in " << maxIndex << ": need to evaluate " << maxClausesInStep << " clauses" << std::endl;
      _tokenStack.back().clauseState.posClause = _clauseStack.size();

      _posInTokenStack = 0;
   }

   bool solveStep()
   {
      bool res = true;
      if (_zeroCounter  > _zeroFuture)
      {
         _zeroFuture = _zeroCounter + 10000000000;
         std::cout << "Conflics per sec: " << (_zeroCounter / std::floor((double) (clock() - _startTime) / CLOCKS_PER_SEC)) << std::endl << "lowest backstep: "
               << _lowestBackstepLvl << " highest: " << _highestLvl << std::endl;
      }
      if (!_backStep && evaluateClauses())
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
         _backStep = false;
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
            for (const int32_t & lit : _cnf->getClause(_clauseStack[i]).lits)
               tmp |= getValueOfLit(lit);
            /*if (true)
            {
               for (const int32_t & lit : _cnf->getClause(i).lits)
                  std::cout << "lit:" << lit << " tokenNum:" << _lits[std::abs(lit)].tokenNum << "mem:" << _lits[std::abs(lit)].memPos << std::endl;
            curValue ^= tmp;
            }*/
            //std::cout << "ones: " << curValue.countOnes() << std::endl;
         } else
            return false;
      }

      return curValue != _vecMem.zero();
   }

   bool shiftLine()
   {
      ClauseState & clauseState = _tokenStack[_posInTokenStack].clauseState;
      const auto & litVec = _tokenStack[_posInTokenStack].litState;

      while (++clauseState.posInVec < LitVector::size() && !litVec.get(clauseState.posInVec))
         ++_zeroCounter;  // TODO more sophisticated approach possible O(VecSize) => O(log(VecSize)) => O(1)

      if (clauseState.posInVec == LitVector::size())
         return false;
      else
         return true;
   }

   bool pushToken()
   {
      //std::cout << "cur level: " << _posInTokenStack +1 << std::endl;
      if(!_printedEstimation && _posInTokenStack == _tokenStack.size()-2)
      {
         long double numPos = 1;
         for(size_t i=0;i<_tokenStack.size()-2;++i)
         {
            numPos *= std::max(1ul,_tokenStack[i].litState.countOnes());
            std::cout << "num ones: " << std::max(1ul,_tokenStack[i].litState.countOnes()) << std::endl;
         }
         std::cout << "Num possibilitiers: " << numPos << std::endl;
         std::cout << "This is " << 100.0*(((double)numPos)/ std::pow(2,_cnf->numLits())) << "% of a normal solver (" << std::pow(2,_cnf->numLits()) << ")" << std::endl;
         _printedEstimation = true;
      }
      if(_highestLvl < _posInTokenStack+1)
      {
         _highestLvl= _posInTokenStack+1;
      }
      return ++_posInTokenStack < _tokenStack.size() - 1;
   }

   bool popToken()
   {
      if(_lowestBackstepLvl > _posInTokenStack)
      {
         _lowestBackstepLvl = std::min(_lowestBackstepLvl, _posInTokenStack);
         std::cout << "Lowest level: " << _lowestBackstepLvl << " highest: " << _highestLvl << "\n";
      }
      _tokenStack[_posInTokenStack].clauseState.posInVec = 0;
      if (_posInTokenStack == 0)
         return false;
      else
      {
         --_posInTokenStack;
         _backStep = true;
         return true;
      }
   }

   const LitVector & getValueOfLit(const int32_t & in)
   {
      Lit & tmpRef = _lits[std::abs(in)];
      if (tmpRef.tokenNum < _posInTokenStack)
         return ((_vecMem[tmpRef.memPos].get(tmpRef.clauseState->posInVec)) ? _vecMem.zero() : _vecMem.one());
      else if(tmpRef.tokenNum == _posInTokenStack)
      {
         int32_t index = ((in < 0) ? -tmpRef.memPos : tmpRef.memPos);
         return _vecMem[index];
      }
      else
         throw std::runtime_error("Lits are in a wrong order");
   }

   void optimizeClauseOrder(bool optimize = true)
   {
      std::vector<std::list<size_t> > clauseNumToTokenNum(_tokenStack.size());
      size_t numMoves = 0;
      for (size_t i = 0; i < _tokenStack.size() - 1; ++i)
      {
         for (size_t j = _tokenStack[i].clauseState.posClause; j < _tokenStack[i + 1].clauseState.posClause; ++j)
         {
            size_t maxToken = 0;
            for (const int32_t & lit : _cnf->getClause(_clauseStack[j]).lits)
            {
               maxToken = std::max(_lits[std::abs(lit)].tokenNum, maxToken);
            }
            if (maxToken < i)
            {
               ++numMoves;
            }
            clauseNumToTokenNum[maxToken].push_back(_clauseStack[j]);
         }

      }
      std::cout << "Move " << numMoves << " clauses for optimization" << std::endl;
      if (optimize)
      {
         size_t clauseIt = 0;
         for (const auto & cList : clauseNumToTokenNum)
         {
            for (const auto & clauseId : cList)
            {
               _clauseStack[clauseIt++] = clauseId;
            }
         }
      }
      //else
      //   exit(0);
   }

};

#endif /* CNFSOLVER_HPP_ */
