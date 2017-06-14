include($$TESTS/test.pri)

TARGET = test_nodeassociate

HEADERS += \
	TestNodeAssociate.hpp

SOURCES += \
	TestNodeAssociate.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

