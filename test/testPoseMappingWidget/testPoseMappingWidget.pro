include($$TESTS/test.pri)

TARGET = test_posemappingwidget

HEADERS += \
	TestPoseMappingWidget.hpp

SOURCES += \
	TestPoseMappingWidget.cpp

FORMS += \
	TestWindow.ui

include($$TESTS/test_autorun.pri)

