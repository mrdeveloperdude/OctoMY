
CLT_TOP= $$SRCS/libs/libclt

contains(DEFINES, EXTERNAL_LIB_OPENCL){



HEADERS	+= \
	$$CLT_TOP/clt/CLDeviceSelection.hpp \
	$$CLT_TOP/clt/CLGLInteropConfig.hpp \
	$$CLT_TOP/clt/CLInclude.hpp \
	$$CLT_TOP/clt/CLThreadManager.hpp \
	$$CLT_TOP/clt/CLTool.hpp \
	$$CLT_TOP/clt/CLUtils.hpp \
	$$CLT_TOP/clt/CLWorkerFactory.hpp \
	$$CLT_TOP/clt/CLWorker.hpp \
	$$CLT_TOP/clt/CLGLViewRenderer.hpp \
	$$CLT_TOP/hellocl/HelloCLWorkerFactory.hpp \
	$$CLT_TOP/hellocl/HelloCLWorker.hpp \
	$$CLT_TOP/hellocl/HelloGLCLViewRenderer.hpp \



SOURCES	+= \
	$$CLT_TOP/clt/CLDeviceSelection.cpp \
	$$CLT_TOP/clt/CLGLInteropConfig.cpp \
	$$CLT_TOP/clt/CLThreadManager.cpp \
	$$CLT_TOP/clt/CLTool.cpp \
	$$CLT_TOP/clt/CLUtils.cpp \
	$$CLT_TOP/clt/CLWorker.cpp \
	$$CLT_TOP/clt/CLWorkerFactory.cpp \
	$$CLT_TOP/clt/CLGLViewRenderer.cpp \
	$$CLT_TOP/hellocl/HelloCLWorker.cpp \
	$$CLT_TOP/hellocl/HelloCLWorkerFactory.cpp \
	$$CLT_TOP/hellocl/HelloGLCLViewRenderer.cpp \


RESOURCES += \
	$$CLT_TOP/resources/gl_shaders.qrc \

}


#NOTE: These are always included to keep form designs happy


HEADERS	+= \
	$$CLT_TOP/widgets/CLGLView.hpp \

SOURCES	+= \
	$$CLT_TOP/widgets/CLGLView.cpp \
