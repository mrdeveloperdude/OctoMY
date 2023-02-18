import qbs.FileInfo

StaticLibrary {
    Depends { 
        name: "cpp"
    }
    OctoMYQtDepends {}

    property string libdir: FileInfo.relativePath(project.sourceDirectory, path2)+FileInfo.pathSeparator()
    property string fulldir: FileInfo.cleanPath(path2)+FileInfo.pathSeparator()
    
    name:{
        var n=FileInfo.fileName(path2);
        console.info("AUTOLIBBBBBB    libdir=" + libdir+", n="+n+", path2="+path2+", fulldir="+fulldir);
        return n;
    }
    
    Export {
        Depends {
            name: "cpp"
        }
        property string e:{
            console.info("exportingProduct.sourceDirectory="+exportingProduct.sourceDirectory)
            return exportingProduct.sourceDirectory +FileInfo.pathSeparator();
        }
        cpp.includePaths: [ e ]
        cpp.defines: [ "USE_LIB_" + exportingProduct.name.toUpperCase() ]
    }
    
    cpp.includePaths: [ fulldir ]
    cpp.cxxLanguageVersion: "c++20"
    cpp.cxxStandardLibrary: "libc++"
    
    /*
    cpp.includePaths: ["api", "ppi", "xdi"]
    cpp.defines:          ["xxx", "zzz"]
    cpp.cxxFlags:       ["xxx", "zzz"]
    cpp.cFlags:           ["xxx", "zzz"]
    cpp.commonCompilerFlags: ["-Os", "-gdwarf-2", "-Wall"]
    cpp.linkerScripts: "xxx"
    */
    
    
}
