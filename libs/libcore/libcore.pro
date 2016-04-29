TARGET =	core
TEMPLATE =	lib
CONFIG +=	staticlib
QT +=		serialport

include($$TOP_PWD/common.pri)

INCLUDEPATH += ./

SOURCES +=\
	3d/PoseView.cpp \
	3d/scene/GeometryEngine.cpp \
	3d/scene/Limb.cpp \
	3d/scene/QtLogo3D.cpp \
	3d/scene/Simulation.cpp \
	agent/Agent.cpp \
	agent/AgentWindow.cpp \
	basic/GenericMain.cpp \
	basic/LogDestination.cpp \
	basic/LogHandler.cpp \
	basic/Settings.cpp \
	basic/Standard.cpp \
	basic/StyleManager.cpp \
	basic/UniquePlatformFingerprint.cpp \
	camera/Camera.cpp \
	camera/CameraSettings.cpp \
	camera/PoorMansProbe.cpp \
	comms/Client.cpp \
	comms/CommsChannel.cpp \
	comms/couriers/Courier.cpp \
	comms/couriers/DirectPoseCourier.cpp \
	comms/couriers/SensorsCourier.cpp \
	comms/FlowControl.cpp \
	comms/messages/QueryMessage.cpp \
	comms/messages/QueryResultMessage.cpp \
	comms/messages/SensorsMessage.cpp \
	comms/ReliabilitySystem.cpp \
	gear/gbody.cpp \
	gear/gconstraint.cpp \
	gear/gconstraint_jointloop.cpp \
	gear/gelement.cpp \
	gear/gjoint_composite.cpp \
	gear/gjoint.cpp \
	gear/gjoint_fixed.cpp \
	gear/gjoint_free.cpp \
	gear/gjoint_planar.cpp \
	gear/gjoint_prismatic.cpp \
	gear/gjoint_revolute.cpp \
	gear/gjoint_spherical.cpp \
	gear/gjoint_translational.cpp \
	gear/gjoint_universal.cpp \
	gear/gspringdamper.cpp \
	gear/gsystem_constrained.cpp \
	gear/gsystem.cpp \
	gear/gsystem_ik.cpp \
	gear/liegroup.cpp \
	gear/liegroup.inl \
	gear/liegroup_rmatrix3_ext.inl \
	gear/rmatrix3j.cpp \
	hub/ClientWindow.cpp \
	hub/Hub.cpp \
	hub/HubWindow.cpp \
	hw/actuators/HexyLeg.cpp \
	hw/actuators/HexySerial.cpp \
	hw/actuators/HexyTool.cpp \
	hw/actuators/SerialSettings.cpp \
	models/ClientModel.cpp \
	plan/parser/PlanHighlighter.cpp \
	plot/qcustomplot.cpp \
	plot/StatsWindow.cpp \
	puppet/GaitController.cpp \
	puppet/GaitWidget.cpp \
	puppet/Pose.cpp \
	puppet/Puppet.cpp \
	remote/Remote.cpp \
	remote/RemoteWindow.cpp \
	security/KeyStore.cpp \
	sensory/SensorInput.cpp \
	utility/BaseTranscode.cpp \
	utility/Status.cpp \
	utility/Utility.cpp \
	widgets/CompasWidget.cpp \
	widgets/ConnectionWidget.cpp \
	widgets/EnumEntry.cpp \
	widgets/FlowLayout.cpp \
	widgets/hexedit/QHexEditComments.cpp \
	widgets/hexedit/QHexEdit.cpp \
	widgets/hexedit/QHexEditData.cpp \
	widgets/hexedit/QHexEditDataDevice.cpp \
	widgets/hexedit/QHexEditDataReader.cpp \
	widgets/hexedit/QHexEditDataWriter.cpp \
	widgets/hexedit/QHexEditHighlighter.cpp \
	widgets/hexedit/QHexEditPrivate.cpp \
	widgets/hexedit/SparseRangeMap.cpp \
	widgets/IdenticonWidget.cpp \
	widgets/ImageLabel.cpp \
	widgets/LightWidget.cpp \
	widgets/LocalAddressEntry.cpp \
	widgets/Logo.cpp \
	widgets/MapEditor.cpp \
	widgets/MultiView.cpp \
	widgets/NumberEntry.cpp \
	widgets/PlanEditor.cpp \
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
	widgets/QRWidget.cpp \
	widgets/SvgWidget.cpp \
	widgets/TextEntry.cpp \
	widgets/TryToggle.cpp \
	security/PortableID.cpp \
	widgets/WaitingSpinnerWidget.cpp \
	basic/Iconv.cpp \
	puppet/FaceWidget.cpp \
	camera/CameraList.cpp \
	hw/BluetoothList.cpp \
	widgets/CameraPairingWidget.cpp \
	widgets/PairingWidget.cpp \
    hw/actuators/SerialList.cpp \
    hw/actuators/ServoInput.cpp \
    puppet/LimbIKWidget.cpp




HEADERS  += \
	3d/PoseView.hpp \
	3d/scene/GeometryEngine.hpp \
	3d/scene/Limb.hpp \
	3d/scene/QtLogo3D.hpp \
	3d/scene/Simulation.hpp \
	agent/Agent.hpp \
	agent/AgentWindow.hpp \
	basic/GenericMain.hpp \
	basic/LogDestination.hpp \
	basic/LogHandler.hpp \
	basic/Settings.hpp \
	basic/Standard.hpp \
	basic/StyleManager.hpp \
	basic/UniquePlatformFingerprint.hpp \
	camera/Camera.hpp \
	camera/CameraSettings.hpp \
	camera/PoorMansProbe.hpp \
	comms/Client.hpp \
	comms/CommsChannel.hpp \
	comms/couriers/Courier.hpp \
	comms/couriers/DirectPoseCourier.hpp \
	comms/couriers/SensorsCourier.hpp \
	comms/DataType.hpp \
	comms/DeviceStatusType.hpp \
	comms/DeviceType.hpp \
	comms/FlowControl.hpp \
	comms/messages/MessageType.hpp \
	comms/messages/QueryMessage.hpp \
	comms/messages/QueryResultMessage.hpp \
	comms/messages/SensorsMessage.hpp \
	comms/QueryType.hpp \
	comms/ReliabilitySystem.hpp \
	gear/gbody.h \
	gear/gconstraint.h \
	gear/gconstraint_jointloop.h \
	gear/gcoordinate.h \
	gear/gear.h \
	gear/gelement.h \
	gear/gforce.h \
	gear/gjoint_composite.h \
	gear/gjoint_fixed.h \
	gear/gjoint_free.h \
	gear/gjoint.h \
	gear/gjoint_planar.h \
	gear/gjoint_prismatic.h \
	gear/gjoint_revolute.h \
	gear/gjoint_spherical.h \
	gear/gjoint_translational.h \
	gear/gjoint_universal.h \
	gear/greal.h \
	gear/gspringdamper.h \
	gear/gsystem_constrained.h \
	gear/gsystem.h \
	gear/gsystem_ik.h \
	gear/liegroup.h \
	gear/liegroup_rmatrix3_ext.h \
	gear/rmatrix3j.h \
	hub/ClientWindow.hpp \
	hub/Hub.hpp \
	hub/HubWindow.hpp \
	hw/actuators/HexyLeg.hpp \
	hw/actuators/HexySerial.hpp \
	hw/actuators/HexyTool.hpp \
	hw/actuators/SerialSettings.hpp \
	models/ClientModel.hpp \
	plan/parser/PlanHighlighter.hpp \
	plot/qcustomplot.hpp \
	plot/StatsWindow.hpp \
	puppet/GaitController.hpp \
	puppet/GaitWidget.hpp \
	puppet/Pose.hpp \
	puppet/Puppet.hpp \
	remote/Remote.hpp \
	remote/RemoteWindow.hpp \
	security/KeyStore.hpp \
	sensory/SensorInput.hpp \
	utility/BaseTranscode.hpp \
	utility/InfInt.hpp \
	utility/Status.hpp \
	utility/Utility.hpp \
	widgets/CompasWidget.hpp \
	widgets/ConnectionWidget.hpp \
	widgets/EnumEntry.hpp \
	widgets/FlowLayout.hpp \
	widgets/hexedit/QHexEditComments.hpp \
	widgets/hexedit/QHexEditDataDevice.hpp \
	widgets/hexedit/QHexEditData.hpp \
	widgets/hexedit/QHexEditDataReader.hpp \
	widgets/hexedit/QHexEditDataWriter.hpp \
	widgets/hexedit/QHexEditHighlighter.hpp \
	widgets/hexedit/QHexEdit.hpp \
	widgets/hexedit/QHexEditPrivate.hpp \
	widgets/hexedit/SparseRangeMap.hpp \
	widgets/IdenticonWidget.hpp \
	widgets/ImageLabel.hpp \
	widgets/LightWidget.hpp \
	widgets/LocalAddressEntry.hpp \
	widgets/Logo.hpp \
	widgets/MapEditor.hpp \
	widgets/MultiView.hpp \
	widgets/NumberEntry.hpp \
	widgets/PlanEditor.hpp \
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
	widgets/QRWidget.hpp \
	widgets/SvgWidget.hpp \
	widgets/TextEntry.hpp \
	widgets/TryToggle.hpp \
	security/PortableID.hpp \
	widgets/WaitingSpinnerWidget.hpp \
	basic/Iconv.hpp \
	puppet/FaceWidget.hpp \
	camera/CameraList.hpp \
	hw/BluetoothList.hpp \
	widgets/CameraPairingWidget.hpp \
	widgets/PairingWidget.hpp \
    hw/actuators/SerialList.hpp \
    hw/actuators/ServoInput.hpp \
    puppet/LimbIKWidget.hpp


FORMS    += \
	ui/AgentWindow.ui \
	ui/CameraSettings.ui \
	ui/Camera.ui \
	ui/ClientWindow.ui \
	ui/ConnectionWidget.ui \
	ui/HexyTool.ui \
	ui/HubWindow.ui \
	ui/MultiView.ui \
	ui/NumberEntry.ui \
	ui/qfi/WidgetADI.ui \
	ui/qfi/WidgetALT.ui \
	ui/qfi/WidgetASI.ui \
	ui/qfi/WidgetHSI.ui \
	ui/qfi/WidgetNAV.ui \
	ui/qfi/WidgetPFD.ui \
	ui/qfi/WidgetSix.ui \
	ui/qfi/WidgetTC.ui \
	ui/qfi/WidgetVSI.ui \
	ui/RemoteWindow.ui \
	ui/ResponsiveTest.ui \
	ui/SerialSettings.ui \
	ui/StatsWindow.ui \
	ui/TryToggle.ui \
	widgets/MapEditor.ui \
	widgets/PlanEditor.ui \
	widgets/CameraPairingWidget.ui \
	widgets/PairingWidget.ui \
    ui/ServoInput.ui


RESOURCES += \
	resources/icons.qrc \
	resources/fonts.qrc \
	resources/style.qrc \
	resources/qfi.qrc \
	resources/3d.qrc \
	resources/data.qrc



OTHER_FILES += \

#According to the link below, native widget support for Qt3D will not be available until at least Qt5.8 or later
#Untill that time comes we will keep back the qt3d enthusiasm and write generalized 3D code that is easy to port
#http://stackoverflow.com/questions/35074830/show-qt3d-stuff-inside-qwidget-in-qt5
contains(DEFINES, USE_QT3D){
HEADERS += \
	3d/qt3d/Qt3DWindow.hpp \
	3d/qt3d/SceneModifier.hpp \

SOURCES += \
	3d/qt3d/Qt3DWindow.cpp \
	3d/qt3d/SceneModifier.cpp \

}

message("FROM lib.pro:")
include($$TOP_PWD/status.pri)

