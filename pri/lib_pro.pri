# This file is intended to be included in each library's .pro file as the only line.
# It will automate all the boilerplate needed to have library building and linking set up


#in case individual settings are needed, we will look for the following:

#projectdir/files.pri : When this file is present, it will be included and we expect it to override the file autodetection
#projectdir/config.pri : When this file is present, it will be included and we expect it to contain any relevant additional configuration options


LIBDIR=$$_PRO_FILE_PWD_
LIBNAME=$$basename(_PRO_FILE_PWD_)
LIBBASE=$$str_member($$LIBNAME, 3, -1)

# This defines AUTOLIBS_SEPARATE and AUTOLIBS_COMBINED variables
include(lib_list.pri)

#message("ALIBS:  " $$AUTOLIBS_SEPARATE)message("ALINKS: " $$AUTOLIBS_COMBINED)

contains(AUTOLIBS_SEPARATE, $$LIBBASE ){
	# Detect and avoid duplicates
	contains(AUTOLIBS_COMBINED, $$LIBBASE){
		error(" * * * " $$LIBNAME "WAS IN BOTH AUTOLIBS_SEPARATE & AUTOLIBS_COMBINED AT THE SAME TIME (pro)")
	}
	else{
		message("-----------------------------------------------------------------")
		message("AUTOLIB  for '" $$LIBNAME "'" )

		TARGET = $$LIBBASE
		CONFIG += staticlib
		TEMPLATE = lib
		include($$PRIS/common.pri)
		include($$PRIS/lib.pri)

		INCLUDEPATH =
		AUTOALL = $$AUTOLIBS_COMBINED $$AUTOLIBS_SEPARATE
#message("AUTOLIBS_COMBINED: " $$AUTOLIBS_COMBINED)message("AUTOLIBS_SEPARATE: " $$AUTOLIBS_SEPARATE)message("AUTOALL: " $$AUTOALL)
		#AUTOALL -= $$LIBBASE
		for(INCLIBBASE, AUTOALL) {
			INCLIBNAME=lib$${INCLIBBASE}
			ipath= $$clean_path($$SRCS/libs/$${INCLIBNAME})
			bipath=$$clean_path($$BUILD_SRCS/libs/$${INCLIBNAME})
			INCLUDEPATH += $$ipath
			INCLUDEPATH += $$bipath
		}
		#message("INCPATH PRO FOR " $$LIBBASE " WAS: " $$INCLUDEPATH)
		#includes = node

		#for(i, includes) {
		#	INCLUDEE=$$i
		#	include($$PRIS/libincluder.pri)
		#}
		config_file=$$LIBDIR/config.pri
		exists($$config_file){
			#message("== INC: $$lo[files]")
			include($$config_file)
		}
		files_file=$$LIBDIR/files.pri
		exists($$files_file){
			#message("== INC: $$lo[files]")
			include($$files_file)
		}
		else{
			#message("== INC: $$lo)")
			# NOTE: 'true' means recursive:
			LOCAL_LIB_SOURCES_CPP =	   $$files($$LIBDIR/*.cpp, true)
			LOCAL_LIB_SOURCES_C +=     $$files($$LIBDIR/*.c, true)
			LOCAL_LIB_HEADERS_HPP =    $$files($$LIBDIR/*.hpp, true)
			LOCAL_LIB_HEADERS_H +=     $$files($$LIBDIR/*.h, true)
			LOCAL_LIB_FORMS_UI =	   $$files($$LIBDIR/*.ui, true)
			LOCAL_LIB_RESOURCES_QRC += $$files($$LIBDIR/*.qrc, true)
			LOCAL_LIB_DISTFILES +=     $$files($$LIBDIR/*.md, true)
			#message("CPP SOURCES FOUND FOR $${LIBNAME} WAS $${LOCAL_LIB_SOURCES_CPP}")
			#¤message("C SOURCES FOUND FOR $${LIBNAME} WAS $${LOCAL_LIB_SOURCES_C}")
			#message("HPP HEADERS FOUND FOR $${LIBNAME} WAS $${LOCAL_LIB_HEADERS_HPP}")
			#message("H SOURCES FOUND FOR $${LIBNAME} WAS $${LOCAL_LIB_HEADERS_H}")
			#message("UI FORMS FOUND FOR $${LIBNAME} WAS $${LOCAL_LIB_FORMS_UI}")
			#message("QRC RESOURCES FOUND FOR $${LIBNAME} WAS $${LOCAL_LIB_RESOURCES_QRC}")
			#message("DISTFILES FOUND FOR $${LIBNAME} WAS $${LOCAL_LIB_DISTFILES}")

			SOURCES   += $$LOCAL_LIB_SOURCES_CPP $$LOCAL_LIB_SOURCES_C
			HEADERS   += $$LOCAL_LIB_HEADERS_HPP $$LOCAL_LIB_HEADERS_H
			FORMS     += $$LOCAL_LIB_FORMS_UI
			RESOURCES += $$LOCAL_LIB_RESOURCES_QRC
			DISTFILES += $$LOCAL_LIB_DISTFILES
		}

		# Remove duplicate files
		SOURCES=   $$unique(SOURCES)
		HEADERS=   $$unique(HEADERS)
		FORMS=     $$unique(FORMS)
		RESOURCES= $$unique(RESOURCES)
		DISTFILES= $$unique(DISTFILES)

		contains(DEFINES, OC_USE_FEATURE_STATUS){
			message("FROM " $$LIBBASE ":")
			include($$PRIS/status.pri)
		}

	}
}
else{
	contains(AUTOLIBS_COMBINED, $$LIBBASE){
		#error(" * * * " $$LIBNAME "WAS IN AUTOLIBS_COMBINED when invoking .pro")
	}
	else{
		error(" * * * " $$LIBNAME "WAS IN NEITHER AUTOLIBS_SEPARATE OR AUTOLIBS_COMBINED (pro)")
	}
}



#message("-----------------------------------------------------------------")
#message("")


