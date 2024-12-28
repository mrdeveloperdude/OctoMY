import qbs.FileInfo

BaseProbe{
	// Parameters
	name: "LibProbe"
	searchDir:"src/libs"
	fileName: "lib.qbs"
	folderPrefix: "lib"
	definePrefix: "OC_LIB_"
	debug: true
}
