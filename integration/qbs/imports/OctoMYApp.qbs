import qbs.FileInfo

OctoMYAppNoFiles {
    OctoMYFiles{
        name: "app_sources"
        prefix: appDir
    }
    // We DON'T use combined libs, include all lib sources directly here
    OctoMYFiles{
        condition: !useCombined
        name: "lib_sources"
        prefix: libsDir
    }
    // We DO use combined libs, depend on combined lib here
    Depends { 
        condition: useCombined
        name: "combined"
    }
}
