import qbs.File
import qbs.FileInfo

BaseProbe{
    // Parameters
    name: "TestProbe"
    searchDir: "test"
    fileName: "test.qbs"
    folderPrefix: "test"
    definePrefix: "OC_TEST_"
    //debug: true
    readonly property string commonTestDir:  FileInfo.cleanPath(FileInfo.joinPaths(searchPath, "common_test"))
    readonly property string commonTestLib:  FileInfo.joinPaths(commonTestDir, "lib.qbs")
}
