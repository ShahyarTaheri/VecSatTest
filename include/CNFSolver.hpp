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
#include <stack>
#include <cstdint>

#include "CNF.hpp"


template<size_t n, typename base_vec>
class CNFSolver {
private:
    typedef LitVec<n,base_vec> LitVector;
    
    struct BranchToken
    {
        ClauseState clauseState;
        LitVector litState;
        
        BranchToken(const size_t & numCurLits, const size_t & numCurClauses)
            : litState(false)
        {
            clauseState.posLit = numCurLits;
            clauseState.posClause = numCurClauses;
            clauseState.posInVec = 0; 
        }
    };
    
public:
	CNFSolver(CNF<n, base_vec> && cnf) 
        :   _cnf(std::move(cnf)), _posLitsComplete(false), _posClausesComplete(false), _solutionFound(false)
        {
            _tokens.push(BranchToken());
            _evaluatedClauses.reserve(_cnf.numClauses());
            _setLits.reserve(_cnf.numLits());
            _newSetLits.resize(LitVector::maxNumSchroedinger());
	}

	bool solve(const size_t & clauseNum = 0) {
		if(_posClauses.empty())
                    _posClauses.push_back(clauseNum);
                updatePosLits();
                while(solveStep())
                {
                }

            return _solutionFound;
	}
	
	bool hasSolution() const
	{
            return _solutionFound;
        }
private:
	CNF<n, base_vec> _cnf;
	std::vector<uint32_t> _evaluatedClauses;
	std::vector<uint32_t> _setLits;
        std::stack<BranchToken> _tokens;
        
        std::vector<uint32_t> _newSetLits; 
        std::list<uint32_t> _posLits;
        std::list<uint32_t> _posClauses;
        std::vector<uint32_t> _evaluatableClauses;
        
        
        bool _posLitsComplete;
        bool _posClausesComplete;
        
        bool _solutionFound;
        

        bool trySelectClauses()
        {
            BranchToken & token = _tokens.top();
            while(!token.state[token.clauseState.posInVec] && token.clauseState.posInVec < LitVector::size())
                ++token.clauseState.posInVec;
            if(token.clauseState.posInVec >= LitVector::size())
                return false;
            
            if(_posLits.size() < LitVector::maxNumSchroedinger())
            {
                //try to add lits from pos clauses and pos lits
                if(!tryAddPosLits())
                    throw std::runtime_error("ui, solver couldn't add pos lits");
            }
            _newSetLits.resize(std::min(_posLits.size(),LitVector::maxNumSchroedinger()));
            auto it = _posLits.begin();
            for(size_t i=0;i<_newSetLits.size();++i, ++it)
                _newSetLits[i] = *it;
            _posLits.erase(_posLits.begin(),++it);
            
            std::list<std::list<uint32_t>::iterator> toRemove;
            for(auto it = _posClauses.begin();it!=_posClauses.end();++it)
            {
                Clause & c = *it;
                
            }
            
        }
        
        bool tryAddPosLits()
        {
            if(_posLitsComplete)
                if(!_posClausesComplete)
                    updatePosClauses();
            updatePosLits();
            return _posLits.size() > 0;
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
                tmp.setZero();
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
                _tokens.top().litState = evaluateClauses();
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
            for(const uint32_t & clauseId : _evaluatableClauses)
                _cnf.getClause(clauseId).clauseState = &(_tokens.top());
            _evaluatedClauses.insert(_evaluatedClauses.end(),_evaluatableClauses.begin(),_evaluatableClauses.end());
            _evaluatableClauses.resize(0);
            _tokens.push(BranchToken(_setLits.size(),_posClauses.size(),0));
            for(const uint32_t & litId : _newSetLits)
                _cnf.getLit(litId).clauseState = &(_tokens.top());
            _setLits.insert(_setLits.end(),_newSetLits.begin(), _newSetLits.end());
        }
        
        void removeToken()
        {
            for(size_t i=_tokens.top().clauseState.posClause;i<_evaluatedClauses.size();++i)
                _cnf.getClause(_evaluatedClauses[i]).unset();
            for(size_t i=_tokens.top().clauseState.posLit;i<_setLits.size();++i)
                _cnf.getLit(_setLits[i]).unset();
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
                    return ((tmpRef.val[tmpRef.clauseState->posInVec]==0) ? LitVector::one() : LitVector::zero());
                else
                    return -tmpRef.val;
            }
            else
            {
                Lit<LitVector> & tmpRef = _cnf.getLit(in);
                if(tmpRef.simpleVal)
                    return ((tmpRef.val[tmpRef.clauseState->posInVec]==1) ? LitVector::one() : LitVector::zero());
                else
                    return tmpRef.val;
            }
        }
        
        void updatePosLits()
        {
            bool added = false;
            bool remove;
            bool lastSign;
            unsigned short numUntouched;
            std::list<std::list<uint32_t>::iterator> toRemove;
            for(auto it = _posClauses.begin();it!=_posClauses.end();++it)
            {
                const uint32_t & clauseId = *it;
                numUntouched = 0;
                remove = false;
                for(const int32_t & litId : _cnf.getClause(clauseId))
                {
                    uint32_t absId = std::abs(litId);
                    if(!_cnf.getLit(absId).touched)
                    {
                        _cnf.getLit(absId).touched = true;
                        _posLits.push_back(absId);
                        added = true;
                        ++numUntouched;
                        if(litId < 0)
                            lastSign = false;
                        else
                            lastSign = true;
                    }
                    else if(_cnf.getLit(absId).clauseState != nullptr) // Clause is already satisfied
                    {
                        bool isPos = _cnf.getLit(absId).val[_cnf.getLit(absId).clauseState->posInVec];
                        bool hasPosSign = (litId >= 0);
                        
                        if((isPos && hasPosSign) || !(hasPosSign || isPos))
                            _evaluatedClauses.push_back(clauseId);
                        remove = true;
                    }
                }
                if(numUntouched == 1)
                {
                    // we can propagate
                    
                    _setLits.push_back(_posLits.back());
                    _cnf.getLit(_setLits.back()).val = ((lastSign) ? LitVector::one() : LitVector::zero());
                    _posLits.resize(_posLits.size()-1);
                    _evaluatedClauses.push_back(clauseId);
                    remove = true;
                    toRemove.push_back(it);
                }
                if(remove)
                    toRemove.push_back(it);
                if(added)
                {
                    _posLitsComplete = true;
                    _posClausesComplete = false;
                }
            }
            for(auto del : toRemove)
                _posClauses.erase(del);
        }
        
        void updatePosClauses()
        {
            bool added = false;
            
            for(const uint32_t & litId : _posLits)
                for(const int32_t & clauseId : _cnf.getLit(litId).clauses)
                {
                    if(!_cnf.getClause(std::abs(clauseId)).touched)
                    {
                        _cnf.getClause(std::abs(clauseId)).touched = true;
                        _posClauses.push_back(std::abs(clauseId));
                        added = true;
                    }
                }
            
            if(added)
            {
                _posLitsComplete = false;
                _posClausesComplete = true;
            }
            
        }


};

#endif /* CNFSOLVER_HPP_ */
