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
              _zeroFuture(1000),
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
        std::cout << "numConflicts: " << _zeroCounter << std::endl;
        return _solutionFound;
    }

    void init()
    {
        _vecMem.init(_cnf->numLits());
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
        for (size_t i = 0; i < _tokenStack.size() - 1; ++i)
            std::cout << i << ". " << _tokenStack[i + 1].clauseState.posClause - _tokenStack[i].clauseState.posClause << " range ("
                    << _tokenStack[i].clauseState.posClause << "," << _tokenStack[i + 1].clauseState.posClause << ") to evaluate" << std::endl;
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
        size_t numTokens = std::ceil(((double) _cnf->numLits()) / LitVector::maxNumSchroedinger()) + 1;
        _tokenStack.resize(numTokens);
        std::cout << "NumTokens: " << _tokenStack.size() << std::endl;
        _lits.resize(_cnf->numLits() + 1);
    }

    void setUpClauseOrder()
    {
        for (size_t i = 0; i < _cnf->numClauses(); ++i)
            _clauseStack[i] = i;

        /*unsigned seed = std::time(0);
        //seed = 1472635084;
        std::cout << "seed = " << seed << std::endl;
        std::srand(seed);
        random_shuffle(_clauseStack.begin(), _clauseStack.end());*/
    }

    void setUpLitOrder()
    {
        size_t schroedId = 0, setLits = 0;
        std::vector<bool> isSet(_cnf->numLits()+1, false);
        size_t litsTouched;
        for (size_t i = 0; i < _clauseStack.size(); ++i)
        {
            const Clause & clause = _cnf->getClause(_clauseStack[i]);
            litsTouched = 0;
            for (size_t j = 0; j < clause.lits.size(); ++j)
            {
                ++litsTouched;
                const uint32_t & litId = std::abs(clause.lits[j]);
                if (!isSet[litId])
                {
                    if (schroedId == LitVector::maxNumSchroedinger())
                    {
                        _tokenStack[++_posInTokenStack] = BranchToken(i);
                        std::cout << _posInTokenStack << ". " << _tokenStack[_posInTokenStack].clauseState.posClause << std::endl;
                        schroedId = 0;
                    }
                    isSet[litId] = true;
                    if (setLits >= _cnf->numLits() - _vecMem.numAdditionals())
                        _lits[litId] = Lit((++schroedId) + LitVector::maxNumSchroedinger(), _posInTokenStack, _tokenStack[_posInTokenStack].clauseState);
                    else
                        _lits[litId] = Lit(++schroedId, _posInTokenStack, _tokenStack[_posInTokenStack].clauseState);
                    ++setLits;
                }
            }
        }
        _tokenStack.back().clauseState.posClause = _clauseStack.size();

        _posInTokenStack = 0;
    }

    bool solveStep()
    {
        bool res = true;
        if (_zeroCounter > _zeroFuture)
        {
            _zeroFuture = _zeroCounter + 10000;
            std::cout << "Conflics per sec: " << (_zeroCounter / std::floor((double) (clock() - _startTime) / CLOCKS_PER_SEC)) << std::endl
                      << "lowest backstep: " << _lowestBackstepLvl << " highest: " << _highestLvl << std::endl;
        }
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
            if (!findValidLineOrToken())
            {
                res = false;
                _solutionFound = false;
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
                const Clause & clause = _cnf->getClause(_clauseStack[i]);
                for (const int32_t & lit : clause.lits)
                    tmp |= getValueOfLit(lit);

                curValue ^= tmp;
                /*if (true)
                 {
                 for (const int32_t & lit : _cnf->getClause(i).lits)
                 std::cout << "lit:" << lit << " tokenNum:" << _lits[std::abs(lit)].tokenNum << "mem:" << _lits[std::abs(lit)].memPos << std::endl;
                 }*/
                //std::cout << "ones: " << curValue.countOnes() << std::endl;
            }
            else
                return false;
        }

        return curValue != _vecMem.zero();
    }

    bool findValidLineOrToken()
    {
        bool res = false;
        if (_posInTokenStack > 0)
        {
            for (size_t i = _posInTokenStack-1;i!=0 && !res;--i)
            {
                _tokenStack[i+1].clauseState.posInVec = 0;
                BranchToken & token = _tokenStack[i];
                for(size_t j=token.clauseState.posInVec+1;j<LitVector::size();++j)
                {
                    if(token.litState.get(j))
                    {
                        _posInTokenStack = i+1;
                        _lowestBackstepLvl = std::min(_posInTokenStack,_lowestBackstepLvl);
                        _highestLvl = std::max(_highestLvl,_posInTokenStack);
                        token.clauseState.posInVec = j;
                        res = true;
                        //std::cout << "Setting back to Stack(" << _posInTokenStack << "), Line(" << j << ")" << std::endl;
                        break;
                    }
                    else
                        ++_zeroCounter;
                }
            }
        }
        return res;
    }

    bool pushToken()
    {
        if (++_posInTokenStack < _tokenStack.size() - 1)
            return true;
        else
            return false;
    }

    const LitVector & getValueOfLit(const int32_t & in)
    {
        Lit & tmpRef = _lits[std::abs(in)];
        int32_t index = ((in < 0) ? -tmpRef.memPos : tmpRef.memPos);
        if (tmpRef.tokenNum < _posInTokenStack)
            return ((_vecMem[index].get(tmpRef.clauseState->posInVec)) ? _vecMem.one() : _vecMem.zero());
        else if (tmpRef.tokenNum == _posInTokenStack)
        {
            return _vecMem[index];
        }
        else
            throw std::runtime_error("Lits are in a wrong order");
    }

    void optimizeClauseOrder(bool optimize = true)
    {
        //TODO crashes on ~/hpsv/sat_test_13/SC13_submissions/heule--randomMUS/MUS-v300-1.cnf
        std::vector<std::list<size_t> > clauseNumToTokenNum(_tokenStack.size());
        size_t numMoves = 0;
        for (size_t i = 0; i < _tokenStack.size() - 1; ++i)
        {
            for (size_t j = _tokenStack[i].clauseState.posClause; j < _tokenStack[i + 1].clauseState.posClause; ++j)
            {
                size_t maxToken = 0;
                const Clause & clause = _cnf->getClause(_clauseStack[j]);
                for (size_t k=0;k < clause.lits.size();++k)
                {
                    int32_t lit = clause.lits[k];
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
                for (auto it=cList.begin();it!=cList.end();++it)
                {
                    _clauseStack[clauseIt++] = *it;
                }
            }
        }
        //else
        //   exit(0);
    }

};

#endif /* CNFSOLVER_HPP_ */
