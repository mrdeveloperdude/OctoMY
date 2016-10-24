# Include helper for the library projects

# IMPORTANT: TOP_PWD and TOP_BUILD are defined in .qmake.config in project root


CORE_IN_DIR = $$TOP_PWD/libs/libcore
CORE_OUT_DIR = $$TOP_BUILD/libs/libcore
LIBS +=				-L$$CORE_OUT_DIR -lcore
INCLUDEPATH +=		$$CORE_IN_DIR
DEPENDPATH +=		$$CORE_IN_DIR
PRE_TARGETDEPS +=	$$CORE_OUT_DIR/libcore.a


PKI_IN_DIR =		$$TOP_PWD/libs/libpki
PKI_OUT_DIR =		$$TOP_BUILD/libs/libpki
LIBS +=				-L$$PKI_OUT_DIR -lpki
INCLUDEPATH +=		$$PKI_IN_DIR
DEPENDPATH +=		$$PKI_IN_DIR
PRE_TARGETDEPS +=	$$PKI_OUT_DIR/libpki.a


PARSER_IN_DIR =		$$TOP_PWD/libs/libparser
PARSER_OUT_DIR =	$$TOP_BUILD/libs/libparser
LIBS +=				-L$$PARSER_OUT_DIR -lparser
INCLUDEPATH +=		$$PARSER_IN_DIR
DEPENDPATH +=		$$PARSER_IN_DIR
PRE_TARGETDEPS +=	$$PARSER_OUT_DIR/libparser.a


MAP_IN_DIR =		$$TOP_PWD/libs/libmap
MAP_OUT_DIR =		$$TOP_BUILD/libs/libmap
LIBS +=				-L$$MAP_OUT_DIR -lmap
INCLUDEPATH +=		$$MAP_IN_DIR
DEPENDPATH +=		$$MAP_IN_DIR
PRE_TARGETDEPS +=	$$MAP_OUT_DIR/libmap.a


QR_IN_DIR =			$$TOP_PWD/libs/libqr
QR_OUT_DIR =		$$TOP_BUILD/libs/libqr
LIBS +=				-L$$QR_OUT_DIR -lqr
INCLUDEPATH +=		$$QR_IN_DIR
DEPENDPATH +=		$$QR_IN_DIR
PRE_TARGETDEPS +=	$$QR_OUT_DIR/libqr.a



ZBAR_IN_DIR =		$$TOP_PWD/libs/libzbar
ZBAR_OUT_DIR =		$$TOP_BUILD/libs/libzbar
LIBS +=				-L$$ZBAR_OUT_DIR -lzbar
INCLUDEPATH +=		$$ZBAR_IN_DIR
DEPENDPATH +=		$$ZBAR_IN_DIR
PRE_TARGETDEPS +=	$$ZBAR_OUT_DIR/libzbar.a


QHTTP_IN_DIR =		$$TOP_PWD/libs/libqhttp
QHTTP_OUT_DIR =		$$TOP_BUILD/libs/libqhttp
LIBS +=				-L$$QHTTP_OUT_DIR -lqhttp
INCLUDEPATH +=		$$QHTTP_IN_DIR
DEPENDPATH +=		$$QHTTP_IN_DIR
PRE_TARGETDEPS +=	$$QHTTP_OUT_DIR/libqhttp.a


STYLE_IN_DIR =		$$TOP_PWD/libs/libstyle
STYLE_OUT_DIR =		$$TOP_BUILD/libs/libstyle
LIBS +=				-L$$STYLE_OUT_DIR -lstyle
INCLUDEPATH +=		$$STYLE_IN_DIR
DEPENDPATH +=		$$STYLE_IN_DIR
PRE_TARGETDEPS +=	$$STYLE_OUT_DIR/libstyle.a


QFI_IN_DIR =		$$TOP_PWD/libs/libqfi
QFI_OUT_DIR =		$$TOP_BUILD/libs/libqfi
LIBS +=				-L$$QFI_OUT_DIR -lqfi
INCLUDEPATH +=		$$QFI_IN_DIR
DEPENDPATH +=		$$QFI_IN_DIR
PRE_TARGETDEPS +=	$$QFI_OUT_DIR/libqfi.a

contains(DEFINES, USE_OPENCL){
CL_IN_DIR =			$$TOP_PWD/libs/libcl
CL_OUT_DIR =		$$TOP_BUILD/libs/libcl
LIBS +=				-L$$CL_OUT_DIR -lcl
INCLUDEPATH +=		$$CL_IN_DIR
DEPENDPATH +=		$$CL_IN_DIR
PRE_TARGETDEPS +=	$$CL_OUT_DIR/libcl.a
}

UTIL_IN_DIR =		$$TOP_PWD/libs/libutil
UTIL_OUT_DIR =		$$TOP_BUILD/libs/libutil
LIBS +=				-L$$UTIL_OUT_DIR -lutil
INCLUDEPATH +=		$$UTIL_IN_DIR
DEPENDPATH +=		$$UTIL_IN_DIR
PRE_TARGETDEPS +=	$$UTIL_OUT_DIR/libutil.a

#message("FROM inc.pri:") include($$TOP_PWD/status.pri)
