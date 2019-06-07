include($$TESTS/pris/test.pri)

TARGET = test_keystore

HEADERS += \
	TestKeyStore.hpp

SOURCES += \
	TestKeyStore.cpp

include($$TESTS/pris/test_autorun.pri)
