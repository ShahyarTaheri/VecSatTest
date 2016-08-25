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

template<size_t n, typename base_vec>
class CNFSolver {
private:
	typedef LitVec<n, base_vec> LitVector;

	struct BranchToken {
		ClauseState clauseState;
		LitVector litState;

                BranchToken()
                {
                    litState = LitVector(false);
                }
                
		BranchToken(const size_t & numCurLits, const size_t & numCurClauses) :
				litState(false) {
			clauseState.posLit = numCurLits;
			clauseState.posClause = numCurClauses;
			clauseState.posInVec = 0;
		}
	};

public:
	CNFSolver(CNF<n, base_vec> & cnf) :
                        _posInLitStack(0), _posInClauseStack(0), _posInTokenStack(
					0), _numSchroedinger(0), _setSchroedinger(true), _backStep(false), _solutionFound(false), _cnf(cnf) {
		init();
	}

	bool solve() {
		while (solveStep()) {
		}

		return _solutionFound;
	}

	void init() {
		setUpMemory();
		setUpClauseOrder();
		setUpLitOrder();
	}

	bool hasSolution() const {
		return _solutionFound;
	}

	void dumpBytesUsed()
	{
		uint64_t numBytes = 0;
		numBytes += (sizeof(LitVector) + LitVector::size() + sizeof(uint32_t)) * _litStack.size();
		numBytes += (sizeof(Clause) + sizeof(uint32_t)) * _litStack.size() + _cnf.countNumLitsInClause() * sizeof(int32_t);
		numBytes += sizeof(BranchToken) * _tokenStack.size();
		std::cout << "Allocating not less than " << numBytes/(1024*1024) << " MB" << std::endl;
	}

private:

	size_t _posInLitStack;
	size_t _posInClauseStack;
	size_t _posInTokenStack;

	size_t _numSchroedinger;
	bool _setSchroedinger;
	bool _backStep;

	bool _solutionFound;

	LitVector _curValue;

	CNF<n, base_vec> & _cnf;
	std::vector<uint32_t> _clauseStack;
	std::vector<uint32_t> _litStack;
	std::vector<BranchToken> _tokenStack;

	void setUpMemory() {
		_clauseStack.resize(_cnf.numClauses(), 0);
		_litStack.resize(_cnf.numLits(), 0);
		_tokenStack.resize(
				std::ceil(
						((double) _cnf.numLits())
								/ LitVector::maxNumSchroedinger()));
	}

	void setUpClauseOrder() {
		for (size_t i = 0; i < _cnf.numClauses(); ++i)
			_clauseStack[i] = i;
	}

	void setUpLitOrder() {
		std::vector<bool> isSet(_litStack.size(), false);
		size_t counter = 1;
		for (const uint32_t & clauseId : _clauseStack)
			for (const auto & litId : _cnf.getClause(clauseId).lits)
				if (!isSet[std::abs(litId)]) {
					isSet[std::abs(litId)] = true;
					_litStack[_posInLitStack++] = std::abs(litId);
					_cnf.getLit(litId).clauseState = &_tokenStack[_posInTokenStack].clauseState;
					if (counter % LitVector::maxNumSchroedinger() == 0)
						++_posInTokenStack;
				}
		_posInLitStack = 0;
		_posInTokenStack = 0;
	}

	bool solveStep() {
		bool res = true;
		size_t startClausePos = _posInClauseStack;
		size_t startLitPos = _posInLitStack;
		if (!_backStep && evaluateClauses()) // check if current setting still satisfies cnf
				{
			if(_posInLitStack < _litStack.size())
				pushToken(startLitPos,startClausePos);
		else
		{
			_solutionFound = true;
			res = false;
		}
	} else {
		_backStep = false;
		while (_tokenStack[_posInTokenStack].clauseState.posInVec
				< LitVector::size()
				&& !_tokenStack[_posInTokenStack].litState.get(++_tokenStack[_posInTokenStack].clauseState.posInVec))
			;
		if (_tokenStack[_posInTokenStack].clauseState.posInVec
				< LitVector::size()) // choose new line from schroedinger setting of this token
				{
			_setSchroedinger = false;
			_posInLitStack = startLitPos;
			_posInClauseStack = startClausePos;
		} else //remove token
		{
			_setSchroedinger = true; //TODO token is not removed
			popToken();
			_backStep = true;
		}
	}
	return res;
}

bool evaluateClauses() // TODO code duplication
{
	LitVector tmp(false);
	_curValue = LitVector::one();
	_numSchroedinger = 0;
	if (_setSchroedinger) {
		while (_numSchroedinger < LitVector::maxNumSchroedinger()) {
			if (_curValue != LitVector::zero()) {
				tmp = LitVector::zero();
				auto & clause = _cnf.getClause(_posInClauseStack);
				for (size_t i = 0;
						i < clause.lits.size()
								&& _numSchroedinger
										< LitVector::maxNumSchroedinger(); ++i)
					tmp |= getValueOfLitSchroedingerAssign(clause.lits[i],
							_numSchroedinger);
				++_posInClauseStack;
			} else
				return false;
		}
	} else {
		while (true) {
			if (_curValue != LitVector::zero()) {
				tmp = LitVector::zero();
				auto & clause = _cnf.getClause(_posInClauseStack);
				for (size_t i = 0;
						i < clause.lits.size()
								&& _numSchroedinger
										< LitVector::maxNumSchroedinger(); ++i)
					tmp |= getValueOfLit(clause.lits[i], _numSchroedinger);
				_curValue ^= tmp;
				++_posInClauseStack;
			} else
				return false;
		}
	}
	return true;
}

void pushToken(const size_t & startLitPos, const size_t & startClausePos) {
	_tokenStack[_posInTokenStack].litState = _curValue;
	_tokenStack[_posInTokenStack].clauseState.posLit = startLitPos;
	_tokenStack[_posInTokenStack].clauseState.posClause = startClausePos;
	_tokenStack[_posInTokenStack].clauseState.posInVec = 0;
	for(size_t i=0;i<LitVector::maxNumSchroedinger();++i)
		_cnf.getLit(startLitPos+i).simpleVal = true;
	++_posInTokenStack;
	_setSchroedinger = true;
}

void popToken() {
	for (size_t i = _tokenStack[_posInTokenStack].clauseState.posLit;
			i < _posInLitStack; ++i)
		_cnf.getLit(_litStack[i]).simpleVal = false;
	--_posInTokenStack;
}

/**
 * Returns the LitVector of the given lit. If a new assignment has be done, out will be incremented
 */
const LitVector getValueOfLitSchroedingerAssign(const int32_t & in,
		size_t & out) // TODO code duplication
		{
	if (in < 0) {
		Lit<LitVector> & tmpRef = _cnf.getLit(-in);
		if (tmpRef.simpleVal)
			return ((tmpRef.val.get(tmpRef.clauseState->posInVec) == false) ?
					LitVector::one() : LitVector::zero());
		else {
			tmpRef.val = LitVector::schroedinger(out++);
			_litStack[_posInLitStack++] = -in;
			return -tmpRef.val;
		}
	} else {
		Lit<LitVector> & tmpRef = _cnf.getLit(in);
		if (tmpRef.simpleVal)
			return ((tmpRef.val.get(tmpRef.clauseState->posInVec) == true) ?
					LitVector::one() : LitVector::zero());
		else {
			tmpRef.val = LitVector::schroedinger(out++);
			_litStack[_posInLitStack++] = in;
			return tmpRef.val;
		}
	}
}

const LitVector getValueOfLit(const int32_t & in, size_t & out) // TODO code duplication
		{
	if (in < 0) {
		Lit<LitVector> & tmpRef = _cnf.getLit(-in);
		if (tmpRef.simpleVal)
			return ((tmpRef.val.get(tmpRef.clauseState->posInVec) == false) ?
					LitVector::one() : LitVector::zero());
		else {
			_posInLitStack++;
			return -tmpRef.val;
		}
	} else {
		Lit<LitVector> & tmpRef = _cnf.getLit(in);
		if (tmpRef.simpleVal)
			return ((tmpRef.val.get(tmpRef.clauseState->posInVec) == true) ?
					LitVector::one() : LitVector::zero());
		else {
			_posInLitStack++;
			return tmpRef.val;
		}
	}
}

};

#endif /* CNFSOLVER_HPP_ */
