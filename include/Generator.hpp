/*
 * Generator.hpp
 *
 *  Created on: 14.08.2016
 *      Author: hartung
 */

#ifndef GENERATOR_HPP_
#define GENERATOR_HPP_

template<size_t n, typename base_vec>
class Generator {
public:
	Generator(const size_t numLits, const size_t numClause,
			const size_t averageClauseLength, const size_t deviation) :
			_numLits(numLits), _numClause(numClause), _averageClauseLength(
					averageClauseLength), _deviation(deviation) {

	}

	CNF<n, base_vec> generate() const {

	}

private:
	size_t _numLits;
	size_t _numClause;
	size_t _averageClauseLength;
	size_t _deviation;
};

#endif /* GENERATOR_HPP_ */
