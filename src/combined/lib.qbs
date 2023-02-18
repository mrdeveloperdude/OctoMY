import qbs.FileInfo

StaticLibrary {
    name: "combined"
    property string path2:path
    
    property string combinedLibDir: FileInfo.cleanPath(path2) + FileInfo.pathSeparator()
    property string srcDir: FileInfo.cleanPath(FileInfo.joinPaths(project.sourceDirectory, "src")) + FileInfo.pathSeparator()
    property string libsDir: FileInfo.cleanPath(FileInfo.joinPaths(srcDir, "libs")) + FileInfo.pathSeparator()
    
    Depends { 
        name: "cpp"
    }
    OctoMYQtDepends {}
    
    
    OctoMYLibProbe {
        id: octomyLibs
        searchPath: libsDir
    }
    
    OctoMYFiles{
        name: FileInfo.fileName(path2)+"_sources"
        prefix: combinedLibDir
    }
    
    OctoMYFiles{
        name: "lib_sources"
        prefix: libsDir
    }

    cpp.includePaths: octomyLibs.libFolders
    cpp.defines: octomyLibs.libDefines
    
    cpp.cxxLanguageVersion: "c++20"
    cpp.cxxStandardLibrary: "libc++"
    
    Export {
        Depends {
            name: "cpp"
        }
        OctoMYQtDepends {}
        property string e:{
            console.info("exportingProduct.sourceDirectory="+exportingProduct.sourceDirectory)
            return exportingProduct.sourceDirectory +FileInfo.pathSeparator();
        }
        cpp.includePaths: octomyLibs.libFolders
        cpp.defines: octomyLibs.libDefines
    }
    
    
}
