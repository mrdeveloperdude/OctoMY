include( standard.pri )
include( optimization.pri )
include( build_optimization.pri )
include( warnings.pri )
include( linkage.pri )
include( protection.pri )
include( ccache.pri )


QMAKE_TARGET_COMPANY =		"OctoMY™"
QMAKE_TARGET_COPYRIGHT =	"Copyright © 2012-2018 Lennart Rolland <lennartrolland@gmail.com>"
QMAKE_TARGET_DESCRIPTION =	"N-Limbed madness™"

# Set maximum compression level for resources
QMAKE_RESOURCE_FLAGS += -compress 9

# OPTIONAL FEATURES AND EXTERNAL LIBS
# NOTE: ALL ARE OFF BY DEFAULT TO KEEP DEPENDENCIES LOW & PREDICTABLE
# NOTE: PLEASE USE THE local_overrides.pri FILE TO INCLUDE THEM (SEE BELOW)

# PUT YOUR LOCAL OVERRIDES IN THIS FILE AND DON'T INCLUDE IT IN GIT!
LOCAL_OVERRIDES_NAME=$$PRIS/local_overrides.pri
DEFAULT_OVERRIDES_NAME=$$PRIS/overrides/default.pri

# If it does not exist, copy the defaults
exists( $$LOCAL_OVERRIDES_NAME ) {
	#message("Found local overrides file")
}else{
	exists( $$DEFAULT_OVERRIDES_NAME ) {
		# TODO: shell_path() is not correct because we might be creating a makefile for another platform
		#       than what qmake is running under. If this starts miss-behaving during cross-compiles
		#       then that is the reason.
		copcmd="$$QMAKE_COPY_FILE $$shell_path($$DEFAULT_OVERRIDES_NAME) $$shell_path($$LOCAL_OVERRIDES_NAME)"
		message("Copying local overrides from defaults with command " $$copcmd)
		copy_output=$$system($$copcmd)
		message("Copy output was " $${copy_output})
	}else{
		message("Default overrides file '" $${DEFAULT_OVERRIDES_NAME} "' was not found")
	}
}


!include( $$LOCAL_OVERRIDES_NAME ) {
	LOCAL_OVERRIDES=false
} else {
	LOCAL_OVERRIDES=true
	#message($${LOCAL_OVERRIDES_NAME} " was loaded")
}


# Add only plugins that are used and supported by the Qt build you are using
QT += core
QT += gui
QT += opengl
QT += widgets
QT += network
QT += multimedia
QT += multimediawidgets
QT += positioning
QT += serialport
QT += bluetooth
QT += sensors
QT += xml
QT += svg
QT += sql
QT += testlib
QT += concurrent
#QT += quick
QT += printsupport
#QT += openglextensions

contains(DEFINES, OC_USE_FEATURE_QT3D){
	QT += 3dcore
	QT += 3drenderer
	QT += 3dinput
}

android{
	QT += androidextras
	#NOTE: Each individual project will look for her own android config (making it possible for each node to have separate adaptations
	ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
	contains(DEFINES, OC_USE_LIB_EXT_OPENCL){
		warning("OpenCL support was enabled but is not supported in Android, so disabling it for now")
		DEFINES -= OC_USE_LIB_EXT_OPENCL
	}
}

win{
	QT += winextras
}

mac{
	QT += macextras
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

msvc:debug:OC_LIB_PREFIX_DIR=debug
msvc:release:OC_LIB_PREFIX_DIR=release
mingw:debug:OC_LIB_PREFIX_DIR=debug
mingw:release:OC_LIB_PREFIX_DIR=release
gcc:OC_LIB_PREFIX=lib


# Add verbose logging to linker (enable for debugging)

#QMAKE_CXXFLAGS += -Wl,--verbose
