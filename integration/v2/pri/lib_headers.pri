# This defines AUTOLIBS_SEPARATE and AUTOLIBS_COMBINED variables
include(lib_list.pri)

# We synthesize the "libcombined" since it does not exist on disk
AUTOLIBS_SEPARATE2= combined

# We add the actual libs to be combined
AUTOLIBS_SEPARATE2 += $$AUTOLIBS_SEPARATE

# Automatically set up each lib to build into libcombined
for(A, AUTOLIBS_SEPARATE2) {
	LIBS +=				-L$$clean_path($${BUILD_SRCS}/libs/lib$${A}/$${OC_LIB_PREFIX_DIR}) -l$${A}
	INCLUDEPATH +=		$$clean_path($${SRCS}/libs/lib$${A})
#	INCLUDEPATH +=		$$clean_path($${BUILD_SRCS}/libs/lib$${A})
	DEPENDPATH +=		$$clean_path($${SRCS}/libs/lib$${A})
	PRE_TARGETDEPS +=	$$clean_path($${BUILD_SRCS}/libs/lib$${A}/$${OC_LIB_PREFIX_DIR}/$${OC_LIB_PREFIX}$${A}.$${QMAKE_EXTENSION_STATICLIB})
}


# Even if code is linked via libcombined we still need full include paths for all sub libraries
for(A, AUTOLIBS_COMBINED) {
	INCLUDEPATH +=$$clean_path($${SRCS}/libs/lib$${A})
}

# CLean each variable by removing duplicates
LIBS= $$unique(LIBS)
INCLUDEPATH= $$unique(INCLUDEPATH)
DEPENDPATH= $$unique(DEPENDPATH)
PRE_TARGETDEPS= $$unique(PRE_TARGETDEPS)

#message("----------- libs_inc incpath -------------------")
#message("| " $${INCLUDEPATH})
#include($$PRIS/status.pri)
