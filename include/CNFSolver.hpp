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

#include "CNF.hpp"
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
                : litState(LitVector(false))
        {
        }

        BranchToken(const size_t & numCurClauses)
                : BranchToken(),
                  clauseState(ClauseState(numCurClauses))
        {
        }
    };

 public:
    CNFSolver(CNF<n, base_vec> & cnf)
            : _posInTokenStack(0),
              _solutionFound(false),
              _cnf(cnf)
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
        numBytes += (sizeof(uint32_t) + sizeof(Clause)) * _clauseStack.size() + _cnf.countNumLitsInClause() * sizeof(int32_t);
        numBytes += sizeof(Lit) * _cnf.numLits();
        std::cout << "Allocating not less than " << numBytes / (1024 * 1024) << " MB" << std::endl;
    }

 private:

    size_t _posInTokenStack;
    bool _solutionFound;

    LitVecMemory<n, base_vec> _vecMem;
    CNF<n, base_vec> & _cnf;

    std::vector<uint32_t> _clauseStack;
    std::vector<BranchToken> _tokenStack;

    void setUpMemory()
    {
        _clauseStack.resize(_cnf.numClauses(), 0);
        _tokenStack.resize(std::ceil(((double) _cnf.numLits()) / LitVector::maxNumSchroedinger()) + 1);
    }

    void setUpClauseOrder()
    {
        for (size_t i = 0; i < _cnf.numClauses(); ++i)
            _clauseStack[i] = i;
    }

    void setUpLitOrder()
    {
        size_t counter = 0;
        size_t posInClauseStack = 0;
        std::vector<bool> isSet(_cnf.numLits(), false);
        for (const uint32_t & clauseId : _clauseStack)
        {
            for (const auto & litId : _cnf.getClause(clauseId).lits)
            {
                if (!isSet[std::abs(litId)])
                {
                    isSet[std::abs(litId)] = true;
                    _cnf.getLit(litId) = Lit(++counter, _posInTokenStack, _tokenStack[_posInTokenStack]);
                }
                if (counter == LitVector::maxNumSchroedinger())
                {
                    _tokenStack[++_posInTokenStack] = BranchToken(posInClauseStack);
                    counter = 0;
                }
            }
            ++posInClauseStack;
        }
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
        }
        else
        {
            // evaluate false
            if (!shiftLine)  // try get next line of token
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
        LitVector tmp(false);
        LitVector & curValue = _tokenStack[_posInTokenStack].litState;
        curValue = _vecMem.one();
        for (size_t i = _tokenStack[_posInTokenStack]; i < _tokenStack[_posInTokenStack + 1]; ++i)
        {
            if (curValue != LitVector::zero())
            {
                tmp = LitVector::zero();
                for (const int32_t & lit : _cnf.getClause(i))
                    tmp |= getValueOfLit(lit);
                curValue ^= tmp;
            }
            else
                return false;
        }

        return true;
    }

    bool shiftLine()
    {
        ClauseState & clauseState = _tokenStack[_posInTokenStack].clauseState;
        const auto & litVec = _tokenStack[_posInTokenStack].litState;

        while (++clauseState.posInVec < LitVector::size() && !litVec[clauseState.posInVec])
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

    const LitVector getValueOfLit(const int32_t & in)
    {
        Lit & tmpRef = _cnf.getLit(std::abs(in));
        if (tmpRef.tokenNum < _posInTokenStack)
            return ((_vecMem[tmpRef.memPos].get(tmpRef.clauseState->posInVec)) ? _vecMem.zero() : _vecMem.one());
        else
        {
            return _vecMem[(in < 0) ? -tmpRef.memPos : tmpRef.memPos];
        }

    }

};

#endif /* CNFSOLVER_HPP_ */
