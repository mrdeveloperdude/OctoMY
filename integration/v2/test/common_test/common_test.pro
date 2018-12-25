TEMPLATE=lib
TARGET=common_test

# Make this library behave like the others
include($$PRIS/common.pri)
include($$PRIS/lib_headers.pri)

# Add the test libraries
QT += testlib
CONFIG += staticlib


SOURCES+= \
#	CourierTester.cpp \
	Common_test.cpp \
	Corruptifier.cpp \
	CPUBurner.cpp \
	GeometryEngine.cpp \
	GLStressWidget.cpp \
	Keys_test.cpp \
	MEMTrasher.cpp \
	Stress.cpp \
	UDPTester.cpp \
	Utility_test.cpp \


HEADERS+= \
#	CourierTester.hpp \
	Common_test.hpp \
	Corruptifier.hpp \
	CPUBurner.hpp \
	GeometryEngine.hpp \
	GLStressWidget.hpp \
	Keys_test.hpp \
	MEMTrasher.hpp \
	Stress.hpp \
	UDPTester.hpp \
	Utility_test.hpp \


test_common.depends=		$${SRCS}/libs
