include($$TESTS/pris/test.pri)

TARGET = test_networksettingswidget

HEADERS += \
	TestNetworkSettingsWidget.hpp \
	NetworkSettingsTestWidget.hpp

SOURCES += \
	TestNetworkSettingsWidget.cpp \
	NetworkSettingsTestWidget.cpp

FORMS += \
	NetworkSettingsTestWidget.ui

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
