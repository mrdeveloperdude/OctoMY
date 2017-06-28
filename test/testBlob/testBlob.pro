include($$TESTS/test.pri)

TARGET = test_blob

HEADERS += \
	TestBlob.hpp \

SOURCES += \
	TestBlob.cpp \


include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

