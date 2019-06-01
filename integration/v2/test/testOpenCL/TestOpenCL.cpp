#include "TestOpenCL.hpp"

#ifdef EXTERNAL_LIB_OPENCL

#include "clt/CLThreadManager.hpp"
#include "clt/CLWorkerFactory.hpp"
#include "clt/CLWorker.hpp"
#include "clt/CLGLInteropConfig.hpp"
#include "clt/CLGLViewRenderer.hpp"
#include "widgets/CLGLView.hpp"

#include "clt/CLUtils.hpp"

#include "utility/Standard.hpp"


#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>


#include <iostream>
////////////////////////////////////////////////////////////////////////////////



static float map(float x, float xmin, float xmax, float ymin, float ymax)
{
	return ymin + (x - xmin) * (ymax - ymin) / (xmax - xmin);
}


static QPoint map(QPoint X, QRect input, QRect output)
{
	QPoint res;
	res.setX(map(X.x(), input.left(), input.right(), output.left(), output.right()));
	res.setY(map(X.y(), input.bottom(), input.top(), output.bottom(), output.top()));
	return res;
}



static void clBuildCallback(cl_program program, void *user_data)
{
	qDebug()<<"clBuildCallback: "<<program;
	//fprintf(stderr, "OpenCL Error (via pfn_notify): %s\n", errinfo);
}


class TestCLWorker: public CLWorker
{
private:
	cl::CommandQueue queue;
	cl::Program::Sources source;
	cl::Program program;
	cl::Kernel kernel;
	QVector<QPoint> sources;
	QMutex sourceChangeMutex;
	float limit;
	bool applyLimit;
	float charge;
	QString kernelParams;
public:
	explicit TestCLWorker(CLThreadManager &man, int index)
		: CLWorker(man, index, nullptr)
		, limit(0.0)
		, applyLimit(false)
		, charge(0.0)
	{
		qDebug()<<"TCLW Ctor INDEX "<<index;
	}

	virtual	~TestCLWorker()
	{
		qDebug()<<"TCLW Dtor";
	}


	void initCLStuff()
	{
		const QSize size=manager().interopConfig().size();
		kernelParams="";
		QTextStream ss(&kernelParams);
		ss.setRealNumberNotation(QTextStream::ScientificNotation);
		ss.setRealNumberPrecision(6);
		ss<<
//		  " -g" <<
		  "\n -D PARAM_SCREEN_WIDTH=" << size.width() <<
		  "\n -D PARAM_SCREEN_HEIGHT=" << size.height() <<

//		  "\n -cl-nv-opt-level 0"
		  //	  "\n -cl-nv-verbose"

		  "";

		auto parts=kernelParams.split("\n", QString::SkipEmptyParts);
		qDebug()<<"Defined CL parametes: ";
		for(auto it:parts) {
			qDebug().noquote()<<" + "<<it;
		}

		// Create a command queue and use the first device
		auto ctx=clContext();
		auto dev=clDevice();
		qDebug()<<"TCLW cl queue with CTX="<<ctx<<" and DEV="<<dev;
		if(nullptr!=ctx && nullptr!=dev) {
			queue = cl::CommandQueue(*ctx,*dev);
		} else {
			qWarning()<<"ERROR: CTX or DEV was null";
		}
	}

	void loadCLProgram()
	{
		QString sourcePath="://kernels/metaballs.cl";
		QFile sourceFile(sourcePath);
		if(sourceFile.exists()) {
			sourceFile.open(QFile::ReadOnly);
			QString sourceCode=QString::fromLatin1(sourceFile.readAll());
			QStringList sourceLines=sourceCode.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
			for(QString sourceLine:sourceLines) {
				qDebug().nospace().noquote()<<"TCLW cl source "<<sourceLine.replace(QRegExp("[\t]"),"   ");
			}
			auto sourceStd=sourceCode.toStdString();
			cl::Program::Sources  source;
			source.push_back({sourceStd.c_str(),sourceStd.length()});
			//(1, std::make_pair(sourceStd.c_str(), sourceCode.length() + 1));
			// Make program of the source code in the context
			qDebug()<<"TCLW cl program";
			cl::Context *ctx=clContext();
			QVERIFY ( nullptr != ctx);
			cl::Program program(*ctx, source);

			// Build program for these specific devices
			qDebug()<<"TCLW cl build";
			VECTOR_CLASS<cl::Device> buildDevices;
			auto dev=clDevice();
			buildDevices.push_back(*dev);
			//CL_DETECT_ERROR(program.build(devices, kernelParams.replace("\n","").toStdString().c_str()));
			auto kParamsStd=kernelParams.replace("\n","").toStdString();
			std::cout << "Kernel parameters: "<<kParamsStd<<std::endl;
			std::cout << "Program source: \n"<<sourceStd<<std::endl;

			auto code=program.build(buildDevices, kParamsStd.c_str() /*, &clBuildCallback */) ;
			if(CL_SUCCESS!=code) {
				QString error=OCLErrorString(code);
				qWarning()<<"ERROR: Build failed with "<<error<<" ( "<<code<<" ). More error data follows:";
				int i=0;
				for(cl::Device buildDev:buildDevices) {

					qDebug()<<" + REPORTING STATUS FOR BUILD DEVICE "<<i<< " ---------";
					i++;
					const cl_build_status buildStatus=program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(buildDev);
					const cl_program_binary_type buildBinaryType=program.getBuildInfo<CL_PROGRAM_BINARY_TYPE>(buildDev);
					const cl::STRING_CLASS buildLog=program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(buildDev);
					const cl::STRING_CLASS buildOptions=program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(buildDev);
					std::cout << " + Build Status: " << buildStatus << std::endl;
					std::cout << " + Build Binary Type: " << buildBinaryType << std::endl;
					std::cout << " + Build Options:\t" << buildOptions << std::endl;
					std::cout << " + Build Log:\t " << buildLog << std::endl;




//					auto buildStatus=program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(buildDev);
					switch(buildStatus) {
					// The build status returned if no build has been performed on the specified program object for device.
					case(CL_BUILD_NONE): {
						qDebug()<<" + TCLW cl build for device "<<buildDev<<" NOT DONE";
					}
					break;
					// The build status returned if the last call to clBuildProgram on the specified program object for device generated an error.
					case(CL_BUILD_ERROR): {
						auto strError = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(buildDev);
						qWarning()<<" + TCLW cl kernel ERROR: compilation failed with:\n" << QString::fromStdString(strError);
						exit(1);
					}
					break;
					// The build status returned if the last call to clBuildProgram on the specified program object for device was successful.
					case(CL_BUILD_SUCCESS) : {
						qDebug()<<" + TCLW cl build for device "<<buildDev<<" OK";
					}
					break;
					//  The build status returned if the last call to clBuildProgram on the specified program object for device has not finished.
					case(CL_BUILD_IN_PROGRESS): {
						qDebug()<<" + TCLW cl build for device "<<buildDev<<" IN PROGRESS";
					}
					break;
					default: {
						qDebug()<<" + TCLW cl build for device "<<buildDev << " status unhandled: "<<buildStatus;
						exit(1);
					}
					}
				}
			} else {
				qDebug()<<"Build succeeded!";
			}
			// Make kernel
			qDebug()<<"TCLW cl kernel";
			kernel = cl::Kernel(program, "metaballs");
		} else {
			qWarning()<<"ERROR: kernel does not exist for path "<<sourcePath;
		}
	}

	void preparePoints()
	{
		const QSize size=manager().interopConfig().size();
		int width=size.width();
		int height=size.height();
		int grate=100;
		qDebug()<<"TCLW cl points";
		// Fill the sources vector with a hex grid pattern
		sources.clear();
		for (int i = 0; i <= width; i += grate) {
			for (int j = 0; j <= height; j += grate) {
				sources.push_back(QPoint(i+((0==(j&1))?grate/2:0), j));
			}
		}
		// One last point that will be used as the mouse pointer
		sources.push_back(QPoint(width/2,height/2));
	}


	void calculateTexture()
	{
		QMutexLocker lock(&sourceChangeMutex);
		auto clctx=*clContext();
		//qDebug()<<"calculateTexture() ";
		const int num = sources.size();
		// Create a memory buffer on the GPU
		cl::Buffer bufferSources = cl::Buffer(clctx, CL_MEM_READ_ONLY, num * sizeof(cl_int2));
		// Create a memory buffer on the CPU
		cl_int2 *sourcePoints = OC_NEW cl_int2[num];
		// Fill the buffer with the coordinates of the source points
		for (int i = 0; i < num; i++) {
			sourcePoints[i].s[0] = sources.at(i).x();
			sourcePoints[i].s[1] = sources.at(i).y();
		}
		// Copy the buffer with the source points
		CL_DETECT_ERROR(queue.enqueueWriteBuffer(bufferSources, CL_TRUE, 0, num * sizeof(cl_int2), sourcePoints));

		// Now we can delete the buffer from the CPU side
		delete[] sourcePoints;
		sourcePoints=nullptr;
		// Set arguments to kernel
		unsigned int argIndex = 0;
		CL_DETECT_ERROR(kernel.setArg(argIndex++, bufferSources));
		CL_DETECT_ERROR(kernel.setArg(argIndex++, cl_int(num)));
		CL_DETECT_ERROR(kernel.setArg(argIndex++, cl_float(charge)));
		CL_DETECT_ERROR(kernel.setArg(argIndex++, cl_float(limit)));
		CL_DETECT_ERROR(kernel.setArg(argIndex++, cl_int(applyLimit)));
		CL_DETECT_ERROR(kernel.setArg(argIndex++, *pboBuff()));
		//qDebug()<<"TCLW SENT "<< argIndex<<" ARGUMENTS TO KERNEL";
		//res += kernel.setArg(argIndex++, *output); // tODO, how to get the pbo here?
		// Run the kernel on specific ND range
		const QSize size=manager().interopConfig().size();
		cl::NDRange global(size.width(), size.height());
		CL_DETECT_ERROR(queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, cl::NullRange));
		CL_DETECT_ERROR(queue.finish());
	}

	void changeMousePoint(QPoint point)
	{
		QMutexLocker lock(&sourceChangeMutex);
		if(!sources.isEmpty()) {
			sources.pop_back( );
		}
		const QSize size=manager().interopConfig().size();
		sources.push_back(map(point, QRect(0, 0, point.x(), point.y()), QRect(0, 0, size.width(), size.height())));
	}

	// CLWorker interface
	//////////////////////////////////////////



public:
	void preProcessImp() Q_DECL_OVERRIDE{
		qDebug()<<"TCLW PRE PROCESS WORKER START WITH INDEX "<<index()<< " IN THREAD "<<QThread::currentThreadId();
		qDebug()<<"TCLW PRE PROCESS WORKER END WITH INDEX "<<index()<< " IN THREAD "<<QThread::currentThreadId();
	}

	void processImp() Q_DECL_OVERRIDE{
		qDebug()<<"TCLW PROCESS WORKER START WITH INDEX "<<index()<< " IN THREAD "<<QThread::currentThreadId();
		initCLStuff();
		loadCLProgram();
		preparePoints();
		while(isRunning())
		{
			calculateTexture();
			qApp->processEvents();
		}
		qDebug()<<"TCLW PROCESS WORKER END WITH INDEX "<<index()<< " IN THREAD "<<QThread::currentThreadId();
	}
	void postProcessImp() Q_DECL_OVERRIDE{
		qDebug()<<"TCLW POST PROCESS WORKER START WITH INDEX "<<index()<< " IN THREAD "<<QThread::currentThreadId();
		qDebug()<<"TCLW POST PROCESS WORKER END WITH INDEX "<<index()<< " IN THREAD "<<QThread::currentThreadId();
	}

};


////////////////////////////////////////////////////////////////////////////////


class TestCLWorkerFactory: public CLWorkerFactory
{
	CLWorker * createInstance(CLThreadManager &man, int index) Q_DECL_OVERRIDE{
		qDebug()<<"TCLWF CREATED WORKER WITH INDEX "<<index;
		TestCLWorker *tw=OC_NEW TestCLWorker(man, index);
		return tw;
	}
};

////////////////////////////////////////////////////////////////////////////////

class TestCLGLViewRenderer:public CLGLViewRenderer
{

private:
	CLThreadManager *mThreadManager;
	bool mDone;

public:

	explicit TestCLGLViewRenderer()
		: CLGLViewRenderer()
		, mThreadManager(nullptr)
		, mDone(true)
	{
		qDebug()<<"TCLGLVR Ctor";
	}


	virtual ~TestCLGLViewRenderer()
	{
		mDone=true;
		qDebug()<<"TCLGLVR Dtor";
	}


	void setManager(CLThreadManager *man)
	{
		mThreadManager=man;
	}


	// CLGLViewRenderer Interface
public:
	void initialize(GLContext &ctx)
	{
		qDebug()<<"TCLGLVR Initializing with: "<<ctx.toString();
		if(nullptr!=mThreadManager) {
			CLWorker *renderWorker=mThreadManager->renderWorker();
			if(nullptr!=renderWorker) {
				//renderWorker->set
				//
				//
				//     TODO FIGURE THIS OUT
				//
				//
				//
			} else {
				qWarning()<<"ERROR: NO render worker while initializing with context";
			}
		} else {
			qWarning()<<"ERROR: NO thread manager while initializing with context";
		}
	}

	void resize(QSize sz) Q_DECL_OVERRIDE{
		qDebug()<<"TCLGLVR Resizing to: "<<sz;
	}

	bool setRendering(bool running, bool block=false) Q_DECL_OVERRIDE{
		if(nullptr!=mThreadManager)
		{
			if(running) {
				if(!mDone) {
					qWarning()<<"TCLGLVR WARNING: Already running, skipping";
					return true;
				}
				mDone=false;
			} else {
				mDone=true;
			}
			mThreadManager->setRunning(running, block);
		} else {
			qWarning()<<"TCLGLVR ERROR: No CL thread manager while set running to: "<<running;
		}
		qDebug()<<"TCLGLVR Setting running to: "<<running;
		return running;
	}

	bool isRendering() const Q_DECL_OVERRIDE
	{
		bool ret=false;
		if(nullptr!=mThreadManager) {
			ret=mThreadManager->isRunning();
		} else {
			qWarning()<<"TCLGLVR ERROR: No CL thread manager while checking for isRunning";
		}
		qDebug()<<"TCLGLVR Returning "<<ret;
		return ret;
	}

	void renderFrame() Q_DECL_OVERRIDE{
		qDebug()<<"TCLGLVR Rendering frame:";
		if(nullptr!=mThreadManager)
		{
			CLWorker *renderWorker=mThreadManager->renderWorker();
			if(nullptr!=renderWorker) {
				//renderWorker->
			}
		}
	}

	GLuint pbo() Q_DECL_OVERRIDE{
		GLuint p=0;
		if(nullptr!=mThreadManager)
		{
			CLWorker *renderWorker=mThreadManager->renderWorker();
			if(nullptr!=renderWorker) {
				p=renderWorker->pbo();
			}
		}
		qDebug()<<"TCLGLVR Returning pbo: "<<p;
		return p;
	}

	QString getRendererSpec() Q_DECL_OVERRIDE{
		return "TestCLGLViewRenderer";
	}

};

////////////////////////////////////////////////////////////////////////////////


void TestOpenCL::initTestCase()
{
	mCrashHelper.setEnabled(true);
}



void TestOpenCL::testWithoutGLInterop()
{
	TestCLWorkerFactory twf;
	CLGLInteropConfig ic(nullptr);
	// Just do CPU for maximum portability/availability/debugability
	CLThreadManager tm(twf, ic, "", true, true); // CLWorkerFactory &factory, CLGLInteropConfig config=CLGLInteropConfig(), QString deviceSelectionString="", bool allowGPU=true, bool allowCPU=false, QObject *parent = nullptr
	tm.setRunning(true);
	const quint64 start=utility::currentMsecsSinceEpoch<quint64>();
	quint64 now=start;
	while(now<start+10000) {
		now=utility::currentMsecsSinceEpoch<quint64>();
		qApp->processEvents();
	}
	tm.setRunning(false,true);
}



void TestOpenCL::testWithGLInterop()
{
	QWidget w, *wp=&w;
	w.resize(512,512);
	w.setWindowTitle("TestOpenCL::testWithGLInterop()");
	QVBoxLayout *verticalLayout = OC_NEW QVBoxLayout(&w);
	verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
	QPushButton *pushButtonClose = OC_NEW QPushButton(&w);
	pushButtonClose->setObjectName(QStringLiteral("pushButtonClose"));
	pushButtonClose->setText("CLOSE");
	verticalLayout->addWidget(pushButtonClose);
	qApp->processEvents();
	CLGLView glView;
	CLGLView *glViewp=&glView;
	QSize sz(320,240);
	wp->layout()->addWidget(glViewp);
	qApp->processEvents();
	w.resize(sz);
	qApp->processEvents();
	glView.resize(sz);
	qApp->processEvents();
	TestCLGLViewRenderer testCLGLViewRendrer;
	TestCLGLViewRenderer *testCLGLViewRendrer_p=&testCLGLViewRendrer;


	qDebug()<<"--- GUI COMPLETE ---";
	GLContext shareingContext=glViewp->sharingGLContext();
	qDebug()<<" SHARING CONTEXT WAS "<<shareingContext.toString();
	TestCLWorkerFactory twf;
	CLGLInteropConfig ic(&shareingContext, sz);
	// Just do CPU for maximum portability/availability/debugability
	CLThreadManager tm(twf, ic, "", true, true); // CLWorkerFactory &factory, CLGLInteropConfig config=CLGLInteropConfig(), QString deviceSelectionString="", bool allowGPU=true, bool allowCPU=false, QObject *parent = nullptr
	testCLGLViewRendrer_p->setManager(&tm);

	testCLGLViewRendrer_p->initialize(shareingContext);

	glViewp->setRenderer(testCLGLViewRendrer_p);
	qApp->processEvents();
	w.show();


	testCLGLViewRendrer_p->setRendering(true);
	qDebug()<<"--- WAITING ---";
	const quint64 start=utility::currentMsecsSinceEpoch<quint64>();
	quint64 now=start;
	while(now<start+20000) {
		now=utility::currentMsecsSinceEpoch<quint64>();
		qApp->processEvents();
	}
	qDebug()<<"--- DONE ---";
	testCLGLViewRendrer_p->setRendering(false, true);
	qApp->processEvents();
	qDebug()<<"--- END ---";
}

void TestOpenCL::testCLGLView()
{
	CLGLView glView;
	glView.show();
	qApp->processEvents();
	QTest::qSleep(10000);

}

void TestOpenCL::cleanupTestCase()
{
	mCrashHelper.setEnabled(false);
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
