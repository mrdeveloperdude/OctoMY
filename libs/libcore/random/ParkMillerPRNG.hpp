#ifndef PARKMILLERPRNG_HPP
#define PARKMILLERPRNG_HPP

#include "RNG.hpp"

// This is a good all around prng. It holds very little state, but still provides resonable speed and accuracy

class ParkMillerPRNG: public RNG {
	public:
		ParkMillerPRNG();
		~ParkMillerPRNG();

	public:
		// Return true if this source is dependant on any pseudo rng
		inline bool hasPseudo();

		// Generates a random number on [0,0xffffffff]-interval
		// This is the main generator function that all the others are based on
		uint32_t generateInt32(void);

		//Initializes entropysouce with a seed if needed
		void init(uint32_t s);
};


#endif // PARKMILLERPRNG_HPP
