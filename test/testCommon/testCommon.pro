#include($$TOP_PWD/test/test.pri) # DONT DO THIS; IT RECURSES!
include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)
include($$TOP_PWD/libs/libs_inc.pri)

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

#include($$TOP_PWD/status.pri)

