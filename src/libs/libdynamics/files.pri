
DYN_TOP= $$SRCS/libs/libdynamics

contains(DEFINES, USE_GEAR){

HEADERS	+= \
	$$DYN_TOP/gear/gbody.h \
	$$DYN_TOP/gear/gconstraint.h \
	$$DYN_TOP/gear/gconstraint_jointloop.h \
	$$DYN_TOP/gear/gcoordinate.h \
	$$DYN_TOP/gear/gear.h \
	$$DYN_TOP/gear/gelement.h \
	$$DYN_TOP/gear/gforce.h \
	$$DYN_TOP/gear/gjoint_composite.h \
	$$DYN_TOP/gear/gjoint_fixed.h \
	$$DYN_TOP/gear/gjoint_free.h \
	$$DYN_TOP/gear/gjoint.h \
	$$DYN_TOP/gear/gjoint_planar.h \
	$$DYN_TOP/gear/gjoint_prismatic.h \
	$$DYN_TOP/gear/gjoint_revolute.h \
	$$DYN_TOP/gear/gjoint_spherical.h \
	$$DYN_TOP/gear/gjoint_translational.h \
	$$DYN_TOP/gear/gjoint_universal.h \
	$$DYN_TOP/gear/greal.h \
	$$DYN_TOP/gear/gspringdamper.h \
	$$DYN_TOP/gear/gsystem_constrained.h \
	$$DYN_TOP/gear/gsystem.h \
	$$DYN_TOP/gear/gsystem_ik.h \
	$$DYN_TOP/gear/liegroup.h \
	$$DYN_TOP/gear/liegroup_rmatrix3_ext.h \
	$$DYN_TOP/gear/rmatrix3j.h \


SOURCES	+= \
	$$DYN_TOP/gear/gbody.cpp \
	$$DYN_TOP/gear/gconstraint.cpp \
	$$DYN_TOP/gear/gconstraint_jointloop.cpp \
	$$DYN_TOP/gear/gelement.cpp \
	$$DYN_TOP/gear/gjoint_composite.cpp \
	$$DYN_TOP/gear/gjoint.cpp \
	$$DYN_TOP/gear/gjoint_fixed.cpp \
	$$DYN_TOP/gear/gjoint_free.cpp \
	$$DYN_TOP/gear/gjoint_planar.cpp \
	$$DYN_TOP/gear/gjoint_prismatic.cpp \
	$$DYN_TOP/gear/gjoint_revolute.cpp \
	$$DYN_TOP/gear/gjoint_spherical.cpp \
	$$DYN_TOP/gear/gjoint_translational.cpp \
	$$DYN_TOP/gear/gjoint_universal.cpp \
	$$DYN_TOP/gear/gspringdamper.cpp \
	$$DYN_TOP/gear/gsystem_constrained.cpp \
	$$DYN_TOP/gear/gsystem.cpp \
	$$DYN_TOP/gear/gsystem_ik.cpp \
	$$DYN_TOP/gear/liegroup.cpp \
	$$DYN_TOP/gear/rmatrix3j.cpp \


}



HEADERS	+= \


SOURCES	+= \



