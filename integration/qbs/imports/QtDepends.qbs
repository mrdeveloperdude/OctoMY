// Add dependency on all Qt modules used by OctoMY
Depends {
	name: "Qt"
	enableFallback: false
	property bool includeTest: false
	submodules: [].concat([
		  "concurrent"
		, "bluetooth"
		, "core"
		, "core-private"
		, "gui"
		, "multimedia"
		, "multimediawidgets"
		, "openglwidgets"
		, "positioning"
		, "printsupport"
		, "sensors"
		, "serialport"
		, "svgwidgets"
		, "widgets"
		, "xml"
		, "testlib"
	], includeTest?[]:["testlib"])
}


