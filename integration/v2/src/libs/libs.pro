# Use qmake's subdirectory function.
# This will automatically traverse all directories we put in the variable "SUBDIRS" and treat each dir as a separate project to be built
# NOTE: each dir named must have a .pro file with the same name as the directory it is in, for example "libfoo/libfoo.pro"

TEMPLATE =	subdirs

# Tell qmake we want the projects to be built in the order we give them in the SUBDIRS variable.
# This prevents the order changing unexpectedly/unpredictably when a project is renamed or removed/introduced.

CONFIG +=	ordered

# IMPORTANT: TOP_PWD and TOP_BUILD and many other frequently used variables
#            are defined in .qmake.config in project root

# How does the build work?
# 1.  libraries are stored in subfolders under libs
# 2a. Each library can either be built as a stand-alone library
# 2b. The remaining libs are compiled into one big library called libcombined
# 3.  The resulting libraries from steps 2a & 2b are linked to the currently selected application

# NOTE: The .pro file in each library's folder should only contain a link to libs_pro.pri as
#       this allows it to be automatically handled by the build system.

# This is common variables and functions that may come in handy
include($$PRIS/common.pri)

# This is the list of projects. It defines AUTOLIBS and AUTLINKS variables for spearate libs and combined libs respectively
include($$PRIS/lib_list.pri)

# Start with no subdirectories
unset(SUBDIRS)

# Go through all the library folders defined in lib_list.pri and add them as subdirectories
for(A, AUTOLIBS) {
	SUBDIRS += lib$$A
}

# Finally add the combined library as a subdirectory
SUBDIRS += \
	libcombined \


# Dependency stuff adapted from https://vilimpoc.org/blog/2014/02/21/qmake-subdirs-project-automatic-dependencies/

# Set up dependencies between some libraries so that build order gets correctly calculated by qmake

libparser.subdir=     libparser
libcombined.subdir=   libcombined
libplan.subdir=       libplan

libplan.depends=      libparser
libcombined.depends=  libparser


