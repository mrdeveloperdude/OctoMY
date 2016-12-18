include($$TOP_PWD/test/test.pri)

TARGET = test_posemappingwidget

HEADERS += \
	TestPoseMappingWidget.hpp

SOURCES += \
	TestPoseMappingWidget.cpp

FORMS += \
	TestWindow.ui

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

