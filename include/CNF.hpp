/*
 * CNF.hpp
 *
 *  Created on: 13.08.2016
 *      Author: hartung
 */

#ifndef CNF_HPP_
#define CNF_HPP_

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "Lit.hpp"
#include "LitVec.hpp"
#include "Clause.hpp"

enum Sign {
	IDENT, NEGAT
};

template<size_t n, typename base_vec>
class CNF {
public:

	typedef LitVec<n, base_vec> LitVector;

	CNF(const std::string & file) :
			_file(file), _bufferSize(1024) {
		readFromFile();
	}

	CNF(CNF<n, base_vec> && in) :
			_bufferSize(0) {
		std::swap(in._clauses, _clauses);
		std::swap(in._lits, _lits);
	}

	size_t numClauses() const {
		return _clauses.size();
	}

	size_t numLits() const {
		return _lits.size();
	}

	Clause & getClause(const size_t & num) {
		return _clauses[num];
	}

	Lit<LitVector> & getLit(const size_t & num) {
		return _lits[num];
	}

	uint32_t countNumLitsInClause() const
	{
		uint32_t res = 0;
		for(size_t i = 0;i<_clauses.size();++i)
			res += _clauses[i].lits.size();
		return res;
	}

	void writeSolution() {
		std::ofstream ofs;
		ofs.open((_file + ".result").c_str());
		for (size_t i = 1; i < _lits.size(); ++i)
			if (_lits[i].val[_lits[i].clauseState->posInVec])
				ofs << -i << " ";
			else
				ofs << i << " ";
		ofs.close();
	}

private:
	std::string _file;
	std::vector<Clause> _clauses;
	std::vector<Lit<LitVector>> _lits;
	size_t _bufferSize;

	void readFromFile() {
		std::ifstream ins;
		ins.open(_file.c_str());
		char buffer[_bufferSize];
		size_t pos;
		while (ins.good()) {
			ins.getline(buffer, _bufferSize);
			if (buffer[0] == 'p')
				break;
		}
		std::string tmp(&(buffer[6]));
		std::stringstream ss(buffer);
		uint32_t uintTmp;
		ss >> uintTmp;
		_lits.resize(uintTmp + 1);
		ss >> tmp;
		_clauses.resize(uintTmp);
		ss.str("");
		size_t clauseNum = 0;
		while (ins.good()) {
			ins >> uintTmp;
			if (uintTmp == 0)
				++clauseNum;
			else {
				_clauses[clauseNum].lits.push_back(uintTmp);
				if (uintTmp < 0)
					_lits[-uintTmp].clauses.push_back(-clauseNum);
				else
					_lits[uintTmp].clauses.push_back(clauseNum);
			}
		}
		ins.close();

	}
};

#endif /* CNF_HPP_ */
