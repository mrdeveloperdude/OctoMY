
include(lib_list.pri)

AUTOLIBS2= rest

AUTOLIBS2 += $$AUTOLIBS

for(A, AUTOLIBS2) {
	LIBS +=				-L$$clean_path($$BUILD_SRCS/libs/lib$${A}) -l$${A}
	INCLUDEPATH +=		$$clean_path($$SRCS/libs/lib$${A})
	INCLUDEPATH +=		$$clean_path($$BUILD_SRCS/libs/lib$${A})
	DEPENDPATH +=		$$clean_path($$SRCS/libs/lib$${A})
	PRE_TARGETDEPS +=	$$clean_path($$BUILD_SRCS/libs/lib$${A}/lib$${A}.a)
}


# Even if code is linked via librest we still need full include paths for all sub libraries
for(A, AUTOLINKS) {
	INCLUDEPATH +=$$clean_path($$SRCS/libs/lib$$A)
}

INCLUDEPATH= $$unique(INCLUDEPATH)

#message("----------- libs_inc incpath -------------------")
#message("| " $${INCLUDEPATH})
#include($$PRIS/status.pri)
