#!/bin/bash

VER=1.0-1
NAME=octomy

function make_deb(){

START=$(pwd)
NAME=$1
VER=$2
DIR=${NAME}_${VER}
BUILD_BASE=$START/build

mkdir /tmp/$DIR
cd /tmp/$DIR
mkdir -p usr/local/bin
cp -a $BUILD_BASE/$NAME/$NAME		usr/local/bin

mkdir DEBIAN

cat << EOF > DEBIAN/control
Package: $NAME
Version: $VER
Section: base
Priority: optional
Architecture: amd64
Depends:
Maintainer: Lennart Rolland <lennartrolland@gmail.com>
Description: OctoMY™ $NAME
 Part of a suite of programs for controlling robots of all sizes and shapes.
 Official website: http://octomy.org

EOF

cd $START

dpkg-deb --build /tmp/$DIR
rm -rf /tmp/$DIR

}


make_deb agent $VER
make_deb remote $VER
make_deb hub $VER
make_deb zoo $VER
