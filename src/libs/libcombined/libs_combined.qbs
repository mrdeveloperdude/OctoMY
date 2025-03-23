import qbs.FileInfo
/*
  This is libcombined, a pseudo lib that includes the sources of all the libs in the project
  It is used to speed up build and simplify the build process.
  
  Ideally each lib is built and linked separately and each lib can chose which other libs they depend on.
  
  This was more important in the qmake era of olde. With Qbs, the dependency graph is 
  automatically detected and everything just works.
  
  We may investigate a more granular build control at a later stage.
*/


StaticLibrary {
	property string localPath: path
	property bool lib_debug: false
	property string lib_prefix: FileInfo.joinPaths(FileInfo.cleanPath(project.sourceDirectory), "src", "libs")
	name: FileInfo.baseName(localPath)
	property bool debug_output: {
		console.info("lib_combined was here:");
		console.info(" + localPath  '" + localPath + "'");
		console.info(" + lib_prefix '" + lib_prefix + "'");
		console.info(" + name       '" + name + "'");
		//console.info(" + libs      '" + libs + "'");
		return false;
	}
	// Enumerate our libs
	LibProbe{
		id: libs
		debug: lib_debug
	}
	
	// Enumerate sources for ALL libraries so we create one big combined library (hence the name "libcombined")
	Sources{
		name: FileInfo.baseName(localPath) + "_sources"
		prefix: lib_prefix
		debug: lib_debug
	}
	
	// And the same for resources
	Resources{
		name: FileInfo.baseName(localPath) + "_resources"
		prefix: lib_prefix
		debug: lib_debug
	}
	
	// Use custom module to build shaders into spir-v binaries that are automatically assembled as resource into the binary
	Depends { name: "Shaders" }

	// We depend on C++
	Depends{ name: "cpp" }
	// We depend on Qt
	QtDepends{}
	// Uniform build options
	BuildOptions{}
	// OctoMY feature defines
	EnabledFeatures{}

	// Tell cpp to look for header files in all the lib folders
	cpp.includePaths: [].concat(base, libs.folders)

	// Tell cpp all the auto-generated defines for OctoMY libraries
	cpp.defines: ["OCTOMY_COMBINED"].concat(base, libs.defines)

	// Tell anyone depending on us what dependencies we forward
	Export {
		// We depend on C++
		Depends{ name: "cpp" }
		// We depend on Qt
		QtDepends{}
		// Uniform build options
		BuildOptions{}
		// OctoMY feature defines
		EnabledFeatures{}
		
		// Tell cpp to look for header files in all the lib folders
		cpp.includePaths: [].concat(base, libs.folders)
	}

}

