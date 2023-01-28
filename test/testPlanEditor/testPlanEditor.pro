include($$TESTS/pris/test.pri)

TARGET = test_planeditor

HEADERS += \
	TestPlanEditor.hpp

SOURCES += \
	TestPlanEditor.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
