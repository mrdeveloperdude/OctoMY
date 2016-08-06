include($$TOP_PWD/test/test.pri)

TARGET = test_nodeassociate

HEADERS += \
	TestNodeAssociate.hpp

SOURCES += \
	TestNodeAssociate.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)

