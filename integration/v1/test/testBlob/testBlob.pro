include($$TESTS/test.pri)

TARGET = test_blob

HEADERS += \
	TestBlob.hpp \

SOURCES += \
	TestBlob.cpp \

include($$TESTS/test_autorun.pri)
