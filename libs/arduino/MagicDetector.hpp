#ifndef MAGICDETECTOR_HPP
#define MAGICDETECTOR_HPP

#include "ArduMY.hpp"

struct MagicDetector {
	uint8_t idx;
	const uint8_t *magic;
	const uint8_t sz;
	MagicDetector(const uint8_t *magic, const uint8_t sz);

	bool detect(const uint8_t in);
	void reset();
};

#endif // MAGICDETECTOR_HPP
