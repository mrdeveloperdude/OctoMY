TARGET =	clt
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)

INCLUDEPATH += ./

HEADERS	+= \
	opencl/CLThreadManager.hpp \
	opencl/CLDeviceSelection.hpp \
	opencl/CLWorker.hpp \
	opencl/CLUtils.hpp \
	opencl/CLInclude.hpp \
	widgets/CLGLViewRenderer.hpp \
	opencl/CLWorkerFactory.hpp \

SOURCES	+= \
	opencl/CLThreadManager.cpp \
	opencl/CLDeviceSelection.cpp \
	opencl/CLWorker.cpp \
	opencl/CLUtils.cpp \
	widgets/CLGLViewRenderer.cpp \
	opencl/CLWorkerFactory.cpp \


RESOURCES += \
	resources/gl_shaders.qrc \
