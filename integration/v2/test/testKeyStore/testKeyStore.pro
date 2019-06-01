include($$TESTS/pris/test.pri)

TARGET = test_keystore

HEADERS += \
	TestKeyStore.hpp

SOURCES += \
	TestKeyStore.cpp

include($$TESTS/test_autorun.pri)
