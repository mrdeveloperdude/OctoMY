import qbs.FileInfo

Project {
	name: "OctoMY™"
	qbsSearchPaths: "integration/qbs"
	
	// Android-specific configuration
	Properties {
		condition: qbs.targetOS.contains("android")
		cpp.androidPlatform.apiLevel: "29" // Set API level to 29 for Android 10
		cpp.defines: cpp.defines.concat(["OC_USE_FEATURE_ARDUINO"])
	}
	//property string androidAbi: "arm64-v8a"

	//qbsModuleProviders: "integration/qbs"
	references: [
		  "src/libs/libcombined/lib.qbs"
		//        ,"src/libs.qbs"
		, "test/tests.qbs"
		, "src/apps.qbs"
		//, "integration/android/android.qbs"
	]
	
}
