########################## QLALR EXTRA COMPILERS

# Inspiration from https://github.com/openwebos/qt/blob/master/mkspecs/features/lex.prf

qlalr_parser_cpp.input = QLALRSOURCES
qlalr_parser_cpp.output = ${QMAKE_FILE_BASE}_parser.cpp
qlalr_parser_cpp.commands = $$[QT_INSTALL_BINS]/qlalr --no-lines ${QMAKE_FILE_IN}
qlalr_parser_cpp.variable_out = GENERATED_SOURCES
qlalr_parser_cpp.CONFIG += target_predeps

QMAKE_EXTRA_COMPILERS += qlalr_parser_cpp

qlalr_table_cpp.input = QLALRSOURCES
qlalr_table_cpp.output = ${QMAKE_FILE_BASE}_table.cpp
qlalr_table_cpp.depends = ${QMAKE_FILE_BASE}_parser.cpp
qlalr_table_cpp.commands = @echo Pretending to create *_table.cpp on behalf of qlalr
qlalr_table_cpp.variable_out = GENERATED_SOURCES
qlalr_table_cpp.CONFIG += target_predeps

#NOTE: Why so many extra compilers? Because qmake cannot handle more than ONE
#      output file per compiler. It will silently ignore all but the first one
#      For example:
#      my_extra_compiler.output = a b c
#      Here "b" and "c" will be ignored.
#      and without the extra compilers specified qmake will have the wrong idea
#      about dependencies which makes it fail the build in various ways,
#      depending on which order it decides to run your build.
#      Thus we made one "real" extra compiler and 3 fake ones to accomodate
#      the 4 files output from qlalr

QMAKE_EXTRA_COMPILERS += qlalr_table_cpp

qlalr_parser_hpp.input = QLALRSOURCES
qlalr_parser_hpp.output = ${QMAKE_FILE_BASE}_parser.hpp
qlalr_parser_hpp.depends = ${QMAKE_FILE_BASE}_parser.cpp
qlalr_parser_hpp.commands = @echo Pretending to create *_parser.hpp on behalf of qlalr
qlalr_parser_hpp.variable_out = GENERATED_HEADERS
qlalr_parser_hpp.CONFIG += target_predeps no_link

QMAKE_EXTRA_COMPILERS += qlalr_parser_hpp

qlalr_table_p_h.input = QLALRSOURCES
qlalr_table_p_h.output = ${QMAKE_FILE_BASE}_table_p.h
qlalr_table_p_h.depends = ${QMAKE_FILE_BASE}_parser.cpp
qlalr_table_p_h.commands = @echo Pretending to create *_table_p.h on behalf of qlalr
qlalr_table_p_h.variable_out = GENERATED_HEADERS
qlalr_table_p_h.CONFIG += target_predeps no_link

QMAKE_EXTRA_COMPILERS += qlalr_table_p_h
