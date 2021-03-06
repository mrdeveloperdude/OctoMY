# Useful build debugging tool
# Included last in all important .pro files
# Comment in the relevant lines below to see
# the values given to important parameters
# for the build at the point of inclusion

#contains(DEFINES, OC_USE_FEATURE_STATUS){
message(" ")
message(" ")
message("===============================================")
message("====== " $${TARGET} " " $${DEFINES} " ======")
message("| Qt: " $$[QT_VERSION] " @ " $$[QT_INSTALL_PREFIX])
message("| PRO_PWD:    " $${_PRO_FILE_PWD_})
message("| PRO_BUILD:  " $$shadowed($$_PRO_FILE_PWD_))
message("| TOP_PWD:    " $${TOP_PWD})
message("| SRCS:       " $${SRCS})
message("| TOP_BUILD:  " $${TOP_BUILD})
message("| BUILD_SRCS: " $${BUILD_SRCS})
message("| PRIS:       " $${PRIS})
message("| OC_LIB_PREFIX: " $${OC_LIB_PREFIX})
message("| OC_LIB_PREFIX_DIR: " $${OC_LIB_PREFIX_DIR})
message("| TESTS:      " $${TESTS})
message("| CONFIG:     " $${CONFIG})
message("| QT:         " $${QT})
message("| SUBDIRS:		" $${SUBDIRS})
message("| INCLUDEPATH:    " $${INCLUDEPATH})
message("| DEPENDPATH:     " $${DEPENDPATH})
message("| PRE_TARGETDEPS: " $${PRE_TARGETDEPS})
message("| TARGETDEPS: " $${TARGETDEPS})
message("| QMAKE_EXTRA_COMPILERS: " $${QMAKE_EXTRA_COMPILERS})
message("| LOCAL_OVERRIDES ENABLED: " $${LOCAL_OVERRIDES})
message("| DEFINES: " $${DEFINES})
message("----------- C++ compiler --------------------")
message("| CLAGS:    " $${QMAKE_CFLAGS})
message("| CC:       " $${QMAKE_CC})
gcc:message("| $${QMAKE_CC} c version:   " $$system($${QMAKE_CC} --version) )
msvc:message("| $${QMAKE_CC} c version:   " $$system($${QMAKE_CC} ) )
message("----------- C Compiler ----------------------")
message("| CXXFLAGS: " $${QMAKE_CXXFLAGS})
message("| CXX:      " $${QMAKE_CXX})
gcc:message("| $${QMAKE_CXX} c++ version:   " $$system($${QMAKE_CXX} --version) )
msvc:message("| $${QMAKE_CXX} c++ version:   " $$system($${QMAKE_CXX} ) )
message("----------- Source files --------------------")
message("| " $${SOURCES})
message("----------- Header files --------------------")
message("| " $${HEADERS})
message("----------- Forms files --------------------")
message("| " $${FORMS})
message("----------- Library files -------------------")
message("| " $${LIBS})
message("----------- Resource files ------------------")
message("| " $${RESOURCES})
message("----------- Distribution files --------------")
message("| " $${DISTFILES})
message("----------- Parser files --------------")
message("| FLEXSOURCES:    " $${FLEXSOURCES})
message("| QLALRSOURCES:   " $${QLALRSOURCES})
message("| GENERATED_HEADERS:   " $${GENERATED_HEADERS})
message("| GENERATED_SOURCES:    " $${GENERATED_SOURCES})
message("-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_")
message(" ")
message(" ")

#}

