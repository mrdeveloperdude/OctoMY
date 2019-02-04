
# This is the complete list of available defines for the OctoMY build system
# They are used to switch on and of features at build time
# They are all prefixed with OC_USE_ for easy identification

# The namespace hierarchy is as follows:

# OC_USE_LIB_* internal library (in git folder under src/libs)

# OC_USE_LIB_EXT_* external library (expected to be available on build system)

# OC_USE_FEATURE_* a feature that may be enabled or disabled. Features are typically spread over many libs, or maybe only part of a single lib.
# NOTE: This is used as a hint, the build system may disable some features for platforms or configurations that would not be able to support them
# In that case a warning is emitted by the build system

# OC_USE_TESTS_* a set of tests can be enabled and disabled



#DEFINES += OC_USE_FEATURE_DOCS
#DEFINES += OC_USE_TESTS_BASIC
#DEFINES += OC_USE_TESTS_WEB
#DEFINES += OC_USE_TESTS_STRESS
#DEFINES += OC_USE_TESTS_SELECTED ###########
#DEFINES += OC_USE_FEATURE_GEAR_DYNAMICS
#DEFINES += OC_USE_FEATURE_QT3D
#DEFINES += OC_USE_LIB_EXT_ESPEAK
#DEFINES += OC_USE_LIB_EXT_OPENCV
#DEFINES += OC_USE_LIB_EXT_OPENCL
#DEFINES += OC_USE_FEATURE_ARDUINO
#DEFINES += OC_USE_FEATURE_STATUS
#DEFINES += OC_USE_LIB_ZBAR
#DEFINES += OC_USE_LIB_PARSER
#DEFINES += OC_USE_FEATURE_CRASH_HELP
#DEFINES += OC_USE_FEATURE_MARKDOWN ############
#DEFINES += OC_USE_FEATURE_MOTORICS


contains(AUTOLIBS_BOTH, motorics){
	!contains(DEFINES, OC_USE_FEATURE_MOTORICS){
		warning("WARNING: motorics lib selected for build while OC_USE_LIB_MOTORICS is disabled")
	}
}
else{
	contains(DEFINES, OC_USE_FEATURE_MOTORICS){
		message("NOTE: motorics lib not selected for build, disabling OC_USE_LIB_MOTORICS")
		DEFINES -= OC_USE_FEATURE_MOTORICS
	}
}


contains(AUTOLIBS_BOTH, ardumy){
	!contains(DEFINES, OC_USE_FEATURE_ARDUINO){
		warning("WARNING: ardumy lib selected for build while OC_USE_FEATURE_ARDUINO is disabled")
	}
}
else{
	contains(DEFINES, OC_USE_FEATURE_ARDUINO){
		message("NOTE: ardumy lib not selected for build, disabling OC_USE_FEATURE_ARDUINO")
		DEFINES -= OC_USE_FEATURE_ARDUINO
	}
}
