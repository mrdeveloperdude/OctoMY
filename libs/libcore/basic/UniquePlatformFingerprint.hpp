#ifndef UNIQUEPLATFORMFINGERPRINT_HPP
#define UNIQUEPLATFORMFINGERPRINT_HPP

#include "Fingerprint.hpp"



///////////////////////////////////////////////////////////////


class UniquePlatformFingerprint{
	private:
		Fingerprint platformPrint;
		Fingerprint executablePrint;
	private:
		UniquePlatformFingerprint();
		static UniquePlatformFingerprint *instance;
	public:
		static  UniquePlatformFingerprint & getInstance();
		const Fingerprint &platform() const ;
		const Fingerprint &executable() const ;
};

#endif // UNIQUEPLATFORMFINGERPRINT_HPP
