include($$TESTS/pris/test.pri)

TARGET = test_eventsourcing

HEADERS += \
	TestEventSourcing.hpp

SOURCES += \
	TestEventSourcing.cpp

include($$TESTS/pris/test_autorun.pri)

