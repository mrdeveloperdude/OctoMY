include($$TESTS/pris/test.pri)

TARGET = test_hardwarewidget

HEADERS += \
	TestHardwareWidget.hpp

SOURCES += \
	TestHardwareWidget.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

