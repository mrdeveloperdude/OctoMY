# This is the global commons, not to be copnfused with common_test
include($$PRIS/common.pri)

CONFIG += qt warn_on depend_includepath testcase
TEMPLATE = app
QT += testlib gui


# We link to common_test which hold globally useful utilities for testing
COMMON_TEST_IN_DIR =   $$clean_path($$TESTS/common_test)
COMMON_TEST_OUT_DIR =  $$clean_path($$TOP_BUILD/test/common_test/$${OC_LIB_PREFIX_DIR})

LIBS +=			       -L$$COMMON_TEST_OUT_DIR -lcommon_test
INCLUDEPATH +=         $$COMMON_TEST_IN_DIR
DEPENDPATH +=          $$COMMON_TEST_IN_DIR
PRE_TARGETDEPS +=      $$COMMON_TEST_OUT_DIR/libcommon_test.a



# NOTE: This must come last to get correct linking order
include($$PRIS/lib_headers.pri)

message("----------- TEST ")
include($$PRIS/status.pri)
