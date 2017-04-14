#ifndef CLDEVICESELECTION_HPP
#define CLDEVICESELECTION_HPP

#include "CLInclude.hpp"

#include <QString>
#include <QVector>

/**
 * @brief The CLDeviceSelection class is a basic means to tell CLThread Manager
 * on which OpenCL devices we wish to run.
 */

class CLDeviceSelection: public QVector<cl::Device>
{

public:
	explicit CLDeviceSelection(const QString selectString="", bool allowGPU=true, bool allowCPU=false, bool mustSupportGLInterop=true);
	virtual  ~CLDeviceSelection();

};

#endif // CLDEVICESELECTION_HPP
