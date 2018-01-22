#!/bin/bash


VER=1.0-1
NAME=octomy
PACKAGE=""

if [ ! -z "$1" ]
then
	PACKAGE="$1"
else
	echo "You need to specify the package name"
	exit -1
fi

if [ ! -z "$2" ]
then
	VER="$2"
fi


function make_deb(){

START=$(pwd)
NAME=$1
VER=$2
DIR=${NAME}_${VER}
TMP_DIR="/tmp/$DIR"
BUILD_BASE=$START/build

mkdir "$TMP_DIR"
cd "$TMP_DIR"
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

dpkg-deb --build "$TMP_DIR"
rm -rf "$TMP_DIR"

}


make_deb "$PACKAGE" "$VER"
