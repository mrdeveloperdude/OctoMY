// Add dependency on all Qt modules sued by OctoMY
Depends {
    name: "Qt"
    enableFallback: false
    property bool includeTest:false
    submodules: [].concat([
                              "bluetooth"
                              , "concurrent"
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
                              , "testlib"
                          ], includeTest?[]:["testlib"])
}


