TARGET =	clt
TEMPLATE =	lib
CONFIG +=	staticlib

contains(DEFINES, EXTERNAL_LIB_OPENCL){

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)

QT         += core core-private gui gui-private
INCLUDEPATH += ./

HEADERS	+= \
	opencl/CLThreadManager.hpp \
	opencl/CLDeviceSelection.hpp \
	opencl/CLWorker.hpp \
	opencl/CLUtils.hpp \
	opencl/CLInclude.hpp \
	widgets/CLGLViewRenderer.hpp \
	opencl/CLWorkerFactory.hpp \
	opencl/CLGLInteropConfig.hpp \
	widgets/HelloGLCLViewRenderer.hpp \
	widgets/HelloCLWorkerFactory.hpp \
	widgets/HelloCLWorker.hpp \


SOURCES	+= \
	opencl/CLThreadManager.cpp \
	opencl/CLDeviceSelection.cpp \
	opencl/CLWorker.cpp \
	opencl/CLUtils.cpp \
	widgets/CLGLViewRenderer.cpp \
	opencl/CLWorkerFactory.cpp \
	opencl/CLGLInteropConfig.cpp \
	widgets/HelloGLCLViewRenderer.cpp \
	widgets/HelloCLWorkerFactory.cpp \
	widgets/HelloCLWorker.cpp \



RESOURCES += \
	resources/gl_shaders.qrc \


#INCLUDEPATH+="./OpenCL-ICD-Loader/inc"
#LIBS+= -L./OpenCL-ICD-Loader/build/bin
INCLUDEPATH+="/usr/local/include/CL"
LIBS+= -L$$TOP_PWD/libs/libclt/OpenCL-ICD-Loader/build/bin
LIBS+= -lOpenCL
PRE_TARGETDEPS += $$TOP_PWD/libs/libclt/OpenCL-ICD-Loader/build/bin/libOpenCL.so


}
