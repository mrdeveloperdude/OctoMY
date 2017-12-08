#ifndef TESTKEYSTORE_HPP
#define TESTKEYSTORE_HPP

#include "../common/TestCommon.hpp"


class TestKeyStore:public QObject
{
	Q_OBJECT

private slots:
	void testInit();
	void testInsertRemove();

};


#endif // TESTKEYSTORE_HPP
