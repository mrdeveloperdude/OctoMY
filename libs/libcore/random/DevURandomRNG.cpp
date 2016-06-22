#include "DevURandomRNG.hpp"


// This entropy source is the second most accurate and safe available. It is a "pseudo-interpolated quantum quality noise source".
// It is idetical to DevRandom, except that it uses pseudo random numbers when the entropy is empty. Its characteristics are thus very comparable to DevRandom (slow, low scalability etc)
// and the quality of its output is lower. However it solves the blocking I/O issue with DevRandom

//Static members
FILE *DevURandomRNG::fh = 0;

//Constructor
DevURandomRNG::DevURandomRNG()
	: RNG("DevURandomRNG")
{
}

//Destructor
DevURandomRNG::~DevURandomRNG() {
	fclose(fh);
	fh = 0;
}

//Return true if this source is dependant on any pseudo rng
inline bool DevURandomRNG::hasPseudo() {
	return false;
}

//Generates a random number on [0,0xffffffff]-interval
//This is the main generator function that all the others are based on
quint32 DevURandomRNG::generateInt32(void) {
	uint32_t s = 0;
	//int r =  //TODO: Check r
	fread(&s, sizeof(quint32), 1, fh);
	return s;
}

//Initializes entropysouce with a seed if needed
void DevURandomRNG::init(quint32) {
	//Open file if closed
	if (0 == fh) {
		//Open entrophy pool
		fh = fopen("/dev/urandom", "rb");
		//Fall back on clock when urandom is not available
		if (0 == fh) return;
	}
}

