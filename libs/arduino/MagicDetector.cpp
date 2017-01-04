#include "MagicDetector.hpp"


MagicDetector::MagicDetector(const unsigned char *magic, const unsigned char sz)
	: idx(0)
	, magic(magic)
	, sz(sz)
{

}

bool MagicDetector::detect(const unsigned char in)
{
	const unsigned char my=(magic[idx])&0xFF;
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
