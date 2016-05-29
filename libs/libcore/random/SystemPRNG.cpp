#include "SystemPRNG.hpp"


// This simply uses the standard entropy source present in the system libraries (libc rand())

SystemPRNG::~SystemPRNG() {
}

SystemPRNG::SystemPRNG() {
}

// Return true if this source is dependant on any pseudo rng
inline bool SystemPRNG::hasPseudo() {
	return true;
}

// Generates a random number on [0,0xffffffff]-interval
// This is the main generator function that all the others are based on
uint32_t SystemPRNG::generateInt32(void) {
	return rand();
}

// Initializes entropysouce with a seed if needed
void SystemPRNG::init(uint32_t s) {
	srand(s);
}
