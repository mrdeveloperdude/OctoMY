include($$TESTS/pris/test.pri)

TARGET = test_irisrendrer

HEADERS += \
	TestIrisRendrer.hpp \
	IrisRendrerTestWidget.hpp \

SOURCES += \
	TestIrisRendrer.cpp \
	IrisRendrerTestWidget.cpp \

FORMS += \
	IrisRendrerTestWidget.ui \

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

