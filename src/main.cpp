/*
 * main.cpp
 *
 *  Created on: 12.08.2016
 *      Author: hartung
 */

#include "Vec.hpp"
#include "LitVec.hpp"


int main()
{
	LitVec<1,Vec> v1, v2, v3;
	v1 |= v2 ^ v3;
	return 0;
}


