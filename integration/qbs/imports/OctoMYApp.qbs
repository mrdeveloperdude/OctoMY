import qbs.FileInfo

OctoMYAppNoFiles {
    Group{
        name: "app_sources"
        prefix: appDir
        files: [
            "*.cpp",
            "*.hpp",
            "*.c",
            "*.h",
            "*.ui",
            "*.qrc"
        ]
    }

    Group{
        name: "lib_sources"
        prefix: libsDir
        excludeFiles: "**/internal/**/*"
        files: [
            "**/*.cpp",
            "**/*.hpp",
            "**/*.c",
            "**/*.h",
            "**/*.ui",
            "**/*.qrc"
        ]
    }
}
