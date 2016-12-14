include($$TOP_PWD/test/test.pri)

TARGET = test_carsteeringwidget

HEADERS += \
	TestCarSteeringWidget.hpp

SOURCES += \
	TestCarSteeringWidget.cpp



FORMS += \
	TestWindow.ui


include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

