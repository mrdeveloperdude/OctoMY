
// Android-specific configuration
Properties {
	condition: qbs.targetOS.contains("android")
	cpp.androidPlatform.apiLevel: "29" // Set API level to 29 for Android 10
	//property string androidAbi: "arm64-v8a"
}
