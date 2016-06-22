#ifndef MERSENNEPRNG_HPP
#define MERSENNEPRNG_HPP

#include "RNG.hpp"

//Period parameters
#define MT_N 624
#define MT_M 397
//Constant vector a
#define MT_MATRIX_A 0x9908b0dfUL
//Most significant w-r bits
#define MT_UPPER_MASK 0x80000000UL
//Least significant r bits
#define MT_LOWER_MASK 0x7fffffffUL

class MersennePRNG: public RNG {

public:

	//Constructor
	MersennePRNG();

	//Return true if this source is dependant on any pseudo rng
	inline bool hasPseudo();

	//Generates a random number on [0,0xffffffff]-interval
	//This is the main generator function that all the others are based on
	quint32 generateInt32(void);

	//Initializes entropysouce with a seed if needed (mt[MT_N])
	void init(quint32 s);

	void init();

	//Initialize by an array with array-length init_key is the array for initializing keys key_length is its length
	virtual void init(quint32 init_key[], size_t key_length);

private:
	//The array for the state vector
	quint32 mt[MT_N];
	//mti==N+1 means mt[N] is not initialized
	qint32 mti;

};

#endif //MERSENNEPRNG_HPP
