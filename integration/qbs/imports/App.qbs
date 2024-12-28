import qbs.FileInfo

Application {
	property string localPath
	name: FileInfo.fileName(localPath);
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
	install: true
	installDir: qbs.targetOS.contains("qnx") ? FileInfo.joinPaths("/tmp", name, "bin") : base
}
