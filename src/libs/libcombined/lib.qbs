import qbs.FileInfo

StaticLibrary{
	property string localPath: path
	name: FileInfo.baseName(localPath)

	// Enumerate our libs
	LibProbe{
		id: libs
	}
	
	// Enumerate sources for ALL libraries so we create one big combined library (hence the name "libcombined")
	Files{
		name: FileInfo.baseName(localPath) + "_sources"
		prefix: FileInfo.joinPaths(FileInfo.cleanPath(project.sourceDirectory), "src", "libs", "lib*")+FileInfo.pathSeparator()
		debug: true
	}

	//We depend on Qt
	QtDepends{}

	// We depend on C++
	Depends{
		name: "cpp"
	}

	// Tell cpp to look for header files in all the lib folders
	cpp.includePaths: [].concat(base, libs.folders)

	// Tell cpp all the auto-generated defines for OctoMY libraries
	cpp.defines: [].concat(base, libs.defines, ["OCTOMY_COMBINED"])

	// Tell anyone depending on us what dependencies we forward
	Export {
		// Export our dependence on Qt
		QtDepends{}
		
		// We export our dependence on C++
		Depends{
			name: "cpp"
		}
		
		// Tell cpp to look for header files in all the lib folders
		cpp.includePaths: [].concat(base, libs.folders)
	}
}
