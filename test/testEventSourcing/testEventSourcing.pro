include($$TOP_PWD/test/test.pri)

TARGET = test_eventsourcing

HEADERS += \
	TestEventSourcing.hpp

SOURCES += \
	TestEventSourcing.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

