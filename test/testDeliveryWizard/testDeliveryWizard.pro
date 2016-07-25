include($$TOP_PWD/test/test.pri)

TARGET = test_deliverywizard

HEADERS += \
	TestDeliveryWizard.hpp

SOURCES += \
	TestDeliveryWizard.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

