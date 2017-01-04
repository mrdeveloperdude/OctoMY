TARGET =	clt
TEMPLATE =	lib
CONFIG +=	staticlib

INCLUDEPATH += ./

HEADERS	+= \
	opencl/CLThreadManager.hpp \
	opencl/CLDeviceSelection.hpp \
	opencl/CLWorker.hpp \
	opencl/CLUtils.hpp \
	opencl/CLInclude.hpp \
	widgets/CLGLViewRenderer.hpp \
	opencl/CLWorkerFactory.hpp \
	opencl/CLGLInteropConfig.hpp

SOURCES	+= \
	opencl/CLThreadManager.cpp \
	opencl/CLDeviceSelection.cpp \
	opencl/CLWorker.cpp \
	opencl/CLUtils.cpp \
	widgets/CLGLViewRenderer.cpp \
	opencl/CLWorkerFactory.cpp \
	opencl/CLGLInteropConfig.cpp


RESOURCES += \
	resources/gl_shaders.qrc \

include($$TOP_PWD/common.pri)
