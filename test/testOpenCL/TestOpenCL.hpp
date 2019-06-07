#ifndef TESTOPENCL_HPP
#define TESTOPENCL_HPP

#include "Common_test.hpp"

//#include "uptime/CrashHelp.hpp"

class TestOpenCL:public QObject
{
	Q_OBJECT

private:
//	CrashHelp mCrashHelper;
//	QSurfaceFormat testFormat;
	void testCLGLView();
	void testWithoutGLInterop();

private slots:
	void initTestCase();
	void testWithGLInterop();
	void cleanupTestCase();

};


#endif
// TESTOPENCL_HPP
