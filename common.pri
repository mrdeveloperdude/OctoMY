include( standard.pri )
include( optimize_small.pri )
include( warnings.pri )
#include( ccache.pri )

QMAKE_TARGET_COMPANY =		"OctoMY™"
QMAKE_TARGET_COPYRIGHT =	"Copyright © 2012-2017 Lennart Rolland <lennartrolland@gmail.com>"
QMAKE_TARGET_DESCRIPTION =	"N-Limbed madness™"


# RUN TESTS ON RELEASE BUILD
# NOTE: RUN_WEB_TESTS SHOULD BE ON WHEN THE BUILDING MACHINE IS CONNECTED TO INTERNET

#  Enable tests by default for now (will be enabled autoamtically for release builds in test.pro otherize)
#DEFINES += USE_DOCS
#DEFINES += USE_TESTS
#DEFINES += USE_BASIC_TESTS
#DEFINES += USE_QT3D
#DEFINES += USE_WEB_TESTS
#DEFINES += EXTERNAL_LIB_ESPEAK
#DEFINES += EXTERNAL_LIB_OPENCV

# OPTIONAL FEATURES AND EXTERNAL LIBS
# NOTE: ALL ARE OFF BY DEFAULT TO KEEP DEPENDENCIES LOW & PREDICTABLE
# NOTE: PLEASE USE THE local_overrides.pri FILE TO INCLUDE THEM (SEE BELOW)
# NOTE: LISTED HERE AS A REFERENCE ONLY

#DEFINES += USE_FEATURE_BUFFER_HONEYPOT

# PUT YOUR LOCAL OVERRIDES IN THIS FILE AND DON'T INCLUDE IT IN GIT!
!include( local_overrides.pri ) {
	LOCAL_OVERRIDES=false
}else{
	LOCAL_OVERRIDES=true
}

# Add only plugins that are used and supported by the Qt build you are using
QT += core gui opengl widgets network multimedia multimediawidgets positioning serialport bluetooth sensors xml svg sql testlib quick printsupport openglextensions

contains(DEFINES, USE_QT3D){
	QT += 3dcore 3drenderer 3dinput
}

android{
	QT += androidextras
	#NOTE: Each individual project will look for her own android config (making it possible for each node to have separate adaptations
	ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
	contains(DEFINES, EXTERNAL_LIB_OPENCL){
		warning("OpenCL support was enabled but is not supported in Android, so disabling it for now")
		DEFINES -= EXTERNAL_LIB_OPENCL
	}
}

static {
	DEFINES += STATIC
}

shared {
	DEFINES += SHARED
}

dynamic {
	DEFINES += DYNAMIC
}

# Ensure one "debug_and_release" in CONFIG, for clarity...
debug_and_release {
	CONFIG -= debug_and_release
	CONFIG += debug_and_release
}

# Ensure one "debug" or "release" in CONFIG so they can be used as
# Conditionals instead of writing "CONFIG(debug, debug|release)"...
CONFIG(debug, debug|release) {
	CONFIG -= debug release
	CONFIG += debug
}

CONFIG(release, debug|release) {
	CONFIG -= debug release
	CONFIG += release
}

release{
	DEFINES += RELEASE
}

debug{
	DEFINES += DEBUG
}




# Add verbose logging to linker (enable for debugging)

#QMAKE_CXXFLAGS += -Wl,--verbose

DISTFILES += \
	$$PWD/ccache.pri
