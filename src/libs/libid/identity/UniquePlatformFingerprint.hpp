#ifndef UNIQUEPLATFORMFINGERPRINT_HPP
#define UNIQUEPLATFORMFINGERPRINT_HPP

#include "Fingerprint.hpp"


class UniquePlatformFingerprint
{

private:
	Fingerprint platformPrint;
	Fingerprint executablePrint;
	static UniquePlatformFingerprint *instance;

private:
	UniquePlatformFingerprint();

public:
	static  UniquePlatformFingerprint & getInstance();
	const Fingerprint &platform() const ;
	const Fingerprint &executable() const ;
};

#endif // UNIQUEPLATFORMFINGERPRINT_HPP
