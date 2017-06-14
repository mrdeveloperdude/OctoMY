
QR_IN_DIR =			$$TOP_PWD/libs/libqr
QR_OUT_DIR =		$$TOP_BUILD/libs/libqr
LIBS +=				-L$$QR_OUT_DIR -lqr
INCLUDEPATH +=		$$QR_IN_DIR
DEPENDPATH +=		$$QR_IN_DIR
PRE_TARGETDEPS +=	$$QR_OUT_DIR/libqr.a
