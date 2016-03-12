TEMPLATE = subdirs

SUBDIRS += \
	libcore \
	libpki \
	libqr \
	libzbar \
	liboctostyle \

# Dependency stuff adapted from https://vilimpoc.org/blog/2014/02/21/qmake-subdirs-project-automatic-dependencies/
libcore.subdir=			libcore
libpki.subdir=			libpki
libqr.subdir=			libqr
libzbar.subdir=			libzbar
liboctostyle.subdir=	libstyle

include($$TOP_PWD/common.pri)


