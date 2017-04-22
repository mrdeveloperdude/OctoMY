TARGET =	dynamics
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)

SOURCES +=\
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


HEADERS  += \
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

contains(DEFINES, USE_STATUS){
message("FROM libdynamics.pro:")
include($$TOP_PWD/status.pri)
}
