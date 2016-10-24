#ifndef CLUTILS_HPP
#define	CLUTILS_HPP

#include <QString>
#include "opencl/CLInclude.hpp"

extern QString OCLLocalMemoryTypeString(cl_device_local_mem_type type);
extern QString OCLDeviceTypeString(cl_device_type type);
extern QString OCLErrorString(cl_int error);
extern void failOnCLError(cl_int code, QString msg="", QString file="", int line=0);

extern QString CLDeviceToString(cl::Device &dev);
#define CL_DETECT_ERROR(A)  qDebug()<<"CL: "<<#A; failOnCLError(A, #A, __FILE__, __LINE__)


extern QDebug &operator<<(QDebug &d, cl::Device &dev);

#endif //CLUTILS_HPP
