include($$TESTS/test.pri)

TARGET = test_carsteeringwidget

HEADERS += \
	TestCarSteeringWidget.hpp

SOURCES += \
	TestCarSteeringWidget.cpp



FORMS += \
	TestWindow.ui


include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

