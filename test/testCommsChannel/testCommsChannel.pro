include($$TESTS/pris/test.pri)

TARGET = test_commschannel

HEADERS += \
	TestCommsChannel.hpp \

SOURCES += \
	TestCommsChannel.cpp \
	testMultiple.cpp \
	testSingle.cpp \
	testCommsMock.cpp \

include($$TESTS/pris/test_autorun.pri)

