#ifndef DEVRANDOMRNG_HPP
#define DEVRANDOMRNG_HPP

#include "RNG.hpp"

//This entropy source is the most accurate and safe available. It is not a pseudo but rather depends on the kernel's "quantum quality noise source".
//It alto happens to be the slowest, least scalable and most cumbersome to use. Worst case scenario

class DevRandomRNG: public RNG {
	public:
		static FILE *fh;

	public:
		//Ctor
		DevRandomRNG();
		//Dtor
		~DevRandomRNG();

		//Return true if this source is dependant on any pseudo rng
		inline bool hasPseudo();

		//Generates a random number on [0,0xffffffff]-interval
		//This is the main generator function that all the others are based on
		quint32 generateInt32(void);

		//Initializes entropysouce with a seed if needed
		void init(quint32 s);

};


#endif // DEVRANDOMRNG_HPP
