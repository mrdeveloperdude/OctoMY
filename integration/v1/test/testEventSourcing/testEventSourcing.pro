include($$TESTS/test.pri)

TARGET = test_eventsourcing

HEADERS += \
	TestEventSourcing.hpp

SOURCES += \
	TestEventSourcing.cpp

include($$TESTS/test_autorun.pri)

