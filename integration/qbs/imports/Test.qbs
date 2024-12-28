import qbs.FileInfo

QtApplication {
	property string localPath
	name: FileInfo.baseName(localPath)
	// Enumerate sources
	Sources{
		name: FileInfo.baseName(localPath) + "_sources"
		prefix: localPath
		debug: true
	}
	// And some resources
	Resources{
		name: FileInfo.baseName(localPath) + "_resources"
		prefix: localPath
		debug: true
	}

	// We depend on C++
	Depends{ name: "cpp" }
	// We depend on Qt
	QtDepends{}
	// Uniform build options
	BuildOptions{}
	// OctoMY feature defines
	EnabledFeatures{}
	// We depend on octomy libs
	Depends { name: "libcombined"}

	// We need testlib
	Depends{
		name: "Qt.testlib"
	}
	// Hook up with autotest in Qt
	type: [].concat(base, ["autotest"])
	// This is not an app the user should care about
	install: false
}
