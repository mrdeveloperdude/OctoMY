 TEMPLATE = subdirs

SUBDIRS += \
	libcore \
	libpki \
	libparser \
	libmap \
	libqr \
	libzbar \
	libqhttp \
	liboctostyle \

# Dependency stuff adapted from https://vilimpoc.org/blog/2014/02/21/qmake-subdirs-project-automatic-dependencies/
libcore.subdir=			libcore
libpki.subdir=			libpki
libparser.subdir=		libparser
libmap.subdir=			libmap
libqr.subdir=			libqr
libzbar.subdir=			libzbar
libqhttp.subdir=		libqhttp
liboctostyle.subdir=	libstyle


include($$TOP_PWD/common.pri)


