CLT_TOP= $$SRCS/libs/libclt

HEADERS	+= \
	$$CLT_TOP/opencl/CLThreadManager.hpp \
	$$CLT_TOP/opencl/CLDeviceSelection.hpp \
	$$CLT_TOP/opencl/CLWorker.hpp \
	$$CLT_TOP/opencl/CLUtils.hpp \
	$$CLT_TOP/opencl/CLInclude.hpp \
	$$CLT_TOP/widgets/CLGLViewRenderer.hpp \
	$$CLT_TOP/opencl/CLWorkerFactory.hpp \
	$$CLT_TOP/opencl/CLGLInteropConfig.hpp \
	$$CLT_TOP/widgets/HelloGLCLViewRenderer.hpp \
	$$CLT_TOP/widgets/HelloCLWorkerFactory.hpp \
	$$CLT_TOP/widgets/HelloCLWorker.hpp \


SOURCES	+= \
	$$CLT_TOP/opencl/CLThreadManager.cpp \
	$$CLT_TOP/opencl/CLDeviceSelection.cpp \
	$$CLT_TOP/opencl/CLWorker.cpp \
	$$CLT_TOP/opencl/CLUtils.cpp \
	$$CLT_TOP/widgets/CLGLViewRenderer.cpp \
	$$CLT_TOP/opencl/CLWorkerFactory.cpp \
	$$CLT_TOP/opencl/CLGLInteropConfig.cpp \
	$$CLT_TOP/widgets/HelloGLCLViewRenderer.cpp \
	$$CLT_TOP/widgets/HelloCLWorkerFactory.cpp \
	$$CLT_TOP/widgets/HelloCLWorker.cpp \


RESOURCES += \
	$$CLT_TOP/resources/gl_shaders.qrc \



