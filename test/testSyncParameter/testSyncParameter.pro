include($$TESTS/test.pri)

TARGET = test_syncparameter

HEADERS += \
	TestSyncParameter.hpp

SOURCES += \
	TestSyncParameter.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

