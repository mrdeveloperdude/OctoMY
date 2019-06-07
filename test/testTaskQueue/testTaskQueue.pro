include($$TESTS/pris/test.pri)

TARGET = test_taskqueue

HEADERS += \
	TestTaskQueue.hpp

SOURCES += \
	TestTaskQueue.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
