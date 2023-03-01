import qbs.FileInfo

StaticLibrary {
    property string localPath
    name: FileInfo.fileName(localPath);
    
    // Enumerate our libs
    LibProbe{
        id: libs
    }
    
    Files{
        name: FileInfo.baseName(localPath) + "_sources"
        prefix: FileInfo.cleanPath(localPath) + FileInfo.pathSeparator()
        debug:true
    }

    //We depend on Qt
    QtDepends{}

    // We depend on C++
    Depends{
        name: "cpp"
    }

    // Tell cpp to look for header files in all the lib folders
    cpp.includePaths: [].concat(base, libs.folders)
    //cpp.includePaths: [].concat(base, [localPath])
    
    // Export our dependencies
    Export {
        // Export our dependence on Qt
        QtDepends{}
        
        // We export our dependence on C++
        Depends{
            name: "cpp"
        }
        
        // Tell cpp to look for header files in all the lib folders
        cpp.includePaths: [].concat(base, [localPath])
    }
}
