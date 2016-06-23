#include "DevURandomRNG.hpp"


// This entropy source is the second most accurate and safe available. It is a "pseudo-interpolated quantum quality noise source".
// It is idetical to DevRandom, except that it uses pseudo random numbers when the entropy is empty. Its characteristics are thus very comparable to DevRandom (slow, low scalability etc)
// and the quality of its output is lower. However it solves the blocking I/O issue with DevRandom


//Constructor
DevURandomRNG::DevURandomRNG()
	: FileRandomRNG("DevURandomRNG", "/dev/urandom")
{
}

