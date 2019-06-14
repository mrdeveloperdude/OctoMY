#include "TestOpenCL.hpp"

#ifdef OC_USE_LIB_EXT_OPENCL

#include "TestCLWorkerFactory.hpp"
#include "TestCLGLViewRenderer.hpp"

#include "clt/CLGLInteropConfig.hpp"
#include "clt/CLThreadManager.hpp"
#include "clt/CLUtils.hpp"
#include "widgets/CLGLView.hpp"

#include "utility/time/HumanTime.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

#include <iostream>

static void clBuildCallback(cl_program program, void *user_data)
{
	Q_UNUSED(user_data);
	qDebug()<<"clBuildCallback: "<<program;
	//fprintf(stderr, "OpenCL Error (via pfn_notify): %s\n", errinfo);
}


void TestOpenCL::initTestCase()
{
	OC_METHODGATE();
	//mCrashHelper.setEnabled(true);
}


void TestOpenCL::testWithoutGLInterop()
{
	OC_METHODGATE();
	TestCLWorkerFactory twf;
	CLGLInteropConfig ic(nullptr);
	// Just do CPU for maximum portability/availability/debugability
	CLThreadManager tm(twf, ic, "", true, true); // CLWorkerFactory &factory, CLGLInteropConfig config=CLGLInteropConfig(), QString deviceSelectionString="", bool allowGPU=true, bool allowCPU=false, QObject *parent = nullptr
	tm.setRunning(true);
	const quint64 start=utility::time::currentMsecsSinceEpoch<quint64>();
	quint64 now=start;
	while(now<start+10000) {
		now=utility::time::currentMsecsSinceEpoch<quint64>();
		qApp->processEvents();
	}
	tm.setRunning(false,true);
}

static QWidget *createTestWidget(CLGLView *glViewp, QSize sz)
{
	QWidget *w=OC_NEW QWidget();
	w->resize(512,512);
	w->setWindowTitle("TestOpenCL::testWithGLInterop()");
	QVBoxLayout *verticalLayout = OC_NEW QVBoxLayout(w);
	verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
	QPushButton *pushButtonClose = OC_NEW QPushButton(w);
	pushButtonClose->setObjectName(QStringLiteral("pushButtonClose"));
	pushButtonClose->setText("CLOSE");
	verticalLayout->addWidget(pushButtonClose);


	w->layout()->addWidget(glViewp);
	qApp->processEvents();
	w->resize(sz);
	qApp->processEvents();
	glViewp->resize(sz);
	qApp->processEvents();
	qDebug()<<"--- GUI COMPLETE ---";
	return w;
}


void TestOpenCL::testWithGLInterop()
{
	OC_METHODGATE();
	for(auto flags=0; flags<4; ++flags) {
		const bool doCPU=(flags&1)>0;
		const bool doGPU=(flags&2)>0;
		qDebug()<<"";
		qDebug().nospace().noquote()<<"======= TESTING WITH DO-GPU="<<doGPU<<", DO-CPU="<<doCPU<<" =======";
		CLGLView *glViewp=OC_NEW CLGLView();
		QSize sz(320,240);
		QWidget *w=createTestWidget(glViewp, sz);
		QVERIFY(nullptr!=w);
		qApp->processEvents();
		//TestCLGLViewRenderer testCLGLViewRendrer;
		TestCLGLViewRenderer *testCLGLViewRendrer_p=OC_NEW TestCLGLViewRenderer();//&testCLGLViewRendrer;

		GLContext shareingContext=glViewp->sharingGLContext();
		qDebug().noquote().nospace()<<" SHARING CONTEXT WAS "<<shareingContext.toString();
		TestCLWorkerFactory twf;
		CLGLInteropConfig ic(&shareingContext, sz);
		CLThreadManager tm(twf, ic, "", doGPU, doCPU);
		testCLGLViewRendrer_p->setManager(&tm);

		testCLGLViewRendrer_p->initialize(shareingContext);

		glViewp->setRenderer(testCLGLViewRendrer_p);
		qApp->processEvents();
		w->show();
		qApp->processEvents();

		testCLGLViewRendrer_p->setRendering(true);
		qDebug()<<"--- WAITING ---";
		const quint64 start=utility::time::currentMsecsSinceEpoch<quint64>();
		quint64 now=start;
		while(now<start+2000) {
			now=utility::time::currentMsecsSinceEpoch<quint64>();
			qApp->processEvents();
		}
		qDebug()<<"--- DONE ---";
		testCLGLViewRendrer_p->setRendering(false, true);
		qApp->processEvents();
		qDebug()<<"--- END ---";
		w->deleteLater();
		w=nullptr;
		glViewp->deleteLater();
		glViewp=nullptr;
		delete testCLGLViewRendrer_p;
		testCLGLViewRendrer_p=nullptr;
		qApp->processEvents();
	}
}


void TestOpenCL::testCLGLView()
{
	OC_METHODGATE();
	CLGLView glView;
	glView.show();
	qApp->processEvents();
	QTest::qSleep(10000);

}


void TestOpenCL::cleanupTestCase()
{
	OC_METHODGATE();
	//mCrashHelper.setEnabled(false);
	qDebug()<<"CLEANUP";
}

// I CANNOT USE QTEST_MAIN MACRO HERE BECAUSE I NEED TO
// ALTER THE QApplication OBJECT BEFORE EXEC TO HAVE GL-CL INTEROP WORKINIG
// OC_TEST_MAIN(test, TestOpenCL)

QT_BEGIN_NAMESPACE
QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS
QT_END_NAMESPACE
int main(int argc, char *argv[])
{
	QSurfaceFormat format=properOctomyDefaultFormat();
	/*
	From QSurfaceFormat::setDefaultFormat() documentation:

	Note: When setting Qt::AA_ShareOpenGLContexts, it is strongly recommended
	to place the call to this function before the construction of the
	QGuiApplication or QApplication. Otherwise format will not be applied to
	the global share context and therefore issues may arise with context sharing
	afterwards.
	*/
	QSurfaceFormat::setDefaultFormat(format);
	//testFormat=format;
	qDebug()<<"INIT: SET UP DEFAULT FORMAT: "<<format;

	QApplication app(argc, argv);
	app.setAttribute(Qt::AA_Use96Dpi, true);
	app.setAttribute(Qt::AA_ShareOpenGLContexts, true); // <-- ENABLE GL Sharing CONTEXTS
	QTEST_DISABLE_KEYPAD_NAVIGATION
	QTEST_ADD_GPU_BLACKLIST_SUPPORT
	TestOpenCL tc;
	QTEST_SET_MAIN_SOURCE_PATH
	return QTest::qExec(&tc, argc, argv);
}

#else


void TestOpenCL::testCLGLView()
{
	qWarning()<<"CL DISABLED IN THIS BUILD";
}


void TestOpenCL::testWithoutGLInterop()
{
	qWarning()<<"CL DISABLED IN THIS BUILD";
}


void TestOpenCL::initTestCase()
{
	qWarning()<<"CL DISABLED IN THIS BUILD";
}


void TestOpenCL::testWithGLInterop()
{
	qWarning()<<"CL DISABLED IN THIS BUILD";
}


void TestOpenCL::cleanupTestCase()
{
	qWarning()<<"CL DISABLED IN THIS BUILD";
}

#endif
// OC_USE_LIB_EXT_OPENCL
