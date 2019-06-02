include($$TESTS/test.pri)

TARGET = test_streamcodec

HEADERS += \
	TestStreamCodec.hpp

SOURCES += \
	TestStreamCodec.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

