include($$TESTS/pris/test.pri)

QT += core gui opengl widgets
#QT += core gui opengl widgets network multimedia multimediawidgets positioning serialport bluetooth sensors xml svg sql testlib quick printsupport openglextensions

TARGET = test_opencl

HEADERS += \
	TestCLGLViewRenderer.hpp \
	TestCLWorker.hpp \
	TestCLWorkerFactory.hpp \
	TestOpenCL.hpp \

SOURCES += \
	TestCLGLViewRenderer.cpp \
	TestCLWorker.cpp \
	TestCLWorkerFactory.cpp \
	TestOpenCL.cpp \

RESOURCES += \
	resources/kernels.qrc \

DISTFILES += \
	resources/kernels/metaballs.cl \

include($$TESTS/pris/test_autorun.pri)
