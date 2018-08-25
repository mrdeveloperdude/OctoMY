#ifndef MAGICDETECTOR_HPP
#define MAGICDETECTOR_HPP

#include "ardumy/ArduMY.hpp"

/**
 * @brief The MagicDetector struct is a mechanism to help detect  a "magic" sequence in a stream of bytes
 */
struct MagicDetector {
	uint8_t idx;
	const uint8_t *magic;
	const uint8_t sz;
	MagicDetector(const uint8_t *magic, const uint8_t sz);

	bool detect(const uint8_t in);
	void reset();
};

#endif // MAGICDETECTOR_HPP
