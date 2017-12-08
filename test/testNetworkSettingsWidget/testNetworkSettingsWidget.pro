include($$TESTS/test.pri)

TARGET = test_networksettingswidget

HEADERS += \
	TestNetworkSettingsWidget.hpp \
    NetworkSettingsTestWidget.hpp

SOURCES += \
	TestNetworkSettingsWidget.cpp \
    NetworkSettingsTestWidget.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

FORMS += \
    NetworkSettingsTestWidget.ui

