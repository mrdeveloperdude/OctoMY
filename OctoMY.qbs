import qbs.FileInfo

Project {
    name: "OctoMY™"
    qbsSearchPaths: "integration/qbs"
    //qbsModuleProviders: "integration/qbs"
    references: [
        "src/libs/libcombined/lib.qbs"
        //        ,"src/libs.qbs"
        , "test/tests.qbs"
        , "src/apps.qbs"
    ]
}
