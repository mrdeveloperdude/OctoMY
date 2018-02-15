#!/bin/bash


DEPS="librt, libpthread, libstdc++, libm, libgcc_s, libc, libcups, libgstaudio-1.0, libgstvideo-1.0, libgstpbutils-1.0, libgstapp-1.0, libgstbase-1.0, libgstreamer-1.0, libgobject-2.0, libasound, libwayland-egl, libXcomposite, libwayland-client, libwayland-cursor, libXi, libSM, libICE, libXrender, libxcb-xkb, libxcb-render-util, libxcb-sync, libxcb-xfixes, libxcb-xinerama, libxcb-randr, libxcb-render, libxcb-image, libxcb-shm, libxcb-keysyms, libxcb-icccm, libxcb-shape, libxkbcommon-x11, libjpeg, libgbm, libdrm, libfontconfig, libfreetype, libxkbcommon, libX11-xcb, libX11, libxcb, libEGL, libmysqlclient, libodbc, libpq, libsybdb, libpulse, libpng12, libharfbuzz, libudev, libdbus-1, libz, libicui18n, libicuuc, libpcre2-16, libdouble-conversion, libdl, libglib-2.0, libGLESv2, libgssapi_krb5, libgnutls, libavahi-common, libavahi-client, libgsttag-1.0, liborc-0.4, libgmodule-2.0, libffi, libXext, libuuid, libxcb-util, libexpat, libwayland-server, libXau, libXdmcp, libxcb-dri2, libxcb-dri3, libxcb-present, libxshmfence, libmirclient, libltdl, libssl, libcrypto, libldap_r-2.4, libjson-c, libpulsecommon-8.0, libgraphite2, libsystemd, libicudata, libpcre, libglapi, libkrb5, libk5crypto, libcom_err, libkrb5support, libp11-kit, libidn, libtasn1, libnettle,libhogweed, libgmp, libmircommon ,libmirprotobuf, libcapnp-0.5.3, libmircore, libboost_system, libprotobuf-lite, liblber-2.4, libresolv, libsasl2, libgssapi, libwrap, libsndfile, libasyncns, libselinux, liblzma, libgcrypt, libkeyutils, libboost_filesystem, libkj-0.5.3, libheimntlm, libkrb5, libasn1, libhcrypto, libroken, libnsl, libFLAC,libvorbisenc, libgpg-error, libwind, libheimbase, libhx509, libsqlite3, libcrypt, libogg, libvorbis"

for dep in $DEPS
do
	filtered="${dep//,}"
	filtered="${filtered// }"
	echo "DEP: $filtered   --------------------"
#	for lib in $(apt-file search "$filtered" | grep -V "\-dbg" | grep -V "\-dev" )
	for lib in $(dpkg -S "$filtered")
	do
		echo " + $lib"
	done
done



