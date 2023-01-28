TARGET =	rest
TEMPLATE =	lib
CONFIG +=	staticlib


include($$PRIS/common.pri)
include($$PRIS/lib.pri)
include($$PRIS/lib_list.pri)

# Link objects directly
REST_DEFINES=$$DEFINES
DEFINES += INCLUDER_BUILD
DEFINES -= INCLUDER_LINK

INCLUDEPATH =

#message("1.AUTOLINKS:" $$AUTOLINKS)
#message("2.AUTOLIBS:" $$AUTOLIBS)
AUTOALL=$$AUTOLINKS $$AUTOLIBS
#message("3.AUTOALL:" $$AUTOALL)

#AUTOALL -= $$LIBBASE
for(INCLIBBASE, AUTOALL) {
	INCLIBNAME=lib$${INCLIBBASE}
	ipath= $$SRCS/libs/$${INCLIBNAME}
	bipath=$$BUILD_SRCS/libs/$${INCLIBNAME}
	#message("+INCPATH 4 " $$LIBBASE " =" $$ipath $$bipath)
	INCLUDEPATH += $$ipath
	INCLUDEPATH += $$bipath
}
#message("INCPATH REST FOR " $$LIBBASE " WAS: " $$INCLUDEPATH)


for(LIBBASE, AUTOLINKS) {
	LIBNAME=$$OC_LIB_PREFIX$${LIBBASE}
	contains(AUTOLIB, $$LIBBASE){
		error(" * * * " $$LIBNAME "WAS IN BOTH AUTOLIBS & AUTOLINKS AT THE SAME TIME (rest)")
	}
	else{
		#message("-----------------------------------------------------------------")
		#message("AUTOLINK for '" $$LIBNAME "'" )

		ipath= $$clean_path($$SRCS/libs/$${LIBNAME})
		bipath=$$clean_path($$BUILD_SRCS/libs/$${LIBNAME})

		config_file=$$ipath/config.pri
		exists($$config_file){
			#message("== CONF: "$$config_file)
			include($$config_file)
			OTHER_FILES += $$config_file
		}
		files_file=$$ipath/files.pri
		exists($$files_file){
			#message("== FILES: "$$files_file)
			include($$files_file)
			OTHER_FILES += $$files_file
		}
		else{
			#message("== INC: $$lo)")
			# NOTE: 'true' means recursive:
			#message("== AUTOFILES")
			SOURCES   += $$files($$ipath/*.cpp, true)
			SOURCES   += $$files($$ipath/*.c, true)
			HEADERS   += $$files($$ipath/*.hpp, true)
			HEADERS   += $$files($$ipath/*.h, true)
			FORMS     += $$files($$ipath/*.ui, true)
			RESOURCES += $$files($$ipath/*.qrc, true)
		}

		SOURCES=   $$unique(SOURCES)
		HEADERS=   $$unique(HEADERS)
		FORMS=     $$unique(FORMS)
		RESOURCES= $$unique(RESOURCES)
	}
}

#for(A, AUTOLIBS) { INCLUDEPATH +=$$clean_path($$SRCS/libs/lib$$A)}

# HACK: This is needed because some of the source files in libparse are generated during build
#INCLUDEPATH += $$clean_path($$BUILD_SRCS/libs/libparser/)


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


DEFINES=$$REST_DEFINES
