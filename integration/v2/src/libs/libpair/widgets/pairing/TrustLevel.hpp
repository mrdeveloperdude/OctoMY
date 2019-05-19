#ifndef TRUSTLEVEL_HPP
#define TRUSTLEVEL_HPP

#include <QString>

enum TrustLevel:quint8 {
	IGNORE, TRUST, BLOCK
};


QString trustLevelToString(TrustLevel level);

#endif
// TRUSTLEVEL_HPP
