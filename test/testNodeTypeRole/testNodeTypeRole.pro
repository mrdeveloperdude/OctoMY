include($$TESTS/pris/test.pri)

TARGET = test_nodetyperole

HEADERS += \
	TestNodeTypeRole.hpp

SOURCES += \
	TestNodeTypeRole.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

