#include "CLDeviceSelection.hpp"

#include "CLUtils.hpp"

#include <QDebug>



CLDeviceSelection::CLDeviceSelection(const QString selectString, bool allowGPU, bool allowCPU, bool mustSupportGLInterop)
{
	// Get all platforms available to us on this system
	VECTOR_CLASS<cl::Platform> allPlatforms;
	CL_DETECT_ERROR(cl::Platform::get(&allPlatforms));
	int selectIndex = 0;
	qDebug().nospace().noquote()<<"Making OpenCL device selection with selectstring='"<< selectString<< "', allowGPU="<<allowGPU<<", allowCPU="<<allowCPU<<"";
	for (size_t i = 0; i < allPlatforms.size(); ++i) {
		qDebug().nospace().noquote()<<"Platform-" << i << ": " << QString::fromLocal8Bit(allPlatforms[i].getInfo<CL_PLATFORM_VENDOR>().c_str());

		// Get the list of devices available on the platform
		VECTOR_CLASS<cl::Device> devices;
		allPlatforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices);

		for (size_t j = 0; j < devices.size(); ++j) {
			bool selected = false;
			const cl_device_type type=devices[j].getInfo<CL_DEVICE_TYPE>();
			bool interop_ok=false;
			if(mustSupportGLInterop) {
				const cl::STRING_CLASS extStd=devices[j].getInfo<CL_DEVICE_EXTENSIONS>();
				const QString extensions=QString::fromStdString(extStd);
				qDebug()<<"EXTENSIONS: "<<extensions;
				if(extensions.contains("cl_khr_gl_sharing")) {
					interop_ok=true;
				}
			} else {
				interop_ok=true;
			}

			if(interop_ok) {
				if ((allowGPU && (type == CL_DEVICE_TYPE_GPU)) || (allowCPU && (type == CL_DEVICE_TYPE_CPU))) {
					if (selectString.length() == 0) {
						selected = true;
					} else {
						if (selectString.length() <= selectIndex) {
							qWarning()<< "OpenCL select devices string (opencl.devices.select) has the wrong length";
							exit(1);
						}

						if (selectString.at(selectIndex) == '1') {
							selected = true;
						}
					}
				}
			}

			if (selected) {
				push_back(devices[j]);
			}

			qDebug().nospace()<< " |-- Device-"<<i <<"."<<j <<": "<< devices[j]<<(selected?" [SEL]":" [---]");
			++selectIndex;

		}
	}

	if (size() == 0) {
		qWarning()<<"This program requires OpenCL enabled hardware. Unable to find any OpenCL GPU devices, so quitting";
		exit(1);
	}
}

CLDeviceSelection::~CLDeviceSelection()
{

}
