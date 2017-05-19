TEMPLATE = subdirs
CONFIG -= ordered

include($$TOP_PWD/common.pri)
#QT         += core core-private gui gui-private


# To help linker, put the most depending libs first, and the most depended last
SUBDIRS += \
	libagent \
	libremote \
	libhub \
	libzoo \
	libsec \
#	libplan \
#	libparser \
#	libmap \
	libqr \
	libzbar \
	libweb \
	libstyle \
	libqfi \
	librng \
	libaudio \
	libdynamics \
	libpuppet \
	libcore \
	libutil \
	libmbedtls \
	libqpolarssl \
	libardumy \
	libcomms \



# Dependency stuff adapted from https://vilimpoc.org/blog/2014/02/21/qmake-subdirs-project-automatic-dependencies/
#libcore.subdir=			libcore
#libutil.subdir=			libutil
#libsec.subdir=			libsec
#libplan.subdir=			libplan
#libparser.subdir=		libparser
#libmap.subdir=			libmap
#libqr.subdir=			libqr
#libzbar.subdir=			libzbar
#libweb.subdir=		    libweb
#libstyle.subdir=	libstyle
#libqfi.subdir=			libqfi
#librng.subdir=			librng
#libaudio.subdir=		libaudio
#libdynamics.subdir=		libdynamics
#libpuppet.subdir=		libpuppet
#libagent.subdir=		libagent
#libremote.subdir=		libremote
#libhub.subdir=			libhub
#libzoo.subdir=			libzoo
#libmbedtls.subdir=		libmbedtls
#libqpolarssl.subdir=	libqpolarssl
#libardumy.subdir=		libardumy
#libcomms.subdir=		libcomms

#libcore.depends= libutil librng libweb libpuppet libsec libcomms
#libaudio.depends= librng libsec
#libsec.depends= libqpolarssl
#libqpolarssl.depends= libmbedtls
#libcomms.depends= libcore
#libplan.depends= libparser

#libagent.depends= libcore libutil libsec libpuppet
#libremote.depends= libcore
#libhub.depends= libcore
#libzoo.depends= libcore


contains(DEFINES, EXTERNAL_LIB_OPENCL){
message("ADDING LIBCLT TO BUILD")
SUBDIRS += \
	libclt \

libclt.subdir=			libclt

}
else{
message("NO LIBCLT IN BUILD")
}




contains(DEFINES, ARDUINO_ARCH_AVR){
message("ADDING ARDUINO TO BUILD")
SUBDIRS += \
	arduino \

arduino.subdir=			arduino

}
else{
message("NO ARDUINO IN BUILD")
}


