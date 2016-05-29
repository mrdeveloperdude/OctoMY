#include "RNG.hpp"

RNG::~RNG() {
}

// Fallback implementation simply uses first integer
inline void RNG::init(quint32 init_key[], size_t key_length) {
	if (key_length > 0){
		init(init_key[0]);
	}
	else{
		init();
	}
}

#include "EfficientPRNG.hpp"
#include "ParkMillerPRNG.hpp"
#include "DevRandomRNG.hpp"
#include "SystemPRNG.hpp"

// Return an instance of a RNG that best matches the given merit
RNG * RNG::sourceFactory(RNGMerit m) {
	// Create and return a new RNG that matches the requested merit
	switch (m) {
		case (EFFICIENT):
			return new EfficientPRNG();
		case (SCALABLE):
			return new ParkMillerPRNG();
		case (SECURE):
			return new DevRandomRNG();
		case (SYSTEM):
			return new SystemPRNG();
		case (ACCURATE):
			return new DevRandomRNG();
		default:
		case (GENERAL):
			return new ParkMillerPRNG();
	}
	return nullptr;
}
