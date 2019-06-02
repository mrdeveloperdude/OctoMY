include($$TESTS/pris/test.pri)

TARGET = test_tetragpsencoder

HEADERS += \
	TestTetraGPSEncoder.hpp

SOURCES += \
	TestTetraGPSEncoder.cpp

include($$TESTS/test_autorun.pri)
