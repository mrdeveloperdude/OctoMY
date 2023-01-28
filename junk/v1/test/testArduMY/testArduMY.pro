include($$TESTS/test.pri)

TARGET = test_ardumy

HEADERS += \
	testActuatorConfigEquals.inc.hpp \
	testActuatorConfigParser.inc.hpp \
	testActuatorConfigSerializer.inc.hpp \
	testActuatorValueEquals.inc.hpp \
	testActuatorValueParser.inc.hpp \
	testActuatorValueSerializer.inc.hpp \
	TestArduMY.hpp \

SOURCES += \
	TestArduMY.cpp \

include($$TESTS/test_autorun.pri)
