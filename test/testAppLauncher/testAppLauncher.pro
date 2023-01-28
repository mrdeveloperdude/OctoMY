include($$TESTS/pris/test.pri)

TARGET = test_applauncher

HEADERS += \
	TestAppLauncher.hpp

SOURCES += \
	TestAppLauncher.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

