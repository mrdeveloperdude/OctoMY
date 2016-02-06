# FROM http://doc.qt.io/qt-5/qmake-advanced-usage.html#adding-compilers

qlalr.output  = ${QMAKE_FILE_BASE}.cpp
qlalr.commands = qlalr ${QMAKE_FILE_NAME}
#qlalr.depend_command = g++ -E -M ${QMAKE_FILE_NAME} | sed "s,^.*: ,,"
qlalr.input = NEW_HEADERS
qlalr.CONFIG += target_predeps
#qlalr.CONFIG += no_link
#qlalr.CONFIG += no_clean



QMAKE_EXTRA_COMPILERS += qlalr

