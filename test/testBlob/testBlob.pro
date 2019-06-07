include($$TESTS/pris/test.pri)

TARGET = test_blob

HEADERS += \
	TestBlob.hpp \

SOURCES += \
	TestBlob.cpp \

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
