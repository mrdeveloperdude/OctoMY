include($$TESTS/pris/test.pri)

TARGET = test_nodeassociate

HEADERS += \
	TestAssociate.hpp

SOURCES += \
	TestAssociate.cpp

include($$TESTS/pris/test_autorun.pri)
