#ifndef MAGICDETECTOR_HPP
#define MAGICDETECTOR_HPP

struct MagicDetector {
	unsigned char idx;
	const unsigned char *magic;
	const unsigned char sz;
	MagicDetector(const unsigned char *magic, const unsigned char sz);

	bool detect(const unsigned char in);
	void reset();
};

#endif // MAGICDETECTOR_HPP
