import qbs.FileInfo

QtApplication {
	property string localPath
	name: FileInfo.baseName(localPath)

	// We depend on octomy libs
	Depends {
		name: "libcombined"
	}

	// We need testlib
	Depends{
		name: "Qt.testlib"
	}

	// We have some files of our own
	Files{
		name: FileInfo.baseName(localPath) + "_sources"
		prefix: FileInfo.cleanPath(localPath) + FileInfo.pathSeparator()
	}

	// Hook up with autotest in Qt
	type: [].concat(base, ["autotest"])

	// This is not an app the user should care about
	install: false

}
