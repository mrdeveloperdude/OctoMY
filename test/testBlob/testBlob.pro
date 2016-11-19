include($$TOP_PWD/test/test.pri)

TARGET = test_blob

HEADERS += \
	TestBlob.hpp

SOURCES += \
	TestBlob.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

