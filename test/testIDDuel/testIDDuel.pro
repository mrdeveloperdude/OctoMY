include($$TESTS/pris/test.pri)

TARGET = test_idduel

HEADERS += \
	TestIDDuel.hpp

SOURCES += \
	TestIDDuel.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

