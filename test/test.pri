include($$PRIS/common.pri)
include($$PRIS/lib_headers.pri)

CONFIG += testcase
QT += testlib

# NOTE: THis is prepended to avoid link-order errors since it relies on libs
TESTCOMMON_IN_DIR =		$$TESTS/common
TESTCOMMON_OUT_DIR =	$$TOP_BUILD/test/common
LIBS =				$$LIBS				-L$$TESTCOMMON_OUT_DIR -lcommon
INCLUDEPATH =		$$INCLUDEPATH		$$TESTCOMMON_IN_DIR
DEPENDPATH =		$$DEPENDPATH		$$TESTCOMMON_IN_DIR
PRE_TARGETDEPS =	$$PRE_TARGETDEPS	$$TESTCOMMON_OUT_DIR/libcommon.a

