include($$TOP_PWD/test/test.pri)

TARGET = test_posemappingstore

HEADERS += \
	TestPoseMappingStore.hpp

SOURCES += \
	TestPoseMappingStore.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

