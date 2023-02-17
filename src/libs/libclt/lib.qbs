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
        //excludeFiles: "zbar/internal/**/*"
        condition: false
    }
    
    Group{
        name: FileInfo.fileName(path2)+"_sources_low"
        prefix: fulldir
        files: [
            "**/CLGLView.*",
            "**/*.ui",
            "**/*.qrc"
        ]
        //excludeFiles: "zbar/internal/**/*"
        condition: true
    }
    
}


