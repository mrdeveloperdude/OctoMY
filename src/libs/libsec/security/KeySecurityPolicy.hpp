#ifndef KEYSECURITYPOLICY_HPP
#define KEYSECURITYPOLICY_HPP

#include "security/SecurityConstants.hpp"

#include <QCryptographicHash>

class KeySecurityPolicy
{
private:
	quint32 mBits;
	QCryptographicHash::Algorithm mHashAlgo;

public:
	explicit KeySecurityPolicy(quint32 bits=OCTOMY_KEY_BITS, QCryptographicHash::Algorithm=QCryptographicHash::Sha256);
	virtual ~KeySecurityPolicy();

public:
	quint32 bits();
	QCryptographicHash::Algorithm hashAlgo();

};

#endif
// KEYSECURITYPOLICY_HPP
