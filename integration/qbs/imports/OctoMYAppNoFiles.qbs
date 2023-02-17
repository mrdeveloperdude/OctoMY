import qbs.FileInfo

Application {
    property string appDir: FileInfo.cleanPath(path2) + FileInfo.pathSeparator()
    property string srcDir: FileInfo.cleanPath(FileInfo.joinPaths(project.sourceDirectory, "src")) + FileInfo.pathSeparator()
    property string libsDir: FileInfo.cleanPath(FileInfo.joinPaths(srcDir, "libs")) + FileInfo.pathSeparator()
    name: "OctoMYApp"
    OctoMYLibProbe {
        id: octomyLibs
        searchPath: libsDir
    }
  
    Depends { name: "cpp" }

    Depends {
        name: "Qt"
        submodules: [
              "bluetooth"
            , "core"
            , "gui"
            , "multimedia"
            , "multimediawidgets"
            , "openglwidgets"
            , "positioning"
            , "printsupport"
            , "sensors"
            , "serialport"
            , "svgwidgets"
            , "widgets"
            , "xml"
        ]
    }

    //"QT_DISABLE_DEPRECATED_BEFORE=0x060000" // disables all the APIs deprecated before Qt 6.0.0
    cpp.defines: octomyLibs.libDefines
    
    cpp.includePaths: {
        return octomyLibs.libFolders;
    }

    cpp.cxxLanguageVersion: "c++20"
    cpp.cxxStandardLibrary: "libc++"

    install: true
    installDir: qbs.targetOS.contains("qnx") ? FileInfo.joinPaths("/tmp", name, "bin") : base
}
