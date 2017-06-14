TEMPLATE =	subdirs
CONFIG +=	ordered

# IMPORTANT: TOP_PWD and TOP_BUILD are defined in .qmake.config in project root

# How does the build work?
# 1. libraries are stored in subfolders under libs
# 2a. Each library can either be built as stand-alone libraries that are linked to the project in the end
# 2b. while the remaining libs are simply compiled into one big library called librest wich is also linked to the project in the end


include($$TOP_PWD/pri/common.pri)
include(libs_list.pri)

# Link separate libs
DEFINES += USE_SEPARATE_LIB

for(A, AUTOLIBS) {
	SUBDIRS += lib$$A
	INCLUDEE=$$A
	include($$TOP_PWD/pri/libincluder.pri)
	libsrest.depends=		lib$$A
}

SUBDIRS += \
	librest \


# Link objects directly
DEFINES -= USE_SEPARATE_LIB

for(A, AUTOLINKS) {
	INCLUDEE=$$A
	include($$TOP_PWD/pri/libincluder.pri)
}

INCLUDEPATH= $$unique(INCLUDEPATH)

#message("----------- libs incpath -------------------")
#message("| " $${INCLUDEPATH})


libs.subdir=		librest
libs.depends=		rest
