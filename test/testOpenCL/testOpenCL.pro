include($$TOP_PWD/test/test.pri)

QT += core gui opengl widgets
#QT += core gui opengl widgets network multimedia multimediawidgets positioning serialport bluetooth sensors xml svg sql testlib quick printsupport openglextensions

TARGET = test_opencl


HEADERS += \
	TestOpenCL.hpp


SOURCES += \
	TestOpenCL.cpp


RESOURCES += \
	resources/kernels.qrc \

DISTFILES += \
	resources/kernels/metaballs.cl \


include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)





