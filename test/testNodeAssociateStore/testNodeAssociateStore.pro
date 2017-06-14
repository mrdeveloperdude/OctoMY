include($$TESTS/test.pri)

TARGET = test_nodeassociatestore

HEADERS += \
	TestNodeAssociateStore.hpp

SOURCES += \
	TestNodeAssociateStore.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

