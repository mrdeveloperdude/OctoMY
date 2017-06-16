include($$TESTS/stress.pri)

TARGET = stress_template

HEADERS += \
	StressTemplate.hpp

SOURCES += \
	StressTemplate.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

