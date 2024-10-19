QtApplication {
	// Define the name of the project
	name: "svgchecker"
	// The source files to include in the project
	files: ["main.cpp"]

	// Use the Qt module for this project
	Depends { name: "Qt"; submodules: ["core", "svg"] }

}


