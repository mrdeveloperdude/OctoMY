#include "DevRandomRNG.hpp"


//This entropy source is the most accurate and safe available. It is not a pseudo but rather depends on the kernel's "quantum quality noise source".
//It alto happens to be the slowest, least scalable and most cumbersome to use. Worst case scenario

//Static members
FILE *DevRandomRNG::fh = 0;

//Ctor
DevRandomRNG::DevRandomRNG()
	: RNG("DevRandomRNG")
{
}

//Dtor
DevRandomRNG::~DevRandomRNG() {
	fclose(fh);
	fh = 0;

}

//Return true if this source is dependant on any pseudo rng
inline bool DevRandomRNG::hasPseudo() {
	return false;
}

//Generates a random number on [0,0xffffffff]-interval
//This is the main generator function that all the others are based on
quint32 DevRandomRNG::generateInt32(void) {
	uint32_t s = 0;
	//int r =  //TODO: Check r
	fread(&s, sizeof(quint32), 1, fh);
	return s;
}

//Initializes entropysouce with a seed if needed
void DevRandomRNG::init(quint32) {
	//Open file if closed
	if (0 == fh) {
		//Open entrophy pool
		fh = fopen("/dev/random", "rb");
		//Fall back on clock when urandom is not available
		if (0 == fh) return;
	}
}


