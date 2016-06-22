#include "ParkMillerPRNG.hpp"

//This is a good all around prng. It holds very little state, but still provides resonable speed and accuracy

ParkMillerPRNG::~ParkMillerPRNG() {
}


ParkMillerPRNG::ParkMillerPRNG()
	: RNG("ParkMillerPRNG")
	, SEED(1)
{
}

// Return true if this source is dependant on any pseudo rng
inline bool ParkMillerPRNG::hasPseudo() {
	return true;
}

// Generates a random number on [0,0xffffffff]-interval
// This is the main generator function that all the others are based on
quint32 ParkMillerPRNG::generateInt32(void) {
	quint32 lo, hi;
	lo = 16807 * (SEED & 0xFFFF);
	hi = 16807 * (SEED >> 16);
	lo += (hi & 0x7FFF) << 16;
	lo += hi >> 15;
	if (lo > 0x7FFFFFFF) lo -= 0x7FFFFFFF;
	return ( SEED = lo );
}


// Initializes the (P)RNG wihtout seed (those that need a seed will default
//   to lowest valid positive seed, such as 0 or 1, or the default specified
//   for that particular generator)
void ParkMillerPRNG::init() {
	init(1);
}

//Initializes entropysouce with a seed if needed
void ParkMillerPRNG::init(quint32 new_seed ) {
	if ( 0<(new_seed) && (new_seed) < 2147483647 ) {
		SEED = new_seed;
	}

}
