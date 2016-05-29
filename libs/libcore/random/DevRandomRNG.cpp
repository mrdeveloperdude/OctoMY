#include "DevRandomRNG.hpp"


//This entropy source is the most accurate and safe available. It is not a pseudo but rather depends on the kernel's "quantum quality noise source".
//It alto happens to be the slowest, least scalable and most cumbersome to use. Worst case scenario

//Dtor
DevRandomRNG::~DevRandomRNG() {
}

//Ctor
DevRandomRNG::DevRandomRNG() {
}

//Return true if this source is dependant on any pseudo rng
inline bool DevRandomRNG::hasPseudo() {
	return false;
}

//Generates a random number on [0,0xffffffff]-interval
//This is the main generator function that all the others are based on
uint32_t DevRandomRNG::generateInt32(void) {

	return 0;
}

//Initializes entropysouce with a seed if needed
void DevRandomRNG::init(uint32_t) {
}
