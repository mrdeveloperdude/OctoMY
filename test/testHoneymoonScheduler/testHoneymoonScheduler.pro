include($$TESTS/pris/test.pri)

TARGET = test_honeymoonscheduler

HEADERS += \
	TestHoneymoonScheduler.hpp

SOURCES += \
	TestHoneymoonScheduler.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

