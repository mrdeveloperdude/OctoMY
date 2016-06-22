/*
 * TausPRNG.hpp
 *
 *  Created on: Jan 7, 2013
 *      Author: lennart
 */

#ifndef TAUSPRNG_HPP
#define TAUSPRNG_HPP

#include "RNG.hpp"

using namespace std;

class TausPRNG: public RNG {

public:

	//Constructor
	TausPRNG();
	//Destructor
	virtual ~TausPRNG();

	//Return true if this source is dependant on any pseudo rng
	virtual inline bool hasPseudo() {
		return true;
	}

	//Generates a random number on [0,0xffffffff]-interval
	//This is the main generator function that all the others are based on
	virtual quint32 generateInt32(void) {
		s1 = ((s1 & -2) << 12) ^ (((s1 << 13) ^ s1) >> 19);
		s2 = ((s2 & -8) << 4) ^ (((s2 << 2) ^ s2) >> 25);
		s3 = ((s3 & -16) << 17) ^ (((s3 << 3) ^ s3) >> 11);
		return s1 ^ s2 ^ s3;
	}

	//Initializes entropysouce with a seed if needed (mt[MT_N])
	virtual void init(quint32 s) {
		s1 = 1243598713U ^ s;
		if (s1 < 2) {
			s1 = 1243598713U;
		}
		s2 = 3093459404U ^ s;
		if (s2 < 8) {
			s2 = 3093459404U;
		}
		s3 = 1821928721U ^ s;
		if (s3 < 16) {
			s3 = 1821928721U;
		}
	}

private:
	//The array for the state vector
	uint32_t s1, s2, s3;
};

#endif //TAUSPRNG_HPP
