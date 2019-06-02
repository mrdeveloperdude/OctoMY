include($$TESTS/pris/test.pri)

TARGET = test_syncparameter

HEADERS += \
	TestSyncParameter.hpp

SOURCES += \
	TestSyncParameter.cpp

include($$TESTS/test_autorun.pri)
