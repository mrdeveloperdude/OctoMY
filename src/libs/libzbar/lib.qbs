OctoMYAutoLibNoFiles {
    property string path2:path
    Group{
        name: FileInfo.fileName(path2)+"_sources"
        prefix: fulldir
        files: [
            "**/*.cpp",
            "**/*.hpp",
//            "**/*.c",
//            "**/*.h",
            "**/*.ui",
            "**/*.qrc"
        ]
        excludeFiles: "zbar/internal/**/*"
    }    
}


