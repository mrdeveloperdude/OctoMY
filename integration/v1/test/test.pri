include($$PRIS/common.pri)

CONFIG += testcase
QT += testlib

TESTCOMMON_IN_DIR =		$$TESTS/common
TESTCOMMON_OUT_DIR =	$$TOP_BUILD/test/common
LIBS +=				-L$$TESTCOMMON_OUT_DIR -lcommon
INCLUDEPATH +=		$$TESTCOMMON_IN_DIR
DEPENDPATH +=		$$TESTCOMMON_IN_DIR
PRE_TARGETDEPS +=	$$TESTCOMMON_OUT_DIR/libcommon.a


#NOTE: This must come last to get correct linking order
include($$PRIS/lib_headers.pri)
