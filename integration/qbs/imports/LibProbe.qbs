import qbs.FileInfo

BaseProbe{
    // Parameters
    name: "LibProbe"
    searchDir:"src/libs"
    fileName: "lib.qbs"
    folderPrefix: "lib"
    definePrefix: "OC_LIB_"
    //debug: true
    readonly property string libsDir: FileInfo.cleanPath(FileInfo.joinPaths(project.sourceDirectory, "src", "libs"))
    readonly property string combinedLib: FileInfo.cleanPath(FileInfo.joinPaths(project.sourceDirectory, "src", "combined.qbs"))
}
