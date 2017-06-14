include($$TESTS/test.pri)

TARGET = test_keystore

HEADERS += \
	TestKeyStore.hpp

SOURCES += \
	TestKeyStore.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)
