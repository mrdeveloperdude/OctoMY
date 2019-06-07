include($$TESTS/pris/test.pri)

TARGET = test_mapeditor

HEADERS += \
	TestMapEditor.hpp

SOURCES += \
	TestMapEditor.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
