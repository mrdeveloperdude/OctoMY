LIBDIR=$$_PRO_FILE_PWD_
LIBNAME=$$basename(_PRO_FILE_PWD_)
LIBBASE=$$str_member($$LIBNAME, 3, -1)

include(lib_list.pri)

#message("ALIBS:  " $$AUTOLIBS)message("ALINKS: " $$AUTOLINKS)

contains(AUTOLIBS, $$LIBBASE ){
	contains(AUTOLINKS, $$LIBBASE){
		error(" * * * " $$LIBNAME "WAS IN BOTH AUTOLIBS & AUTOLINKS AT THE SAME TIME (pro)")
	}
	else{
		#message("-----------------------------------------------------------------")
		#message("AUTOLIB  for '" $$LIBNAME "'" )

		TARGET = $$LIBBASE
		CONFIG += staticlib
		TEMPLATE = lib
		include($$PRIS/common.pri)
		include($$PRIS/lib.pri)

		INCLUDEPATH =
		AUTOALL = $$AUTOLINKS $$AUTOLIBS
#message("AUTOLINKS: " $$AUTOLINKS)message("AUTOLIBS: " $$AUTOLIBS)message("AUTOALL: " $$AUTOALL)
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
			SOURCES   += $$files($$LIBDIR/*.cpp, true)
			SOURCES   += $$files($$LIBDIR/*.c, true)
			HEADERS   += $$files($$LIBDIR/*.hpp, true)
			HEADERS   += $$files($$LIBDIR/*.h, true)
			FORMS     += $$files($$LIBDIR/*.ui, true)
			RESOURCES += $$files($$LIBDIR/*.qrc, true)
		}

		SOURCES=   $$unique(SOURCES)
		HEADERS=   $$unique(HEADERS)
		FORMS=     $$unique(FORMS)
		RESOURCES= $$unique(RESOURCES)

		contains(DEFINES, USE_STATUS){
			message("FROM " $$LIBBASE ":")
			include($$PRIS/status.pri)
		}

	}
}
else{
	contains(AUTOLINKS, $$LIBBASE){
		#error(" * * * " $$LIBNAME "WAS IN AUTOLINKS when invoking .pro")
	}
	else{
		error(" * * * " $$LIBNAME "WAS IN NEITHER AUTOLIBS OR AUTOLINKS (pro)")
	}
}



#message("-----------------------------------------------------------------")
#message("")


