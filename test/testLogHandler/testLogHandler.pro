include($$TESTS/pris/test.pri)

TARGET = test_loghandler

HEADERS += \
	TestLogHandler.hpp

SOURCES += \
	TestLogHandler.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

