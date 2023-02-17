#ifdef OC_USE_LIB_EXT_OPENCL

#ifndef CLDEVICESELECTION_HPP
#define CLDEVICESELECTION_HPP

#include "CLInclude.hpp"

#include <QString>
#include <QVector>

/**
 * @brief The CLDeviceSelection class is a basic means to tell CLThreadManager
 * on which OpenCL devices we wish to run.
 */

class CLDeviceSelection: public QVector<cl::Device>
{

public:
	explicit CLDeviceSelection(const QString selectString="", bool allowGPU=true, bool allowCPU=false, bool mustSupportGLInterop=true);
	virtual  ~CLDeviceSelection();

};

#endif // CLDEVICESELECTION_HPP

#endif 
// OC_USE_LIB_EXT_OPENCL
