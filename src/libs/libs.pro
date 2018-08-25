TEMPLATE =	subdirs
CONFIG +=	ordered

# IMPORTANT: TOP_PWD and TOP_BUILD and many other frequently used variables
#            are defined in .qmake.config in project root

# How does the build work?
# 1.  libraries are stored in subfolders under libs
# 2a. Each library can either be built as a stand-alone library
# 2b. The remaining libs are compiled into one big library called librest
# 3.  The resulting libraries from steps 2a & 2b are linked to the currently selected application

# NOTE: The .pro file in each library's folder should only contain a link to libs_pro.pri as
#       this allows it to be automatically handled by the build system.

include($$PRIS/common.pri)
include($$PRIS/lib_list.pri)


#= #$$files(lib*)

unset(SUBDIRS)


for(A, AUTOLIBS) {
	SUBDIRS += lib$$A
}

SUBDIRS += \
	librest \



# Dependency stuff adapted from https://vilimpoc.org/blog/2014/02/21/qmake-subdirs-project-automatic-dependencies/
libparser.subdir= libparser
librest.subdir= librest
libplan.subdir= libplan

libplan.depends=	libparser
librest.depends=	libparser


