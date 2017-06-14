include($$TOP_PWD/test/test.pri)

TARGET = test_nodeassociatestore

HEADERS += \
	TestNodeAssociateStore.hpp

SOURCES += \
	TestNodeAssociateStore.cpp

include($$TOP_PWD/pri/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

