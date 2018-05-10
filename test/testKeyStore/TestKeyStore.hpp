#ifndef TESTKEYSTORE_HPP
#define TESTKEYSTORE_HPP

#include "../common/TestCommon.hpp"


class TestKeyStore:public QObject
{
	Q_OBJECT

private:
	const QString fn="test_keystore.json";
private slots:
	void testInit();
	//void testBootstrap();
	void testInsertRemove();

};


#endif // TESTKEYSTORE_HPP
