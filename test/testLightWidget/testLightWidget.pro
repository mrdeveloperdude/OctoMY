include($$TESTS/pris/test.pri)

TARGET = test_lightwidget

HEADERS += \
	TestLightWidget.hpp \
	LightTestWidget.hpp \

SOURCES += \
	TestLightWidget.cpp \
	LightTestWidget.cpp \

FORMS += \
	LightTestWidget.ui

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)


