#ifndef DEVURANDOMRNG_HPP
#define DEVURANDOMRNG_HPP

#include "RNG.hpp"

// This entropy source is the second most accurate and safe available. It is a "pseudo-interpolated quantum quality noise source".
// It is idetical to DevRandom, except that it uses pseudo random numbers when the entropy is empty. Its characteristics are thus very comparable to DevRandom (slow, low scalability etc)
// and the quality of its output is lower. However it solves the blocking I/O issue with DevRandom

class DevURandomRNG: public RNG {
public:
	static FILE *fh;
public:

	//Ctor
	DevURandomRNG();
	//Dtor
	~DevURandomRNG();

	//Return true if this source is dependant on any pseudo rng
	inline bool hasPseudo();
	//Generates a random number on [0,0xffffffff]-interval
	//This is the main generator function that all the others are based on
	uint32_t generateInt32(void);
	//Initializes entropysouce with a seed if needed
	void init(uint32_t s);
};

#endif // DEVURANDOMRNG_HPP
