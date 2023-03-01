Group{
    name: "UNSET_IN_OctoMYFiles_qbs_sources"
    prefix: "sdfsdfsdfsedf"
    excludeFiles: "**/internal/**/*"
    property bool debug: false
    files: {
        const f =         [
                    "**/*.cpp",
                    "**/*.hpp",
                    "**/*.c",
                    "**/*.h",
                    "**/*.ui",
                    "**/*.qrc"
                ];
        if(debug){
            console.info("Files:"+name);
            console.info(" + prefix:"+prefix);
            console.info(" + excludeFiles:"+excludeFiles);
            console.info(" + files:"+f);
        }
        return f;
    }
     
}
