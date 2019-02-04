#include "MagicDetector.hpp"


MagicDetector::MagicDetector(const uint8_t *magic, const uint8_t sz)
	: idx(0)
	, magic(magic)
	, sz(sz)
{

}

bool MagicDetector::detect(const uint8_t in)
{
	const uint8_t my=(magic[idx])&0xFF;
	if(my==in) {
		idx++;
		if(sz == idx) {
			idx=0;
			return true;
		}
	} else {
		idx=0;
	}
	return false;
}

void MagicDetector::reset()
{
	idx=0;
}
