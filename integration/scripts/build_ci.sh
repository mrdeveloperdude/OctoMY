#!/bin/bash

qmk="/home/lennart/Qt/5.8/gcc_64/bin/qmake"
overrides="pri/local_overrides.pri"
pro="OctoMY.pro"
bdir="build"
cat << EOF > $overrides

#DEFINES += USE_DOCS
DEFINES += USE_TESTS_BASIC
#DEFINES += USE_TESTS_WEB
#DEFINES += USE_TESTS_STRESS
#DEFINES += USE_TESTS_SELECTED
#DEFINES += USE_QT3D
#DEFINES += EXTERNAL_LIB_ESPEAK
#DEFINES += EXTERNAL_LIB_OPENCV
#DEFINES += EXTERNAL_LIB_OPENCL
DEFINES += USE_ARDUINO
#DEFINES += USE_STATUS

CONFIG += silent

EOF


mkdir -p "$bdir"
pushd "$bdir"
"$qmk" "../$pro"
make -j9
make check
popd


