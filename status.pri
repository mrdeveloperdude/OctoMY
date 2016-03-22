# Useful build debugging tool
# Included last in all important .pro files
# Comment in the relevant lines below to see
# the final values given to important
# parameters for the build


message("")
message("------ " $${TARGET} " " $${DEFINES} " ------")
message("| Qt: " $$[QT_VERSION] " @ " $$[QT_INSTALL_PREFIX])
message("| TOP_PWD:   " $${TOP_PWD})
message("| TOP_BUILD: " $${TOP_BUILD})
message("| PRO_PWD:   " $${_PRO_FILE_PWD_})
message("| PRO_BUILD: " $$shadowed($$_PRO_FILE_PWD_))
message("| CONFIG:    " $${CONFIG})
message("| QT:        " $${QT})
message("| CXXFLAGS:  " $${QMAKE_CXXFLAGS})
message("| CXX:       " $${QMAKE_CXX})
message("| SOURCES:   " $${SOURCES})
message("| HEADERS:   " $${HEADERS})
message("| LIBS:      " $${LIBS})
message("| RESOURCES: " $${RESOURCES})
message("| DISTFILES: " $${DISTFILES})
message("| INCLUDEPATH:    " $${INCLUDEPATH})
message("| DEPENDPATH:     " $${DEPENDPATH})
message("| PRE_TARGETDEPS: " $${PRE_TARGETDEPS})
message("| QMAKE_EXTRA_COMPILERS: " $${QMAKE_EXTRA_COMPILERS})
message("| LOCAL_OVERRIDES ENABLED: " $${LOCAL_OVERRIDES})
message("| C++ Compiler Version: " $$system(c++ --version) )
message("---------------------------------------------")

