include($$TOP_PWD/test/test.pri)

TARGET = test_commschannel

HEADERS += \
	TestCommsChannel.hpp

SOURCES += \
	TestCommsChannel.cpp

include($$TOP_PWD/pri/status.pri)
#include($$TOP_PWD/test/test_autorun.pri)

