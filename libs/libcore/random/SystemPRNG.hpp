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
	virtual quint32 generateInt32(void);

	//Initializes entropysouce with a seed if needed
	virtual void init(quint32 s);
};


#endif // SYSTEMPRNG_HPP
