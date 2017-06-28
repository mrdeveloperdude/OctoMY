include($$PRIS/common.pri)
include($$SRCS/libs/libs_inc.pri)

CONFIG += testcase
QT += testlib

TESTCOMMON_IN_DIR =		$$TESTS/testCommon
TESTCOMMON_OUT_DIR =	$$TOP_BUILD/test/testCommon
LIBS +=					-L$$TESTCOMMON_OUT_DIR -ltest_common
INCLUDEPATH +=			$$TESTCOMMON_IN_DIR
DEPENDPATH +=			$$TESTCOMMON_IN_DIR
PRE_TARGETDEPS +=		$$TESTCOMMON_OUT_DIR/libtest_common.so


