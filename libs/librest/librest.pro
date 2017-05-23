TARGET =	rest
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)
include($$TOP_PWD/libs/libs_list.pri)

# Link objects directly
DEFINES -= USE_SEPARATE_LIB

for(A, AUTOLINKS) {
	INCLUDEE=$$A
	include($$TOP_PWD/libincluder.pri)
}

for(A, AUTOLIBS) {
	INCLUDEPATH +=$$clean_path($$TOP_PWD/libs/lib$$A)
}

# HACK: This is needed because some of the source files int libparse are generated
INCLUDEPATH += $$clean_path($$TOP_BUILD/libs/libparser/)


SOURCES_CLEAN=
for(F, SOURCES) {
	SOURCES_CLEAN += $$clean_path($$F)
}

HEADERS_CLEAN=
for(F, HEADERS) {
	HEADERS_CLEAN += $$clean_path($$F)
}

#FORMS_CLEAN=
#for(F, FORMS) {
#	FORMS_CLEAN += $$relative_path($$clean_path($$F), $$TOP_PWD)
#}


# Refine and normalize the file lists to save bandwidth amd strain on the eyes
SOURCES= $$unique(SOURCES_CLEAN)
HEADERS= $$unique(HEADERS_CLEAN)
#FORMS= $$unique(FORMS_CLEAN)
INCLUDEPATH= $$unique(INCLUDEPATH)

#message("----------- librest Source files -------------------")
#message("| " $${SOURCES})
#message("----------- librest Header files -------------------")
#message("| " $${HEADERS})
#message("----------- librest incpath -------------------")
#message("| " $${INCLUDEPATH})
#message("----------- librest Forms files --------------------")
#message("| " $${FORMS})
#message("--------------------------------------------")
