import qbs.FileInfo

StaticLibrary {
	property string localPath
	name: FileInfo.fileName(localPath)
	property string lib_prefix: FileInfo.joinPaths(FileInfo.cleanPath(project.sourceDirectory), "src", "libs")
	property string lib_define: "OC_LIB" + name.toUpperCase()
	property string lib_use: "OC_USE_LIB" + name.toUpperCase()
	property bool lib_debug: false
	
	property bool debug_output: {
		if(lib_debug){
			console.info("Lib '" + name + "':");
			console.info("  localPath: " + localPath);
			console.info(" lib_prefix: " + lib_prefix);
			console.info(" lib_define: " + lib_define)
		}
		return false;
	}
	
	// Tag shaders
	Files{
		name: "shaders"
		types: ['vert', 'frag', 'comp', 'tesc', 'tese', 'geom']
		fileTags: ["shader_source"]
	}

	// Enumerate sources for ALL libraries so we create one big combined library (hence the name "libcombined")
	Sources{
		name: "sources"
		prefix: lib_prefix
		debug: lib_debug
	}
	// And some resources
	Resources{
		name: "resources"
		prefix: lib_prefix
		debug: lib_debug
	}
	
	Depends { name: "Shaders" }

	// We depend on C++
	Depends{ name: "cpp" }
	// We depend on Qt
	QtDepends{}
	// Uniform build options
	BuildOptions{}
	// OctoMY feature defines
	EnabledFeatures{}

	// Tell cpp to look for header files in the lib folder
	cpp.includePaths: [].concat(base, [localPath])
	
	// Tell cpp the auto-generated defines for this lib
	cpp.defines: [].concat(base, [lib_define])

	// Export our dependencies
	Export {
		// We depend on C++
		Depends{ name: "cpp" }
		// We depend on Qt
		QtDepends{}
		// Uniform build options
		BuildOptions{}
		// OctoMY feature defines
		EnabledFeatures{}
		// Tell cpp to look for header files in the lib folder
		cpp.includePaths: [].concat(base, [localPath])
	}
}
