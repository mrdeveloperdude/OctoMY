#ifndef TESTOPENCL_HPP
#define TESTOPENCL_HPP

#include "utility/CrashHelp.hpp"
#include <QTest>


class TestOpenCL:public QObject
{
	Q_OBJECT

private:
	CrashHelp mCrashHelper;
//	QSurfaceFormat testFormat;

void testCLGLView();
	void testWithoutGLInterop();

private slots:
	void initTestCase();


	void testWithGLInterop();
	void cleanupTestCase();

};


#endif // TESTOPENCL_HPP
