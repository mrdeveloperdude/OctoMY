import qbs.FileInfo

Project {
	name: "Apps"
	// Enumerate OctoMY apps
	AppProbe {
		id: octomyApps
		searchDir:"src"
	}
	references: octomyApps.files
}
