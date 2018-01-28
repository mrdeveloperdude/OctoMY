#!/bin/bash


VER=1.0-1
NAME=octomy
PACKAGE_PATH=""
PACKAGE=""

if [ ! -z "$1" ]
then
	PACKAGE_PATH="$1"
	if [ ! -x "$PACKAGE_PATH" ]
	then
		echo "Specified package '$PACKAGE_PATH' did not exist or was not executable"
		exit -1
	fi
	PACKAGE="$(basename $PACKAGE_PATH)"
	echo "Using package '$PACKAGE' from '$PACKAGE_PATH'"
else
	echo "You need to specify the full package path"
	exit -1
fi

if [ ! -z "$2" ]
then
	VER="$2"
fi

echo "Using version '$VER'"

DIR=${NAME}_${VER}
TMP_DIR="/tmp/$DIR"
BUILD_BASE=$START/build
echo "Using base dir '$BUILD_BASE'"

mkdir -p "$TMP_DIR"
cd "$TMP_DIR"
mkdir -p usr/local/bin
cp -a "$PACKAGE_PATH" usr/local/bin

mkdir DEBIAN

cat << EOF > DEBIAN/control
Package: $NAME
Version: $VER
Section: base
Priority: optional
Architecture: amd64
Depends: librt, libpthread, libstdc++, libm, libgcc_s, libc, libcups, libgstaudio-1.0, libgstvideo-1.0, libgstpbutils-1.0, libgstapp-1.0, libgstbase-1.0, libgstreamer-1.0, libgobject-2.0, libasound, libwayland-egl, libXcomposite, libwayland-client, libwayland-cursor, libXi, libSM, libICE, libXrender, libxcb-xkb, libxcb-render-util, libxcb-sync, libxcb-xfixes, libxcb-xinerama, libxcb-randr, libxcb-render, libxcb-image, libxcb-shm, libxcb-keysyms, libxcb-icccm, libxcb-shape, libxkbcommon-x11, libjpeg, libgbm, libdrm, libfontconfig, libfreetype, libxkbcommon, libX11-xcb, libX11, libxcb, libEGL, libmysqlclient, libodbc, libpq, libsybdb, libpulse, libpng12, libharfbuzz, libudev, libdbus-1, libz, libicui18n, libicuuc, libpcre2-16, libdouble-conversion, libdl, libglib-2.0, libGLESv2, libgssapi_krb5, libgnutls, libavahi-common, libavahi-client, libgsttag-1.0, liborc-0.4, libgmodule-2.0, libffi, libXext, libuuid, libxcb-util, libexpat, libwayland-server, libXau, libXdmcp, libxcb-dri2, libxcb-dri3, libxcb-present, libxshmfence, libmirclient, libltdl, libssl, libcrypto, libldap_r-2.4, libjson-c, libpulsecommon-8.0, libgraphite2, libsystemd, libicudata, libpcre, libglapi, libkrb5, libk5crypto, libcom_err, libkrb5support, libp11-kit, libidn, libtasn1, libnettle,libhogweed, libgmp, libmircommon ,libmirprotobuf, libcapnp-0.5.3, libmircore, libboost_system, libprotobuf-lite, liblber-2.4, libresolv, libsasl2, libgssapi, libwrap, libsndfile, libasyncns, libselinux, liblzma, libgcrypt, libkeyutils, libboost_filesystem, libkj-0.5.3, libheimntlm, libkrb5, libasn1, libhcrypto, libroken, libnsl, libFLAC,libvorbisenc, libgpg-error, libwind, libheimbase, libhx509, libsqlite3, libcrypt, libogg, libvorbis
Maintainer: Lennart Rolland <lennartrolland@gmail.com>
Description: OctoMY™ $NAME
 Part of a suite of programs for controlling robots of all sizes and shapes.
 Official website: http://octomy.org

EOF

cd $START

dpkg-deb --build "$TMP_DIR"
rm -rf "$TMP_DIR"


