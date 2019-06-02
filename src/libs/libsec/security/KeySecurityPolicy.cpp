#include "KeySecurityPolicy.hpp"

#include "uptime/MethodGate.hpp"

KeySecurityPolicy::KeySecurityPolicy(quint32 bits, QCryptographicHash::Algorithm hashAlgo)
	: mBits(bits)
	, mHashAlgo(hashAlgo)
{
	OC_METHODGATE();
}


KeySecurityPolicy::~KeySecurityPolicy()
{
	OC_METHODGATE();
}


quint32 KeySecurityPolicy::bits()
{
	OC_METHODGATE();
	return mBits;
}


QCryptographicHash::Algorithm KeySecurityPolicy::hashAlgo()
{
	OC_METHODGATE();
	return mHashAlgo;
}
