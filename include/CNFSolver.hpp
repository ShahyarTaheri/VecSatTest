/*
 * CNFSolver.hpp
 *
 *  Created on: 14.08.2016
 *      Author: hartung
 */

#ifndef CNFSOLVER_HPP_
#define CNFSOLVER_HPP_

#include <vector>
#include <stack>
#include <cstdint>

#include "CNF.hpp"
#include "LitSetGraph.hpp"


template<size_t n, typename base_vec>
class CNFSolver {
private:
    typedef LitVec<n,base_vec> LitVector;
    
    struct BranchToken
    {
        ClauseState clauseState;
        LitVector litState;
    };
    
public:
	CNFSolver(CNF<n, base_vec> && cnf) 
        :   _cnf(std::move(cnf))
        {
            _tokens.push(BranchToken());
            _evaluatedClauses.reserve(_cnf.numClauses());
            _setLits.reserve(_cnf.numLits());
            _newSetLits.resize(LitVector::maxNumSchroedinger());
	}

	bool solve(const size_t & clauseNum = 0) {
		if(_posClauses.empty())
                    _posClauses.push_back(clauseNum);
                while(solveStep())
                {
                }

            return _solutionFound;
	}
private:
	CNF<n, base_vec> _cnf;
	std::vector<uint32_t> _evaluatedClauses;
	std::vector<uint32_t> _setLits;
        std::stack<BranchToken> _tokens;
        
        std::vector<uint32_t> _newSetLits; 
        std::vector<uint32_t> _posClauses;
        std::vector<uint32_t> _evaluatableClauses;
        
        bool _solutionFound;
        

        bool trySelectClauses()
        {
            BranchToken & token = _tokens.top();
            while(!token.state[token.posInVec] && token.posInVec < LitVector::size())
                ++token.posInVec;
            if(token.posInVec >= LitVector::size())
                return false;
        }
        
        /*
         * Function sets the lits in _newSetLits to all possible combinations TODO
         */
        void setLits()
        {
            for(size_t i=0;i<_newSetLits.size();++i)
                _cnf.getLit(_newSetLits[i]).assignSchroedinger(i);
        }
        
        /**
         * Evaluates all clauses in _evaluatableClauses
         * @return the solutionen of the evaluated clauses
         */
        LitVector evaluateClauses()
        {
            LitVector res(LitVector::one()), tmp(false);
            for(const uint32_t & clauseId : _evaluatableClauses)
            {
                tmp.setOne();
                for(const int32_t & litId : _cnf.getClause(clauseId)._lits)
                    tmp |= getValueOfLit(litId);
                res ^= tmp;
            }
            return res;
        }
        
        bool solveStep()
        {
            bool res = true;
            if(trySelectClauses())
            {
                setLits();
                _tokens.top().state = evaluateClauses();
                res = _tokens.top() != LitVector::zero();
            }
            if(res)
            {
                if(_evaluatedClauses.size() + _evaluatableClauses.size() >= _cnf.getNumClauses())
                {
                    createNewToken();
                }
                else
                {
                    _solutionFound = true;
                    return false;
                }
            }
            else // token has no poses
                if(_tokens.size() > 1)
                {
                    removeToken();
                    res = true;
                }
            return res;
        }
            
        void createNewToken()
        {
            for(const unint32_t & clauseId : _evaluatableClauses)
                _cnf.getClause(clauseId).clauseState = &(tokens.top());
            _evaluatedClauses.insert(_evaluatedClauses.end(),_evaluatableClauses.begin(),_evaluatableClauses.end());
            _evaluatableClauses.resize(0);
            _tokens.push(BranchToken(_setLits.size(),_posClauses.size(),0,LitVector(true)));
            for(const unint32_t & litId : _newSetLits)
                _cnf.getLit(litId).clauseState = &(tokens.top());
            _setLits.insert(_setLits.end(),_newSetLits.begin(), _newSetLits.end());
            _newSetLits.resize(0);
        }
        
        void removeToken()
        {
            for(size_t i=_tokens.top().clauseState.posClause;i<_evaluatedClauses.size();++i)
                _cnf.getClause(_evaluatedClauses[i]).clauseState = nullptr;
            for(size_t i=_tokens.top().clauseState.posLit;i<_setLits.size();++i)
                _cnf.getLit(_setLits[i]).clauseState = nullptr;
            _setLits.resize(_tokens.top().clauseState.posLit);
            _evaluatedClauses.resize(_tokens.top().clauseState.posClause);
            
            _tokens.pop();
            
        }
        
        const LitVector & getValueOfLit(const int32_t & in)
        {
            if(in < 0)
            {
                Lit<LitVector> & tmpRef = _cnf.getLit(-in);
                if(tmpRef.simpleVal)
                    return ((tmpRef.val[tmpRef.clauseState->posInVec]==0) ? LitVector::one() : LitVector::zero();
                else
                    return -tmpRef.val;
            }
            else
            {
                Lit<LitVector> & tmpRef = _cnf.getLit(in);
                if(tmpRef.simpleVal)
                    return ((tmpRef.val[tmpRef.clauseState->posInVec]==1) ? LitVector::one() : LitVector::zero();
                else
                    return tmpRef.val;
            }
        }


};

#endif /* CNFSOLVER_HPP_ */
