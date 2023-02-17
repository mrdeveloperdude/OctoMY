# This is the special purpose library that combines all libraries named in variable AUTOLIBS_COMBINED (defined in lib_list.pri) into one single library called libcombined
TARGET =	combined
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)
include($$PRIS/lib_list.pri)

# Link objects directly
COMBINED_DEFINES=$$DEFINES
DEFINES += INCLUDER_BUILD
DEFINES -= INCLUDER_LINK

INCLUDEPATH =

#message("1.AUTOLIBS_COMBINED:" $$AUTOLIBS_COMBINED)
#message("2.AUTOLIBS_SEPARATE:" $$AUTOLIBS_SEPARATE)
AUTOALL=$$AUTOLIBS_COMBINED $$AUTOLIBS_SEPARATE
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
#message("INCPATH COMBINED FOR " $$LIBBASE " WAS: " $$INCLUDEPATH)


for(LIBBASE, AUTOLIBS_COMBINED) {
	LIBNAME=$$OC_LIB_PREFIX$${LIBBASE}
	contains(AUTOLIB, $$LIBBASE){
		error(" * * * " $$LIBNAME "WAS IN BOTH AUTOLIBS_SEPARATE & AUTOLIBS_COMBINED AT THE SAME TIME (combined)")
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

			LOCAL_LIB_SOURCES_CPP = $$files($$ipath/*.cpp, true)
			LOCAL_LIB_SOURCES_C += $$files($$ipath/*.c, true)
			LOCAL_LIB_HEADERS_HPP = $$files($$ipath/*.hpp, true)
			LOCAL_LIB_HEADERS_H += $$files($$ipath/*.h, true)
			LOCAL_LIB_FORMS_UI = $$files($$ipath/*.ui, true)
			LOCAL_LIB_RESOURCES_QRC += $$files($$ipath/*.qrc, true)

			#message("CPP SOURCES FOUND FOR $${LIBNAME} WAS $${LOCAL_LIB_SOURCES_CPP}")
			#¤message("C SOURCES FOUND FOR $${LIBNAME} WAS $${LOCAL_LIB_SOURCES_C}")
			#message("HPP HEADERS FOUND FOR $${LIBNAME} WAS $${LOCAL_LIB_HEADERS_HPP}")
			#message("H SOURCES FOUND FOR $${LIBNAME} WAS $${LOCAL_LIB_HEADERS_H}")
			#message("UI FORMS FOUND FOR $${LIBNAME} WAS $${LOCAL_LIB_FORMS_UI}")
			#message("QRC RESOURCES FOUND FOR $${LIBNAME} WAS $${LOCAL_LIB_RESOURCES_QRC}")

			SOURCES   += $$LOCAL_LIB_SOURCES_CPP $$LOCAL_LIB_SOURCES_C
			HEADERS   += $$LOCAL_LIB_HEADERS_HPP $$LOCAL_LIB_HEADERS_H
			FORMS     += $$LOCAL_LIB_FORMS_UI
			RESOURCES += $$LOCAL_LIB_RESOURCES_QRC
		}

		SOURCES=   $$unique(SOURCES)
		HEADERS=   $$unique(HEADERS)
		FORMS=     $$unique(FORMS)
		RESOURCES= $$unique(RESOURCES)
	}
}

#for(A, AUTOLIBS_SEPARATE) { INCLUDEPATH +=$$clean_path($$SRCS/libs/lib$$A)}

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

FORMS_CLEAN=
for(F, FORMS) {
	FORMS_CLEAN += $$clean_path($$F)
}

RESOURCES_CLEAN=
for(F, RESOURCES) {
	RESOURCES_CLEAN += $$clean_path($$F)
}


# Refine and normalize the file lists to save bandwidth amd strain on the eyes
SOURCES= $$unique(SOURCES_CLEAN)

HEADERS= $$unique(HEADERS_CLEAN)

FORMS= $$unique(FORMS_CLEAN)

RESOURCES= $$unique(RESOURCES_CLEAN)

INCLUDEPATH= $$unique(INCLUDEPATH)

#message("----------- libcombined Source files -------------------")
#message("| " $${SOURCES})
#message("----------- libcombined Header files -------------------")
#message("| " $${HEADERS})
#message("----------- libcombined incpath -------------------")
#message("| " $${INCLUDEPATH})
#message("----------- libcombined Forms files --------------------")
#message("| " $${FORMS})
#message("--------------------------------------------")


DEFINES=$$COMBINED_DEFINES

