include($$TESTS/pris/test.pri)

TARGET = test_store

HEADERS += \
	Store.hpp \
	TestStore.hpp

SOURCES += \
	Store.cpp \
	TestStore.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

