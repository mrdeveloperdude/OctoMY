# Include this .pri to have all internal OctoMY library projects included into your projects

# IMPORTANT: TOP_PWD and TOP_BUILD are defined in .qmake.config in project root

# Most depending at the top

include($$TOP_PWD/libs/libparser/libparser.pri)
include($$TOP_PWD/libs/libqpolarssl/libqpolarssl.pri)
include($$TOP_PWD/libs/libzbar/libzbar.pri)

contains(DEFINES, EXTERNAL_LIB_OPENCL){
	include($$TOP_PWD/libs/libclt/libclt.pri)
}

includes = node agent remote hub zoo plan map qr web style qfi rng audio dynamics puppet sec core util comms ardumy

for(i, includes) {
	INCLUDEE=$$i
	include($$TOP_PWD/libincluder.pri)
}


SOURCES_CLEAN=
for(F, SOURCES) {
	SOURCES_CLEAN += $$relative_path($$clean_path($$F), $$TOP_BUILD)
}

HEADERS_CLEAN=
for(F, HEADERS) {
	HEADERS_CLEAN += $$relative_path($$clean_path($$F), $$TOP_BUILD)
}

#FORMS_CLEAN=
#for(F, FORMS) {
#	FORMS_CLEAN += $$relative_path($$clean_path($$F), $$TOP_PWD)
#}


# Refine and normalize the file lists to save bandwidth amd strain on the eyes
SOURCES= $$unique(SOURCES_CLEAN)
HEADERS= $$unique(HEADERS_CLEAN)
#FORMS= $$unique(FORMS_CLEAN)

#message("----------- Source files -------------------")
#message("| " $${SOURCES})
#message("----------- Header files -------------------")
#message("| " $${HEADERS})
#message("----------- Forms files --------------------")
#message("| " $${FORMS})
#message("--------------------------------------------")



#include($$TOP_PWD/libs/libagent/libagent.pri)
#include($$TOP_PWD/libs/libremote/libremote.pri)
#include($$TOP_PWD/libs/libhub/libhub.pri)
#include($$TOP_PWD/libs/libzoo/libzoo.pri)

#contains(DEFINES, EXTERNAL_LIB_OPENCL){
#include($$TOP_PWD/libs/libclt/libclt.pri)
#}


#include($$TOP_PWD/libs/libplan/libplan.pri)
#include($$TOP_PWD/libs/libmap/libmap.pri)
#include($$TOP_PWD/libs/libqr/libqr.pri)
#include($$TOP_PWD/libs/libzbar/libzbar.pri)
#include($$TOP_PWD/libs/libweb/libweb.pri)
#include($$TOP_PWD/libs/libstyle/libstyle.pri)
#include($$TOP_PWD/libs/libqfi/libqfi.pri)
#include($$TOP_PWD/libs/librng/librng.pri)
#include($$TOP_PWD/libs/libaudio/libaudio.pri)
#include($$TOP_PWD/libs/libdynamics/libdynamics.pri)
#include($$TOP_PWD/libs/libpuppet/libpuppet.pri)
#include($$TOP_PWD/libs/libsec/libsec.pri)
#   #include($$TOP_PWD/libs/libmbedtls/libmbedtls.pri)include($$TOP_PWD/libs/libqpolarssl/libqpolarssl.pri)
#include($$TOP_PWD/libs/libcore/libcore.pri)
#include($$TOP_PWD/libs/libutil/libutil.pri)
#include($$TOP_PWD/libs/libcomms/libcomms.pri)

# Most depended at the bottom

#message("FROM inc.pri:") include($$TOP_PWD/status.pri)
