#ifndef CLDEVICESELECTION_HPP
#define CLDEVICESELECTION_HPP

#include "CLInclude.hpp"

#include <QString>
#include <QVector>

class CLDeviceSelection: public QVector<cl::Device>
{

public:
	explicit CLDeviceSelection(const QString selectString="", bool allowGPU=true, bool allowCPU=false);
	virtual  ~CLDeviceSelection();

};

#endif // CLDEVICESELECTION_HPP
