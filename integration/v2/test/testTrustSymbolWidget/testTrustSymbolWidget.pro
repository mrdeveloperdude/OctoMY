include($$TESTS/pris/test.pri)

TARGET = test_trustsymbolwidget

HEADERS += \
	TestTrustSymbolWidget.hpp

SOURCES += \
	TestTrustSymbolWidget.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)
