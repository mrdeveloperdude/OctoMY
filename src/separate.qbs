import qbs.FileInfo

Project {
    name: "Separate"
    
    property string srcDir: FileInfo.cleanPath(FileInfo.joinPaths(project.sourceDirectory, "src")) + FileInfo.pathSeparator()
    property string libsDir: FileInfo.cleanPath(FileInfo.joinPaths(srcDir, "libs")) + FileInfo.pathSeparator()

    // Enumerate OctoMY lib folders
    OctoMYLibProbe {
        id: octomyLibs
        searchPath: libsDir
    }
    
    references: octomyLibs.libFiles
}
