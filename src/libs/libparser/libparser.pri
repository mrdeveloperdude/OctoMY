# Include this .pri file to have libparser linked with your project

# IMPORTANT: TOP_PWD and TOP_BUILD are defined in .qmake.config in project root

PARSER_IN_DIR =		$$TOP_PWD/libs/libparser
PARSER_OUT_DIR =	$$TOP_BUILD/libs/libparser
LIBS +=				-L$$PARSER_OUT_DIR -lparser
INCLUDEPATH +=		$$PARSER_IN_DIR
# NOTE: Since libparser outputs some sources to the shadow dir we put thqat on includepath explicitly
INCLUDEPATH +=		$$PARSER_OUT_DIR
DEPENDPATH +=		$$PARSER_IN_DIR
PRE_TARGETDEPS +=	$$PARSER_OUT_DIR/libparser.a
