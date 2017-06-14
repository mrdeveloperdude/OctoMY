

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



