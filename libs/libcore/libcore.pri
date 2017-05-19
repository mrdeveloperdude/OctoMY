# Include this .pri file to have libcore linked with your project

# IMPORTANT: TOP_PWD and TOP_BUILD are defined in .qmake.config in project root

CORE_IN_DIR =		$$TOP_PWD/libs/libcore
CORE_OUT_DIR =		$$TOP_BUILD/libs/libcore
LIBS +=				-L$$CORE_OUT_DIR -lcore
INCLUDEPATH +=		$$CORE_IN_DIR
DEPENDPATH +=		$$CORE_IN_DIR
PRE_TARGETDEPS +=	$$CORE_OUT_DIR/libcore.a
#TARGETDEPS +=		$$CORE_OUT_DIR/libcore.a
