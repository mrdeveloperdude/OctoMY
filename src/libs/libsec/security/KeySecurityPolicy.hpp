#ifndef KEYSECURITYPOLICY_HPP
#define KEYSECURITYPOLICY_HPP

#include "security/SecurityConstants.hpp"

#include <QCryptographicHash>

class KeySecurityPolicy
{
private:
	const quint32 mBits;
	const QCryptographicHash::Algorithm mHashAlgo;
public:
	KeySecurityPolicy(quint32 bits=OCTOMY_KEY_BITS, QCryptographicHash::Algorithm=QCryptographicHash::Sha256);

	quint32 bits() const;

	QCryptographicHash::Algorithm hashAlgo() const;
};

#endif // KEYSECURITYPOLICY_HPP
