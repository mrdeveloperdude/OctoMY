#ifndef TESTHAAR_HPP
#define TESTHAAR_HPP

#include "test/Common.hpp"

class TestHaar:public QObject
{
	Q_OBJECT
private:
	void testBT601Gray();
	void testIntegralImage();
	void testCameraProcessor();
private slots:
	void testHaarProcessor();

};


#endif
// TESTHAAR_HPP
