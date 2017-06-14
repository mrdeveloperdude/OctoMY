
lo=$$lower($$INCLUDEE)

INCLUDEPATH +=		$$clean_path($$TOP_PWD/libs/lib$${lo})

contains(DEFINES, USE_SEPARATE_LIB){
	#message("== LNK: $$lo")
	LIBS +=				$$TOP_BUILD/libs/lib$${lo} -l$${lo}
	DEPENDPATH +=		$$TOP_PWD/libs/lib$${lo}
	PRE_TARGETDEPS +=	$$TOP_BUILD/libs/lib$${lo}/lib$${lo}.a
}
else{
	files_folder=$$TOP_PWD/libs/lib$${lo}
	files_file=$$files_folder/files.pri
	exists($$files_file){
		#message("== INC: $$lo[files]")
		include($$files_file)
	}
	else{
		#message("== INC: $$lo)")
		SOURCES   += $$files($$files_folder/*.cpp, true)
		SOURCES   += $$files($$files_folder/*.c, true)
		HEADERS   += $$files($$files_folder/*.hpp, true)
		HEADERS   += $$files($$files_folder/*.h, true)
		FORMS     += $$files($$files_folder/*.ui, true)
		RESOURCES += $$files($$files_folder/*.qrc, true)

		SOURCES=   $$unique(SOURCES)
		HEADERS=   $$unique(HEADERS)
		FORMS=     $$unique(FORMS)
		RESOURCES= $$unique(RESOURCES)

		#message("NEW SOURECS: $$SOURCES")
		#message("NEW HEADERS: $$HEADERS")
	}
}

