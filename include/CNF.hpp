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

template<size_t n, typename base_vec>
class CNF {
public:

	typedef LitVec<n, base_vec> LitVector;

	CNF(const std::string & file) :
			_file(file) {
		readFromFile();
	}

	CNF(CNF<n, base_vec> && in) :
			_numLits(0) {
		std::swap(in._clauses, _clauses);
	}

	size_t numClauses() const {
		return _clauses.size();
	}

	size_t numLits() const {
		return _numLits;
	}

	Clause & getClause(const size_t & num) {
		return _clauses[num];
	}

   const Clause & getClause(const size_t & num) const {
      return _clauses[num];
   }

	uint32_t countNumLitsInClause() const
	{
		uint32_t res = 0;
		for(size_t i = 0;i<_clauses.size();++i)
			res += _clauses[i].lits.size();
		return res;
	}

private:
	size_t _numLits;
	std::string _file;
	std::vector<Clause> _clauses;
	static constexpr size_t _bufferSize = 2048;

	void readFromFile() {
		std::ifstream ins;
		std::cout << "Reading file " << _file << std::endl;
		ins.open(_file.c_str());
		char buffer[_bufferSize];
		while (ins.good()) {
			ins.getline(buffer, _bufferSize);
			if (buffer[0] == 'p')
				break;
		}
		std::string tmp(&(buffer[6]));
		std::stringstream ss(tmp);
		int32_t intTmp;
		ss >> _numLits;
		ss >> intTmp;
		std::cout << "NumLits: " << _numLits << std::endl << "NumClauses: " << intTmp << std::endl;
		_clauses.resize(intTmp);
		ss.str("");
		size_t clauseNum = 0;
		while (ins.good()) {
			ins >> intTmp;
			if (intTmp == 0)
				++clauseNum;
			else {
				_clauses[clauseNum].lits.push_back(intTmp);
// 				if (intTmp < 0)
// 					_lits[-intTmp].clauses.push_back(-clauseNum);
// 				else
// 					_lits[intTmp].clauses.push_back(clauseNum);
			}
		}
		ins.close();

	}
};

template<size_t n, typename base_vec>
constexpr size_t CNF<n,base_vec>::_bufferSize;

#endif /* CNF_HPP_ */
