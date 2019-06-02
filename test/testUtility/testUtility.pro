include($$TESTS/pris/test.pri)

TARGET = test_utility

QT += network

HEADERS += \
	TestUtility.hpp
	testUtilityNetwork.inc.hpp

SOURCES += \
	TestUtility.cpp \
	testUtilityNetwork.inc.cpp \
	testUtilityRandom.inc.cpp \
	testUtilityTime.inc.cpp \
	testUtilityString.inc.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
