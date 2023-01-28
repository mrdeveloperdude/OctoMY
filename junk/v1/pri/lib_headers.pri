
include(lib_list.pri)

AUTOLIBS2= rest

AUTOLIBS2 += $$AUTOLIBS


for(A, AUTOLIBS2) {
	LIBS +=				-L$$clean_path($${BUILD_SRCS}/libs/lib$${A}/$${OC_LIB_PREFIX_DIR}) -l$${A}
	INCLUDEPATH +=		$$clean_path($${SRCS}/libs/lib$${A})
#	INCLUDEPATH +=		$$clean_path($${BUILD_SRCS}/libs/lib$${A})
	DEPENDPATH +=		$$clean_path($${SRCS}/libs/lib$${A})
	PRE_TARGETDEPS +=	$$clean_path($${BUILD_SRCS}/libs/lib$${A}/$${OC_LIB_PREFIX_DIR}/$${OC_LIB_PREFIX}$${A}.$${QMAKE_EXTENSION_STATICLIB})
}


# Even if code is linked via librest we still need full include paths for all sub libraries
for(A, AUTOLINKS) {
	INCLUDEPATH +=$$clean_path($${SRCS}/libs/lib$${A})
}

LIBS= $$unique(LIBS)
INCLUDEPATH= $$unique(INCLUDEPATH)
DEPENDPATH= $$unique(DEPENDPATH)
PRE_TARGETDEPS= $$unique(PRE_TARGETDEPS)

#message("----------- libs_inc incpath -------------------")
#message("| " $${INCLUDEPATH})
#include($$PRIS/status.pri)
