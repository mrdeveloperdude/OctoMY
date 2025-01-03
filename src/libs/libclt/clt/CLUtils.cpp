#ifdef USE_OC_OCL_STUFF

#include <inttypes.h>
#include <limits>


#include "CLUtils.hpp"
#include "glt/IncludeOpenGL.hpp"

#include "utility/string/String.hpp"

#include <QMap>
#include <QString>
#include <QDebug>
#include <string.h>


QString OCLLocalMemoryTypeString(cl_device_local_mem_type type)
{

#define CL_LOCAL_MEMORY_TYPE_CASE(A) case(A):{ return #A; }break;
	switch(type) {
		CL_LOCAL_MEMORY_TYPE_CASE(CL_LOCAL);
		CL_LOCAL_MEMORY_TYPE_CASE(CL_GLOBAL);
	default: {
		return "Unknwon local memory type "+QString::number(type);
	}
//	break;
	}
#undef CL_LOCAL_MEMORY_TYPE_CASE
}

QString OCLDeviceTypeString(cl_device_type type)
{
#define CL_DEVICE_TYPE_CASE(A, B) case(A):{ return B; }break
	switch(type) {
		CL_DEVICE_TYPE_CASE(CL_DEVICE_TYPE_DEFAULT, "DEFAULT");
		CL_DEVICE_TYPE_CASE(CL_DEVICE_TYPE_CPU, "CPU");
		CL_DEVICE_TYPE_CASE(CL_DEVICE_TYPE_GPU, "GPU");
		CL_DEVICE_TYPE_CASE(CL_DEVICE_TYPE_ACCELERATOR, "ACCELERATOR");
		CL_DEVICE_TYPE_CASE(CL_DEVICE_TYPE_CUSTOM, "CUSTOM");
		CL_DEVICE_TYPE_CASE(CL_DEVICE_TYPE_ALL, "ALL");
	default: {
		return "Unknwon device type string "+QString::number(type);
	}
//	break;
	}
#undef CL_DEVICE_TYPE_CASE
}

QString OCLErrorString(cl_int code)
{
#define CL_ERROR_CODE_CASE(A) case(A):{ return #A; }break;
	switch(code) {
		CL_ERROR_CODE_CASE(CL_SUCCESS);
		CL_ERROR_CODE_CASE(CL_DEVICE_NOT_FOUND);
		CL_ERROR_CODE_CASE(CL_DEVICE_NOT_AVAILABLE);
		CL_ERROR_CODE_CASE(CL_COMPILER_NOT_AVAILABLE);
		CL_ERROR_CODE_CASE(CL_MEM_OBJECT_ALLOCATION_FAILURE);
		CL_ERROR_CODE_CASE(CL_OUT_OF_RESOURCES);
		CL_ERROR_CODE_CASE(CL_OUT_OF_HOST_MEMORY);
		CL_ERROR_CODE_CASE(CL_PROFILING_INFO_NOT_AVAILABLE);
		CL_ERROR_CODE_CASE(CL_MEM_COPY_OVERLAP);
		CL_ERROR_CODE_CASE(CL_IMAGE_FORMAT_MISMATCH);
		CL_ERROR_CODE_CASE(CL_IMAGE_FORMAT_NOT_SUPPORTED);
		CL_ERROR_CODE_CASE(CL_BUILD_PROGRAM_FAILURE);
		CL_ERROR_CODE_CASE(CL_MAP_FAILURE);

#ifdef CL_VERSION_1_1
		CL_ERROR_CODE_CASE(CL_MISALIGNED_SUB_BUFFER_OFFSET);
		CL_ERROR_CODE_CASE(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
#endif

		CL_ERROR_CODE_CASE(CL_COMPILE_PROGRAM_FAILURE);
		CL_ERROR_CODE_CASE(CL_LINKER_NOT_AVAILABLE);
		CL_ERROR_CODE_CASE(CL_LINK_PROGRAM_FAILURE);
		CL_ERROR_CODE_CASE(CL_DEVICE_PARTITION_FAILED);
		CL_ERROR_CODE_CASE(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
		CL_ERROR_CODE_CASE(CL_INVALID_VALUE);
		CL_ERROR_CODE_CASE(CL_INVALID_DEVICE_TYPE);
		CL_ERROR_CODE_CASE(CL_INVALID_PLATFORM);
		CL_ERROR_CODE_CASE(CL_INVALID_DEVICE);
		CL_ERROR_CODE_CASE(CL_INVALID_CONTEXT);
		CL_ERROR_CODE_CASE(CL_INVALID_QUEUE_PROPERTIES);
		CL_ERROR_CODE_CASE(CL_INVALID_COMMAND_QUEUE);
		CL_ERROR_CODE_CASE(CL_INVALID_HOST_PTR);
		CL_ERROR_CODE_CASE(CL_INVALID_MEM_OBJECT);
		CL_ERROR_CODE_CASE(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
		CL_ERROR_CODE_CASE(CL_INVALID_IMAGE_SIZE);
		CL_ERROR_CODE_CASE(CL_INVALID_SAMPLER);
		CL_ERROR_CODE_CASE(CL_INVALID_BINARY);
		CL_ERROR_CODE_CASE(CL_INVALID_BUILD_OPTIONS);
		CL_ERROR_CODE_CASE(CL_INVALID_PROGRAM);
		CL_ERROR_CODE_CASE(CL_INVALID_PROGRAM_EXECUTABLE);
		CL_ERROR_CODE_CASE(CL_INVALID_KERNEL_NAME);
		CL_ERROR_CODE_CASE(CL_INVALID_KERNEL_DEFINITION);
		CL_ERROR_CODE_CASE(CL_INVALID_KERNEL);
		CL_ERROR_CODE_CASE(CL_INVALID_ARG_INDEX);
		CL_ERROR_CODE_CASE(CL_INVALID_ARG_VALUE);
		CL_ERROR_CODE_CASE(CL_INVALID_ARG_SIZE);
		CL_ERROR_CODE_CASE(CL_INVALID_KERNEL_ARGS);
		CL_ERROR_CODE_CASE(CL_INVALID_WORK_DIMENSION);
		CL_ERROR_CODE_CASE(CL_INVALID_WORK_GROUP_SIZE);
		CL_ERROR_CODE_CASE(CL_INVALID_WORK_ITEM_SIZE);
		CL_ERROR_CODE_CASE(CL_INVALID_GLOBAL_OFFSET);
		CL_ERROR_CODE_CASE(CL_INVALID_EVENT_WAIT_LIST);
		CL_ERROR_CODE_CASE(CL_INVALID_EVENT);
		CL_ERROR_CODE_CASE(CL_INVALID_OPERATION);
		CL_ERROR_CODE_CASE(CL_INVALID_GL_OBJECT);
		CL_ERROR_CODE_CASE(CL_INVALID_BUFFER_SIZE);
		CL_ERROR_CODE_CASE(CL_INVALID_MIP_LEVEL);
		CL_ERROR_CODE_CASE(CL_INVALID_GLOBAL_WORK_SIZE);
		CL_ERROR_CODE_CASE(CL_INVALID_PROPERTY);
		CL_ERROR_CODE_CASE(CL_INVALID_IMAGE_DESCRIPTOR);
		CL_ERROR_CODE_CASE(CL_INVALID_COMPILER_OPTIONS);
		CL_ERROR_CODE_CASE(CL_INVALID_LINKER_OPTIONS);
		CL_ERROR_CODE_CASE(CL_INVALID_DEVICE_PARTITION_COUNT);
		CL_ERROR_CODE_CASE(CL_INVALID_PIPE_SIZE);
		CL_ERROR_CODE_CASE(CL_INVALID_DEVICE_QUEUE);

		// extension errors
		CL_ERROR_CODE_CASE(CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR);
		CL_ERROR_CODE_CASE(CL_PLATFORM_NOT_FOUND_KHR);
//		CL_ERROR_CODE_CASE(CL_INVALID_D3D10_DEVICE_KHR);
//		CL_ERROR_CODE_CASE(CL_INVALID_D3D10_RESOURCE_KHR);
//		CL_ERROR_CODE_CASE(CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR);
//		CL_ERROR_CODE_CASE(CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR);

	default: {
		return "Unknwon errorcode "+QString::number(code);
	}
//	break;
	}
#undef CL_ERROR_CODE_CASE
}


void failOnCLError(cl_int code, QString msg, QString file, int line)
{
	if(CL_SUCCESS==code) {
		return;
	}
	QString error=OCLErrorString(code);
	qWarning()<<"CL: ERROR: "<<error<<"("<<code<<") for "<<msg<<" @"<<file<<":"<<line;
	//exit(1);
}




QString CLDeviceToString(const cl::Device *dev)
{
	if(nullptr==dev) {
		return "NULL";
	}
	//std::string sname=dev.getInfo<CL_DEVICE_NAME>();
	const QString name=QString::fromLocal8Bit(dev->getInfo<CL_DEVICE_NAME>().c_str());
	const QString profileName=QString::fromLocal8Bit(dev->getInfo<CL_DEVICE_PROFILE>().c_str());
	//	QString::fromLocal8Bit(sname.c_str());
	const cl_bool available= dev->getInfo<CL_DEVICE_AVAILABLE>();
	const cl_bool compailerAvailable= dev->getInfo<CL_DEVICE_COMPILER_AVAILABLE>();
	const cl_bool errorCorrection= dev->getInfo<CL_DEVICE_ERROR_CORRECTION_SUPPORT>();
	const QString endianess=dev->getInfo<CL_DEVICE_ENDIAN_LITTLE>()?"Little, ":"Big, ";
	const QString extensions=QString::fromLocal8Bit(dev->getInfo<CL_DEVICE_EXTENSIONS>().c_str());
	const bool interopAvailable=extensions.contains("cl_khr_gl_sharing");

	return name //Device name
		   + " ( "+OCLDeviceTypeString(dev->getInfo<CL_DEVICE_TYPE>()) +" x " //Type
		   + QString::number(dev->getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>())+" ) Memory: " // Number of cores
		   + utility::string::humanReadableSize(dev->getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>(), 2) + " Global, "
		   + utility::string::humanReadableSize(dev->getInfo<CL_DEVICE_LOCAL_MEM_SIZE>(), 2) + (CL_GLOBAL==dev->getInfo<CL_DEVICE_LOCAL_MEM_TYPE>()?" Global, ":" Local, ")
		   + utility::string::humanReadableSize(dev->getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>(), 2) +" Constant"
		   + " FLAGS["+(available?"Available, ":" --------, ")+(compailerAvailable?"Compiler, ":" -------, ")+(errorCorrection?"ErrCorr, ":" ------, ")+ endianess+(interopAvailable?"Interop":" ------")+"]"
		   + " profile= "+profileName;
}

/*
  dev.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &i);
		std::cout << "\t\tMax. Compute Units: " << i << std::endl;

		size_t size;
		dev.getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &size);
		std::cout << "\t\tLocal Memory Size: " << size/1024 << " KB" << std::endl;

		dev.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &size);
		std::cout << "\t\tGlobal Memory Size: " << size/(1024*1024) << " MB" << std::endl;

		dev.getInfo(CL_DEVICE_MAX_MEM_ALLOC_SIZE, &size);
		std::cout << "\t\tMax Alloc Size: " << size/(1024*1024) << " MB" << std::endl;

		dev.getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &size);
		std::cout << "\t\tMax Work-group Size: " << size << std::endl;

		std::vector<size_t> d;
		dev.getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &d);
		std::cout << "\t\tMax Work-item Dims: (";
		for (size_t& st : d)
		  std::cout << st << " ";
		std::cout << "\x08)" << std::endl;
*/

QDebug &operator<<(QDebug &d, cl::Device &dev)
{
	QString s=CLDeviceToString(&dev);
	d.noquote().operator<<(s);
	return d;
}


QDebug &operator<<(QDebug &d, cl::Device *dev)
{
	QString s=CLDeviceToString(dev);
	d.noquote().operator<<(s);
	return d;
}


QSurfaceFormat properOctomyDefaultFormat()
{
	QSurfaceFormat defFormat=QSurfaceFormat::defaultFormat();
	QSurfaceFormat format = defFormat;
	format.setVersion( OCTOMY_QT_OGL_VERSION_MAJOR, OCTOMY_QT_OGL_VERSION_MINOR );
	format.setProfile( QSurfaceFormat::OCTOMY_QT_OGL_SURFACE_PROFILE );
	format.setOption(QSurfaceFormat::DebugContext);
	format.setDepthBufferSize(OCTOMY_QT_OGL_DEPTH_BUFFER);
	format.setStencilBufferSize(OCTOMY_QT_OGL_STENSIL_BUFFER);
	format.setSwapBehavior(QSurfaceFormat::OCTOMY_QT_OGL_SWAP_BEHAVIOUR);
	format.setSwapInterval(OCTOMY_QT_OGL_SWAP_INTERVAL);
	format.setRenderableType(QSurfaceFormat::OCTOMY_QT_OGL_RENDERABLE_TYPE);
	// Log stuff
	qDebug().noquote().nospace()<<"        OCTOMY_QT_OGL_VERSION: "<< OCTOMY_QT_OGL_VERSION_MAJOR << "." << OCTOMY_QT_OGL_VERSION_MINOR << ",\tformat= "<<format.version().first<<"."<<format.version().second <<"  vs. DEFAULT:\t"<<defFormat.version().first<<"."<<defFormat.version().second;
	qDebug().noquote().nospace()<<"        OCTOMY_QT_OGL_PROFILE: "<< OCTOMY_QT_OGL_PROFILE_STR <<	",\tformat= "<< format.profile() <<		    " vs. DEFAULT:\t"<<defFormat.profile();
	qDebug().noquote().nospace()<<"   OCTOMY_QT_OGL_DEPTH_BUFFER: "<< OCTOMY_QT_OGL_DEPTH_BUFFER<<	",\tformat= "<< format.depthBufferSize() <<	" vs. DEFAULT:\t"<<defFormat.depthBufferSize();
	qDebug().noquote().nospace()<<" OCTOMY_QT_OGL_STENSIL_BUFFER: "<< STRINGIFY(OCTOMY_QT_OGL_STENSIL_BUFFER) << ",\tformat= "<< format.stencilBufferSize() <<" vs. DEFAULT:\t"<<defFormat.stencilBufferSize();
	qDebug().noquote().nospace()<<" OCTOMY_QT_OGL_SWAP_BEHAVIOUR: "<< STRINGIFY(OCTOMY_QT_OGL_SWAP_BEHAVIOUR) <<  ",\tformat= "<< format.swapBehavior() <<     " vs. DEFAULT:\t"<<defFormat.swapBehavior();
	qDebug().noquote().nospace()<<"  OCTOMY_QT_OGL_SWAP_INTERVAL: "<< OCTOMY_QT_OGL_SWAP_INTERVAL<<  ",\tformat= "<< format.swapInterval() <<     " vs. DEFAULT:\t"<<defFormat.swapInterval();
	qDebug().noquote().nospace()<<"OCTOMY_QT_OGL_RENDERABLE_TYPE: "<< STRINGIFY(OCTOMY_QT_OGL_RENDERABLE_TYPE)<<  ",\tformat= "<< format.renderableType() <<     " vs. DEFAULT:\t"<<defFormat.renderableType();
	// Done
	qDebug()<<"PROPER OCTOMY FORMAT WAS: "<<format;
	return format;
}

#endif
// USE_OC_OCL_STUFF
