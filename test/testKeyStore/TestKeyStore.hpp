#ifndef TESTKEYSTORE_HPP
#define TESTKEYSTORE_HPP

#include <QTest>


class TestKeyStore:public QObject
{
	Q_OBJECT

private slots:
	void testInit();
	void testInsertRemove();

};


#endif // TESTKEYSTORE_HPP
