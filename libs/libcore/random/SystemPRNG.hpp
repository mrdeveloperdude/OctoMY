#ifndef SYSTEMPRNG_HPP
#define SYSTEMPRNG_HPP

#include "RNG.hpp"
//This simply uses the standard entropy source present in the system libraries (libc rand())

class SystemPRNG: public RNG {
public:
	//Ctor
	SystemPRNG();
	//Dtor
	~SystemPRNG();

	//Return true if this source is dependant on any pseudo rng
	inline bool hasPseudo();
	//Generates a random number on [0,0xffffffff]-interval
	//This is the main generator function that all the others are based on
	uint32_t generateInt32(void);
	//Initializes entropysouce with a seed if needed
	void init(uint32_t s);
};


#endif // SYSTEMPRNG_HPP
