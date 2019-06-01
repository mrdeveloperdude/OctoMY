#ifndef TESTKEYS_HPP
#define TESTKEYS_HPP

#include <QString>

struct TestKeys {
	QString privKeyPEM;
	QString pubKeyPEM;
	QString keyID;
	QString keyID_bad;

	TestKeys();
};

#endif
// TESTKEYS_HPP
