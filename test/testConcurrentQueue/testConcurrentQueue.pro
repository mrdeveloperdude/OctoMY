include($$TESTS/test.pri)

TARGET = test_concurrentqueue

HEADERS += \
	TestConcurrentQueue.hpp

SOURCES += \
	TestConcurrentQueue.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)
