########################## FLEX EXTRA COMPILERS

#NOTE: Why not just use LEXSOURCES and leverage qmake's built-in support for
#      flex? Because by using the approach below we get more explisit control
#      over the process (for example which flex binary/version to use and
#      optional arguments about which filenames to use etc).


# Inspiration from https://github.com/openwebos/qt/blob/master/mkspecs/features/lex.prf

flexsource.input = FLEXSOURCES
flexsource.output = ${QMAKE_FILE_BASE}_lexer.cpp
flexsource.commands = flex --header-file=${QMAKE_FILE_BASE}_lexer.h -o ${QMAKE_FILE_BASE}_lexer.cpp ${QMAKE_FILE_IN}
flexsource.variable_out = GENERATED_SOURCES
flexsource.CONFIG += target_predeps

QMAKE_EXTRA_COMPILERS += flexsource

flexheader.input = FLEXSOURCES
flexheader.output = ${QMAKE_FILE_BASE}_lexer.h
flexheader.commands = @echo Pretending to create *_lexer.h on behalf of flex
flexheader.depends = ${QMAKE_FILE_BASE}_lexer.cpp
flexheader.variable_out = GENERATED_HEADERS
flexheader.CONFIG += target_predeps no_link

QMAKE_EXTRA_COMPILERS += flexheader
