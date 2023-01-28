include($$TESTS/test.pri)

TARGET = test_hardwarewidget

HEADERS += \
	TestHardwareWidget.hpp

SOURCES += \
	TestHardwareWidget.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

