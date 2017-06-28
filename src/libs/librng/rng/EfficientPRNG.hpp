#ifndef EFFICIENTPRNG_HPP
#define EFFICIENTPRNG_HPP

#include "RNG.hpp"

// Main criteria is speed. Its neither accurate, nor secure. It probably isnt
// balanced or normalized in any depth or dimension. It just spews lots of
// barely random data in a really short time.

class EfficientPRNG: public RNG {
private:
	uint32_t noise, carry, seed;

public:
	//Ctor
	EfficientPRNG();
	//Dtor
	~EfficientPRNG();

	//Return true if this source is dependant on any pseudo rng
	inline bool hasPseudo();
	//Generates a random number on [0,0xffffffff]-interval
	//This is the main generator function that all the others are based on
	uint32_t generateInt32(void);
	//Initializes entropysouce with a seed if needed
	void init(uint32_t s);

};

#endif // EFFICIENTPRNG_HPP
