#ifndef TESTKEYSTORE_HPP
#define TESTKEYSTORE_HPP

#include "test/Common.hpp"


class TestKeyStore:public QObject
{
	Q_OBJECT

private:
	const QString fileName="test_keystore.json";

private slots:
	void testInit();
	void testInsertRemove();

};


#endif
// TESTKEYSTORE_HPP
