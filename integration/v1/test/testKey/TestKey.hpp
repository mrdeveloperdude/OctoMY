#ifndef TESTKEY_HPP
#define TESTKEY_HPP

#include "../common/TestCommon.hpp"


class TestKey:public QObject
{
	Q_OBJECT

private slots:
	void testAllocation();
	void testBasicIntegrity();
	void testSignature();
	void testEncryption();
};


#endif // TESTKEY_HPP
