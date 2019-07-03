TEMPLATE=lib
TARGET=common_test

# Make this library behave like the others
include($$PRIS/common.pri)
include($$PRIS/lib_headers.pri)

# Add the test libraries
QT += testlib
CONFIG += staticlib


SOURCES+= \
	Common_test.cpp \
	CommsSignalLogger.cpp \
	CommsTester.cpp \
	Corruptifier.cpp \
	CourierTester.cpp \
	CPUBurner.cpp \
	GeometryEngine.cpp \
	GLStressWidget.cpp \
	Keys_test.cpp \
	MEMTrasher.cpp \
	mock/MockCommsCarrier.cpp \
	mock/MockCommsCarrierLog.cpp \
	mock/MockCourier.cpp \
	Stress.cpp \
	UDPTester.cpp \
	Utility_test.cpp \

HEADERS+= \
	Common_test.hpp \
	CommsSignalLogger.hpp \
	CommsTester.hpp \
	Corruptifier.hpp \
	CourierTester.hpp \
	CPUBurner.hpp \
	GeometryEngine.hpp \
	GLStressWidget.hpp \
	Keys_test.hpp \
	MEMTrasher.hpp \
	mock/MockCommsCarrier.hpp \
	mock/MockCommsCarrierLog.hpp \
	mock/MockCourier.hpp \
	Stress.hpp \
	UDPTester.hpp \
	Utility_test.hpp \

RESOURCES += \
	resources/test_resources.qrc

test_common.depends=		$${SRCS}/libs
