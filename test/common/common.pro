TEMPLATE=lib
TARGET=common

# Make this library behave like the others
include($$PRIS/common.pri)
include($$PRIS/lib_headers.pri)
# Add the test libraries
QT += testlib
CONFIG += staticlib


SOURCES+= \
	CourierTester.cpp \
    Corruptifier.cpp \
    TestCommon.cpp \
    CPUBurner.cpp \
    GeometryEngine.cpp \
    GLStressWidget.cpp \
    MEMTrasher.cpp \
    Stress.cpp

HEADERS+= \
	CourierTester.hpp \
    Corruptifier.hpp \
    TestCommon.hpp \
    CPUBurner.hpp \
    GeometryEngine.hpp \
    GLStressWidget.hpp \
    MEMTrasher.hpp \
    Stress.hpp


common.depends=		$${SRCS}/libs
