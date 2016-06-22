#include "EfficientPRNG.hpp"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////
///////   Efficient Entropy Source
///////

//Main criteria is speed. Its neither accurate, nor secure. It probably isnt balanced or normalized in any depth or dimension. It just spews lots of barely random data in a really short time.

//Dtor
EfficientPRNG::~EfficientPRNG() {
}

//Ctor
EfficientPRNG::EfficientPRNG()
	: RNG("EfficientPRNG")
	, noise(0)
	, carry(0)
	, seed(1337)
{
	//Skip first result which is always 0
	generateInt32();
	generateInt32();
	generateInt32();
}

//Return true if this source is dependant on any pseudo rng
bool EfficientPRNG::hasPseudo() {
	return true;
}

//Generates a random number on [0,0xffffffff]-interval
//This is the main generator function that all the others are based on
inline uint32_t EfficientPRNG::generateInt32(void) {
	//debug("NOISE WAS: %d, %d, %d\n", noise, seed, carry);
	/*
	 seed >>= 1;
	 seed |= (carry << 30);
	 noise = seed;
	 noise >>= 3;
	 noise ^= seed;
	 carry = noise & 1;
	 noise >>= 1;
	 */

	noise = seed;
	noise >>= 3;
	noise ^= seed;
	carry = noise & 1;
	noise >>= 1;
	seed >>= 1;
	seed |= (carry << 30);
	//            noise &= 0xFF;

	return noise;
}

//Initializes entropysouce with a seed if needed
void EfficientPRNG::init(uint32_t s) {
	if (0 == s) s = 1;
	seed = s;
}
