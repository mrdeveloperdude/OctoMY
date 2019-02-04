#ifndef DEVURANDOMRNG_HPP
#define DEVURANDOMRNG_HPP

#include "FileRandomRNG.hpp"


// This entropy source is the second most accurate and safe available. It is a "pseudo-interpolated quantum quality noise source".
// It is idetical to DevRandom, except that it uses pseudo random numbers when the entropy is empty. Its characteristics are thus very comparable to DevRandom (slow, low scalability etc)
// and the quality of its output is lower. However it solves the blocking I/O issue with DevRandom

class DevURandomRNG: public FileRandomRNG {
	public:

		explicit DevURandomRNG();

};

#endif // DEVURANDOMRNG_HPP
