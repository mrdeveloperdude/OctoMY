include($$TESTS/pris/test.pri)

TARGET = test_posemappingwidget

HEADERS += \
	TestPoseMappingWidget.hpp

SOURCES += \
	TestPoseMappingWidget.cpp

FORMS += \
	TestWindow.ui

include($$TESTS/pris/test_autorun.pri)

