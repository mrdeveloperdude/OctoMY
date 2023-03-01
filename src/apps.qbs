import qbs.FileInfo

Project {
    name: "Apps"
    // Enumerate OctoMY apps
    AppProbe {
        id: octomyApps
    }
    references: octomyApps.files
}
