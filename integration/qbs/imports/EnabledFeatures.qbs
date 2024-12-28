// Features enabled by OctoMY
Properties {
	
	/*
	This is the complete list of available defines for the OctoMY build system
	They are used to switch on and of features at build time
	They are all prefixed with OC_USE_ for easy identification
	
	The namespace hierarchy is as follows:
	
	OC_USE_LIB_* internal library (in git folder under src/libs)
	
	OC_USE_LIB_EXT_* external library (expected to be available to build system)
	
	OC_USE_FEATURE_* a feature that may be enabled or disabled. Features are typically spread over many libs, but can also just affect a single lib.
	
	NOTE: Features are used as hints, the build system may disable some features for platforms or configurations that would not be able to support them
	      In that case a warning is emitted by the build system
	
	OC_USE_TESTS_* a set of tests can be enabled and disabled

	*/

	property stringList feature_defines: [
		  "OC_USE_FEATURE_DUMMY"
		, "OC_USE_FEATURE_ARDUINO"
		
//		, "OC_USE_FEATURE_CRASH_HELP"
//		, "OC_USE_FEATURE_DOCS"
//		, "OC_USE_FEATURE_GEAR_DYNAMICS"
//		, "OC_USE_FEATURE_MARKDOWN ############"
//		, "OC_USE_FEATURE_MOTORICS"
//		, "OC_USE_FEATURE_QT3D"
//		, "OC_USE_FEATURE_STATUS"
//		, "OC_USE_LIB_EXT_ESPEAK"
//		, "OC_USE_LIB_EXT_OPENCL"
//		, "OC_USE_LIB_EXT_OPENCV"
//		, "OC_USE_LIB_PARSER"
//		, "OC_USE_LIB_ZBAR"
//		, "OC_USE_TESTS_BASIC"
//		, "OC_USE_TESTS_SELECTED ###########"
//		, "OC_USE_TESTS_STRESS"
//		, "OC_USE_TESTS_WEB"

	]
	
	// TODO: resolve these ad-hoc defines used to disable parts of project:
	// + USE_GL_STRESS_WIDGET
	// + USE_OC_OCL_STUFF
	// + OC_USE_LIBZBAR_STUFF
	
	
	cpp.defines: [].concat(base, feature_defines)
//	*/
}
