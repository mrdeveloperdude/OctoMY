include($$TESTS/test.pri)

TARGET = test_mapeditor

HEADERS += \
	TestMapEditor.hpp

SOURCES += \
	TestMapEditor.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

