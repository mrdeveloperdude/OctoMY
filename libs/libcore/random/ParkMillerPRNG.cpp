#include "ParkMillerPRNG.hpp"

//This is a good all around prng. It holds very little state, but still provides resonable speed and accuracy

ParkMillerPRNG::~ParkMillerPRNG() {
}


ParkMillerPRNG::ParkMillerPRNG() {
}

// Return true if this source is dependant on any pseudo rng
inline bool ParkMillerPRNG::hasPseudo() {
	return true;
}

// Generates a random number on [0,0xffffffff]-interval
// This is the main generator function that all the others are based on
uint32_t ParkMillerPRNG::generateInt32(void) {
	return 0;
}

//Initializes entropysouce with a seed if needed
void ParkMillerPRNG::init(uint32_t ) {

}
