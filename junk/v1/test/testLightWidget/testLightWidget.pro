include($$TESTS/test.pri)

TARGET = test_lightwidget

HEADERS += \
	TestLightWidget.hpp \
    LightTestWidget.hpp

SOURCES += \
	TestLightWidget.cpp \
    LightTestWidget.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

FORMS += \
    LightTestWidget.ui

