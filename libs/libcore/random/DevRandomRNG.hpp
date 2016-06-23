#ifndef DEVRANDOMRNG_HPP
#define DEVRANDOMRNG_HPP

#include "FileRandomRNG.hpp"

//This entropy source is the most accurate and safe available. It is not a pseudo but rather depends on the kernel's "quantum quality noise source".
//It alto happens to be the slowest, least scalable and most cumbersome to use. Worst case scenario

class DevRandomRNG: public FileRandomRNG {

	public:

		DevRandomRNG();

};


#endif // DEVRANDOMRNG_HPP
