#include "KeySecurityPolicy.hpp"

KeySecurityPolicy::KeySecurityPolicy(quint32 bits, QCryptographicHash::Algorithm hashAlgo)
	: mBits(bits)
	, mHashAlgo(hashAlgo)
{
}



quint32 KeySecurityPolicy::bits() const
{
	return mBits;
}



QCryptographicHash::Algorithm KeySecurityPolicy::hashAlgo() const
{
	return mHashAlgo;
}
