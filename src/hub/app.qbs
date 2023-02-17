QtApplication {
    name: "Hub"
    Depends { name: "Qt.widgets" }
    Depends{
        name: "libcombined"
    }
    
    cpp.defines: [
        "QT_DISABLE_DEPRECATED_BEFORE=0x060000" // disables all the APIs deprecated before Qt 6.0.0
    ]
    
    Group{
        name: "sources"
        prefix: "src/"+name.toLowerCase()+"/"
        files: [
            "**/*.cpp",
            "**/*.hpp",
            "**/*.c",
            "**/*.h",
            "**/*.ui",
            "**/*.qrc"
        ]
    }
    
    install: true
    installDir: qbs.targetOS.contains("qnx") ? FileInfo.joinPaths("/tmp", name, "bin") : base
}



