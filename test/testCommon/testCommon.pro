#include($$TESTS/test.pri) # DONT DO THIS; IT RECURSES!
include($$PRIS/common.pri)
include($$PRIS/lib.pri)
include($$PRIS/lib_headers.pri)

# This is not a test alone, but rather a collection of useful test code that can be reused among tests
TEMPLATE = lib
CONFIG -= testcase
QT += testlib

TARGET = test_common

HEADERS += \
	TestCommon.hpp \
	CourierTester.hpp \

SOURCES += \
	TestCommon.cpp \
	CourierTester.cpp \

#include($$PRIS/status.pri)

