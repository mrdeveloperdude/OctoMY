include($$TOP_PWD/test/test.pri)

TARGET = test_blob

HEADERS += \
	TestBlob.hpp \
    CourierTester.hpp

SOURCES += \
	TestBlob.cpp \
    CourierTester.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

