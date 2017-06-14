#include "SystemPRNG.hpp"


// This simply uses the standard entropy source present in the system libraries (libc rand())

SystemPRNG::~SystemPRNG() {
}

SystemPRNG::SystemPRNG()
	: RNG("SystemPRNG")
{
}

// Return true if this source is dependant on any pseudo rng
inline bool SystemPRNG::hasPseudo() {
	return true;
}

// Generates a random number on [0,0xffffffff]-interval
// This is the main generator function that all the others are based on
quint32 SystemPRNG::generateInt32(void) {
	return rand();
}

// Initializes entropysouce with a seed if needed
void SystemPRNG::init(quint32 s) {
	srand(s);
}
