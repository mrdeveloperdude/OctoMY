#ifndef TESTMAPEDITOR_HPP
#define TESTMAPEDITOR_HPP

#include "../common/TestCommon.hpp"

class TestMapEditor:public QObject
{
	Q_OBJECT
private slots:
	void testNetworkCache();
	void test();

};


#endif // TESTMAPEDITOR_HPP
