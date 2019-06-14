#include "TestCLWorker.hpp"

#include "clt/CLThreadManager.hpp"
#include "clt/CLUtils.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <iostream>


template<typename T>
constexpr static T map(T x, T xmin, T xmax, T ymin, T ymax)
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



TestCLWorker::TestCLWorker(CLThreadManager &man, int index)
	: CLWorker(man, index, nullptr)
	, limit(0.0)
	, applyLimit(false)
	, charge(0.0)
{
	OC_METHODGATE();
	qDebug()<<"TCLW Ctor INDEX "<<index;
}


TestCLWorker::~TestCLWorker()
{
	OC_METHODGATE();
	qDebug()<<"TCLW Dtor";
}


void TestCLWorker::initCLStuff()
{
	OC_METHODGATE();
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


void TestCLWorker::loadCLProgram()
{
	OC_METHODGATE();
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
					QVERIFY(false);
					// exit(1);
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


void TestCLWorker::preparePoints()
{
	OC_METHODGATE();
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


void TestCLWorker::calculateTexture()
{
	OC_METHODGATE();
	QMutexLocker lock(&sourceChangeMutex);
	auto clctx=*clContext();
	//qDebug()<<"calculateTexture() ";
	const unsigned long num = static_cast<unsigned long>(sources.size());
	// Create a memory buffer on the GPU
	cl::Buffer bufferSources = cl::Buffer(clctx, CL_MEM_READ_ONLY, num * sizeof(cl_int2));
	// Create a memory buffer on the CPU
	cl_int2 *sourcePoints = OC_NEW cl_int2[num];
	// Fill the buffer with the coordinates of the source points
	for (int i = 0; i < static_cast<int>(num); i++) {
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
	cl::NDRange global(static_cast<unsigned long>(size.width()), static_cast<unsigned long>(size.height()));
	CL_DETECT_ERROR(queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, cl::NullRange));
	CL_DETECT_ERROR(queue.finish());
}


void TestCLWorker::changeMousePoint(QPoint point)
{
	OC_METHODGATE();
	QMutexLocker lock(&sourceChangeMutex);
	if(!sources.isEmpty()) {
		sources.pop_back( );
	}
	const QSize size=manager().interopConfig().size();
	sources.push_back(map(point, QRect(0, 0, point.x(), point.y()), QRect(0, 0, size.width(), size.height())));
}


void TestCLWorker::preProcessImp()
{
	OC_METHODGATE();
	qDebug()<<"TCLW PRE PROCESS WORKER START WITH INDEX "<<index()<< " IN THREAD "<<QThread::currentThreadId();
	qDebug()<<"TCLW PRE PROCESS WORKER END WITH INDEX "<<index()<< " IN THREAD "<<QThread::currentThreadId();
}


void TestCLWorker::processImp()
{
	OC_METHODGATE();
	qDebug()<<"TCLW PROCESS WORKER START WITH INDEX "<<index()<< " IN THREAD "<<QThread::currentThreadId();
	initCLStuff();
	loadCLProgram();
	preparePoints();
	while(isRunning()) {
		calculateTexture();
		qApp->processEvents();
	}
	qDebug()<<"TCLW PROCESS WORKER END WITH INDEX "<<index()<< " IN THREAD "<<QThread::currentThreadId();
}


void TestCLWorker::postProcessImp()
{
	OC_METHODGATE();
	qDebug()<<"TCLW POST PROCESS WORKER START WITH INDEX "<<index()<< " IN THREAD "<<QThread::currentThreadId();
	qDebug()<<"TCLW POST PROCESS WORKER END WITH INDEX "<<index()<< " IN THREAD "<<QThread::currentThreadId();
}

