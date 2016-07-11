include($$TOP_PWD/test/test.pri)

TARGET = test_tetragpsencoder

HEADERS += \
	TestTetraGPSEncoder.hpp

SOURCES += \
	TestTetraGPSEncoder.cpp

include($$TOP_PWD/status.pri)
include($$TOP_PWD/test/test_autorun.pri)
