TEMPLATE = subdirs

#CONFIG += ordered

message("")
message("_______________________________")

include(common.pri)
#include(functions.pri)

DEFINES += USE_SEPARATE_LIBS

LIB_BASE=libs

# To help linker, put the most depending targets first, and the most depended last
SUBDIRS += \
#	libs \
#	$$LIB_BASE/libagent \
#	$$LIB_BASE/libremote \
#	$$LIB_BASE/libhub \
#	$$LIB_BASE/libzoo \
#	$$LIB_BASE/libsec \
#	$$LIB_BASE/libplan \
	$$LIB_BASE/libparser \
#	$$LIB_BASE/libmap \
#	$$LIB_BASE/libqr \
	$$LIB_BASE/libzbar \
#	$$LIB_BASE/libweb \
#	$$LIB_BASE/libstyle \
#	$$LIB_BASE/libqfi \
#	$$LIB_BASE/librng \
#	$$LIB_BASE/libaudio \
#	$$LIB_BASE/libdynamics \
#	$$LIB_BASE/libpuppet \
#	$$LIB_BASE/libutil \
#	$$LIB_BASE/libmbedtls \
	$$LIB_BASE/libqpolarssl \
#	$$LIB_BASE/libardumy \
#	$$LIB_BASE/libcomms \
#	$$LIB_BASE/libnode \
	agent \
#	remote \
#	hub \
#	zoo \


# NOTE: USE_TESTS is enabled when useful in local_override.pri
contains(DEFINES, USE_TESTS){
	# Warn the user that there is potential problems in build config for tests
	if( contains(DEFINES, USE_BASIC_TESTS) | contains(DEFINES, USE_WEB_TESTS)  ){
		message(TEST PROJECT WAS ADDED TO OCTOMY BUILD)
	}
	else{
		error(USE_TESTS defined without USE_BASIC_TESTS or USE_WEB_TESTS)
	}

	SUBDIRS += \
		test \

}


contains(DEFINES, USE_DOCS){
	message(DOCUMENTATION PROJECT WAS ADDED TO OCTOMY BUILD)
	SUBDIRS += \
		doc \
}



# Dependency stuff adapted from https://vilimpoc.org/blog/2014/02/21/qmake-subdirs-project-automatic-dependencies/
#libs.subdir=		libs
#hub.subdir=			hub
#agent.subdir=		agent
#remote.subdir=		remote
#zoo.subdir=			zoo
#test.subdir=		test
#doc.subdir=			doc


#hub.depends=		libs
#agent.depends=		libs
#remote.depends=		libs
#zoo.depends=		libs
#test.depends=		libs
#doc.depends=		libs hub remote agent test

RESOURCES += \
	libs/libcore/resources/icons.qrc
