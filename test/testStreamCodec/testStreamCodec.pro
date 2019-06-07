include($$TESTS/pris/test.pri)

TARGET = test_streamcodec

HEADERS += \
	TestStreamCodec.hpp

SOURCES += \
	TestStreamCodec.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
