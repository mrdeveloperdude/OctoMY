lo=$$lower($$INCLUDEE)

ipath= $$clean_path($$SRCS/libs/lib$${lo})
bipath=$$clean_path($$BUILD_SRCS/libs/lib$${lo})


# 1. We want access to headers
################################################################################
INCLUDEPATH += $$ipath
# Make sure to include all generated headers such as those resulting from
# moc/parser generators and resources (if any)
INCLUDEPATH += $$bipath


# 2. We want to link libraries
################################################################################
contains(DEFINES, INCLUDER_LINK){
	#message("== LNK: $$lo")
	LIBS +=				$$BUILD_SRCS/libs/lib$${lo} -l$${lo}
	DEPENDPATH +=		$$ipath
	PRE_TARGETDEPS +=	$$BUILD_SRCS/libs/lib$${lo}/lib$${lo}.a
}


# 3. We want to add sources into other build
################################################################################
contains(DEFINES, INCLUDER_BUILD){
	files_file=$$ipath/files.pri
	exists($$files_file){
		#message("== INC: $$lo[files]")
		include($$files_file)
	}
	else{
		#message("== INC: $$lo)")
		# NOTE: 'true' means recursive:
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


# 4. We want to build sources into stand-alone library
################################################################################
contains(DEFINES, INCLUDER_BUILD_LIB){
	files_file=$$ipath/files.pri
	exists($$files_file){
		#message("== INC: $$lo[files]")
		include($$files_file)
	}
	else{
		#message("== INC: $$lo)")
		# NOTE: 'true' means recursive:
		SOURCES   = $$files($$ipath/*.cpp, true)
		SOURCES   = $$files($$ipath/*.c, true)
		HEADERS   = $$files($$ipath/*.hpp, true)
		HEADERS   = $$files($$ipath/*.h, true)
		FORMS     = $$files($$ipath/*.ui, true)
		RESOURCES = $$files($$ipath/*.qrc, true)
	}

	SOURCES=   $$unique(SOURCES)
	HEADERS=   $$unique(HEADERS)
	FORMS=     $$unique(FORMS)
	RESOURCES= $$unique(RESOURCES)

	#$${lo}Target.target = lib$${lo}
	#$${lo}Target.depends = $$SOURCES $$HEADERS $$FORMS $$RESOURCES
	#QMAKE_EXTRA_TARGETS += $${lo}Target
}

