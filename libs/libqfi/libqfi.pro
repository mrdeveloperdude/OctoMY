TARGET =	qfi
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)

INCLUDEPATH += ./


SOURCES  += \
	widgets/qfi/LayoutSquare.cpp \
	widgets/qfi/qfi_ADI.cpp \
	widgets/qfi/qfi_ALT.cpp \
	widgets/qfi/qfi_ASI.cpp \
	widgets/qfi/qfi_HSI.cpp \
	widgets/qfi/qfi_NAV.cpp \
	widgets/qfi/qfi_PFD.cpp \
	widgets/qfi/qfi_TC.cpp \
	widgets/qfi/qfi_VSI.cpp \
	widgets/qfi/WidgetADI.cpp \
	widgets/qfi/WidgetALT.cpp \
	widgets/qfi/WidgetASI.cpp \
	widgets/qfi/WidgetHSI.cpp \
	widgets/qfi/WidgetNAV.cpp \
	widgets/qfi/WidgetPFD.cpp \
	widgets/qfi/WidgetSix.cpp \
	widgets/qfi/WidgetTC.cpp \
	widgets/qfi/WidgetVSI.cpp \

HEADERS  += \
	widgets/qfi/LayoutSquare.h \
	widgets/qfi/qfi_ADI.h \
	widgets/qfi/qfi_ALT.h \
	widgets/qfi/qfi_ASI.h \
	widgets/qfi/qfi_HSI.h \
	widgets/qfi/qfi_NAV.h \
	widgets/qfi/qfi_PFD.h \
	widgets/qfi/qfi_TC.h \
	widgets/qfi/qfi_VSI.h \
	widgets/qfi/WidgetADI.h \
	widgets/qfi/WidgetALT.h \
	widgets/qfi/WidgetASI.h \
	widgets/qfi/WidgetHSI.h \
	widgets/qfi/WidgetNAV.h \
	widgets/qfi/WidgetPFD.h \
	widgets/qfi/WidgetSix.h \
	widgets/qfi/WidgetTC.h \
	widgets/qfi/WidgetVSI.h \

FORMS    += \
	ui/qfi/WidgetADI.ui \
	ui/qfi/WidgetALT.ui \
	ui/qfi/WidgetASI.ui \
	ui/qfi/WidgetHSI.ui \
	ui/qfi/WidgetNAV.ui \
	ui/qfi/WidgetPFD.ui \
	ui/qfi/WidgetSix.ui \
	ui/qfi/WidgetTC.ui \
	ui/qfi/WidgetVSI.ui \


RESOURCES += \
	resources/qfi.qrc \


message("FROM libqfi.pro:")
include($$TOP_PWD/status.pri)

