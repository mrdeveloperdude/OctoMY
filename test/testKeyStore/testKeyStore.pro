include($$TOP_PWD/test/test.pri)

TARGET = test_keystore

HEADERS += \
	TestKeyStore.hpp

SOURCES += \
	TestKeyStore.cpp

include($$TOP_PWD/pri/status.pri)
include($$TOP_PWD/test/test_autorun.pri)
