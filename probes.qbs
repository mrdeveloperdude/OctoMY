
Project{
    property string combinedLibDir: FileInfo.cleanPath(path2) + FileInfo.pathSeparator()
    property string srcDir: FileInfo.cleanPath(FileInfo.joinPaths(project.sourceDirectory, "src")) + FileInfo.pathSeparator()
    property string libsDir: FileInfo.cleanPath(FileInfo.joinPaths(srcDir, "libs")) + FileInfo.pathSeparator()
    
    OctoMYLibProbe {
        id: octomyLibs2
        searchPath: libsDir
    }
}
