import qbs.FileInfo

Project {
    name: "Libs"
    // Enumerate OctoMY libs
    LibProbe {
        id: octomyLibs
    }
    references: [].concat(octomyLibs.files)
}
