include($$TESTS/pris/test.pri)

TARGET = test_carsteeringwidget

HEADERS += \
	TestCarSteeringWidget.hpp

SOURCES += \
	TestCarSteeringWidget.cpp


FORMS += \
	TestWindow.ui

include($$TESTS/pris/test_autorun.pri)
