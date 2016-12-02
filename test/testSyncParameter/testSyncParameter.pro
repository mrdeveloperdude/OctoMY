include($$TOP_PWD/test/test.pri)

TARGET = test_syncparameter

HEADERS += \
	TestSyncParameter.hpp

SOURCES += \
	TestSyncParameter.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

