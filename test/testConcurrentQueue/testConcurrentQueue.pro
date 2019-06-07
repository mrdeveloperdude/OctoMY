include($$TESTS/pris/test.pri)

TARGET = test_concurrentqueue

HEADERS += \
	TestConcurrentQueue.hpp

SOURCES += \
	TestConcurrentQueue.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
