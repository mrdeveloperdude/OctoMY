import qbs.FileInfo

Project {
    qbsSearchPaths: [ FileInfo.joinPaths(path, "integration", "qbs") + FileInfo.pathSeparator()]

    references: [
          "src/combined/lib.qbs"
        , "src/testapp/app.qbs"
        , "src/agent/app.qbs"
        , "src/remote/app.qbs"
        , "src/hub/app.qbs"
        , "src/zoo/app.qbs"
        , "src/web/app.qbs"
    ]
}
