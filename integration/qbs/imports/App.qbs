import qbs.FileInfo

QtApplication {
	property string localPath
	name: FileInfo.fileName(localPath);

	// We depend on octomy libs
	Depends {
		name: "libcombined"
	}

	// We have some files of our own
	Files{
		name: FileInfo.baseName(localPath) + "_sources"
		prefix: FileInfo.cleanPath(localPath) + FileInfo.pathSeparator()
	}

	install: true
	installDir: qbs.targetOS.contains("qnx") ? FileInfo.joinPaths("/tmp", name, "bin") : base
}
