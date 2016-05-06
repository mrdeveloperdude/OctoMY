#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi


PW=$(pwd)
BASE="octomy"
CACHE=""
RELEASE="jessie"
ARCH="amd64"
VARIANT="minbase"

maintainer="OctoMY™ Project by Lennart Rolland <lennartrolland@gmail.com>"

qt_version="5.6.0"
qt_version_short="${qt_version%.*}"
qt_build_log="qt-${qt_version}-build-log.txt"
qt_prefix_dir="/usr/local/Qt"



function do_debian(){
	local NAME="${BASE}_apt"
	local STRAP_DIR="strap"
	local sources="/etc/apt/sources.list"
	local temp_sources="/etc/apt/sources.list_tmp"
	local COMPONENTS="main, universe, contrib, non-free"
	local MIRROR="http://ftp.no.debian.org/debian"

	local EXCLUDE=""
	local PKGS="";
	local DEPS=""
		DEPS+=" libasound2-dev"
		DEPS+=" libsqlite-dev"
		DEPS+=" libsqlite3-dev"
		DEPS+=" libssl-dev"
		DEPS+=" gnutls-dev"
#		DEPS+=" libsslcommon2-dev" #Not in Debian
		DEPS+=" build-essential"
		DEPS+=" perl"
		DEPS+=" python"
		DEPS+=" git"
		DEPS+=" wget"
		DEPS+=" libgl1-mesa-dev"
		DEPS+=" clang"
		DEPS+=" pigz"
		DEPS+=" glx-alternative-mesa"
		DEPS+=" libgl1-mesa-dev"
		DEPS+=" libgl1-mesa-dev"
		DEPS+=" ca-certificates"
		DEPS+=" libasyncns-dev"
		DEPS+=" libc-dev"
		DEPS+=" libdbus-1-dev"
		DEPS+=" libdrm-dev"
		DEPS+=" libexpat-dev"
		DEPS+=" libffi-dev"
		DEPS+=" libFLAC-dev"
		DEPS+=" libfontconfig-dev"
		DEPS+=" libgbm-dev"
		DEPS+=" libICE-dev"
		DEPS+=" libjson-c-dev"
		DEPS+=" liblzma-dev"
		DEPS+=" libnspr4-dev"
		DEPS+=" libnss3-dev"
		DEPS+=" libogg-dev"
		DEPS+=" libpng12-dev"
		DEPS+=" libpulse-dev"
		DEPS+=" libSM-dev"
		DEPS+=" libsndfile-dev"
		DEPS+=" libvorbis-dev"
		DEPS+=" libX11-dev"
		DEPS+=" libX11-xcb-dev"
		DEPS+=" libXau-dev"
		DEPS+=" libxcb-dri3-dev"
		DEPS+=" libxcb-present-dev"
		DEPS+=" libxcb-sync-dev"
		DEPS+=" libXcomposite-dev"
		DEPS+=" libXcursor-dev"
		DEPS+=" libXdamage-dev"
		DEPS+=" libXdmcp-dev"
		DEPS+=" libXext-dev"
		DEPS+=" libXi-dev"
		DEPS+=" libxml2-dev"
		DEPS+=" libXrender-dev"
		DEPS+=" libxshmfence-dev"
		DEPS+=" libxslt-dev"
		DEPS+=" libXtst-dev"
		DEPS+=" libXxf86vm-dev"
		DEPS+=" libz-dev"
		
		# Module spesific for qt3d
		DEPS+=" libassimp-dev"						# Asset management (loading/saving of 3D content) library
		
		# Module spesific for qtconnectivity (bluetooth++)
#		DEPS+=" libbluez-dev"						# Bluetooth support (UBUNTU)
		DEPS+=" libbluetooth-dev"						# Bluetooth support (DEBIAN)
		
		# Module spesific for qtimageformats 
		DEPS+=" libjasper-dev"						# Jpeg2000 image support
		DEPS+=" libmng-dev"							# mng image support
		DEPS+=" libtiff-dev"						# tiff image support
		DEPS+=" libwebp-dev"						# webp image support
		DEPS+=" libwebp-dev"						# webp image support

		# Module spesific for qtlocation
#		DEPS+=" libgypsy-dev"						# libgypsy GPS multiplexing daemon (UBUNTU)
		DEPS+=" libgps-dev"						# libgypsy GPS multiplexing daemon (DEBIAN)

		# Module spesific for qtmultimedia
		DEPS+=" libopenal-dev"						# OpenAL portable hardware accelerated autio library
		DEPS+=" libasound2-dev"						# ALSA2 low level sound library for Linux 

		#NOTE: one of these might provide "gst/interfaces/photography.h
		DEPS+=" libgstreamer-plugins-bad1.0-dev"	# GStreamer development files for libraries from the "ba
		DEPS+=" libgstreamer-plugins-base1.0-dev"	# GStreamer development files for libraries from the "ba
#		DEPS+=" libgstreamer-plugins-good1.0-dev"	# GStreamer development files for libraries from the "go (UBUNTU)
		
		DEPS+=" libgstreamer0.10-dev"
		DEPS+=" libgstreamer1.0-dev"
		DEPS+=" libgstreamer-plugins-base0.10-dev"

		
#		DEPS+=" libgstbuzztard-dev"					# Buzztard - Support plugins for GStreamer (UBUNTU)
		DEPS+=" libgstreamer-vaapi1.0-dev"			# VA-API stuff for Gstreamer (interface for hardware acceleration of media encoding/decoding)

		# Module spesific for qtwayland
		DEPS+=" libwayland-dev"						# Wayland compositor infrastructure - development files
		DEPS+=" libxkbcommon-dev"					# Library interface to the XKB compiler
		DEPS+=" libxcomposite-dev"					# X11 Composite extension library (development headers)
		
		# Module spesific for qtlinuxfb
		DEPS+=" libdirectfb-dev"					# direct framebuffer stuff 
		
		DEPS+=" libgles2-mesa-dev"
		DEPS+=" mesa-common-dev"
		DEPS+=" libgl1-mesa-dev"
		DEPS+=" libegl1-mesa-dev"
		

		# Module spesific for qtwebengine
		DEPS+=" libcap-dev" # Development and header files for libcap
		DEPS+=" libsnappy-dev" # Fast compression/decompression library
		DEPS+=" libsrtp-dev" # Secure RTP (SRTP) and UST Reference Implementations

	
	cd $PW
	mkdir -p "$NAME"
	cd "$NAME"
	
	PKGS+=" systemd"
	EXCLUDE+=" --exclude=sysvinit"

	if [ -f "$STRAP_DIR/bin/bash" ]
	then
		# Skip
		echo "FOUND EXISTING BASH BINARY, ABORTING DEBOOTSTRAP STEP"
	else
		# Install debian into the dir
		debootstrap --resolve-deps --include="$PKGS" $EXCLUDE --variant="$VARIANT" --components="$COMPONENTS" --verbose --arch "$ARCH" "$RELEASE" "$STRAP_DIR" "$MIRROR"
		chgrp -R docker "$STRAP_DIR"
	fi
	
	cat << EOT >Dockerfile
FROM		scratch
MAINTAINER	${maintainer}

ADD $STRAP_DIR /
RUN ["/bin/bash", "-c", "unlink /bin/sh"]
RUN ["/bin/bash", "-c", "ln -s /bin/bash /bin/sh"]
RUN sed s/^deb/deb-src/ ${sources} > ${temp_sources}
RUN cat ${temp_sources} >> ${sources}
# From https://wiki.debian.org/ReduceDebian
RUN echo "APT::Install-Recommends "0" ; APT::Install-Suggests "0" ;" >> /etc/apt/apt.conf
RUN apt-key update \
 && apt-get update \
 && apt-get build-dep -y qt5-default \
 && apt-get install -y  $DEPS \
#RUN apt-get install -y aptitude
#RUN apt-get purge \$(aptitude search '~i!~M!~prequired!~pimportant!~R~prequired!~R~R~prequired!~R~pimportant!~R~R~pimportant!busybox!grub!initramfs-tools' | awk '{print $2}')
#RUN apt-get purge -y aptitude
# THIS RUINED A LOT:
#RUN apt-get --purge remove x11-* -y && 
RUN apt-get autoremove -y && apt-get clean -y
RUN rm -rf /var/lib/apt/lists/* /usr/share/man

EOT
	cd $PW
	C="docker build $CACHE -t ${NAME} ${NAME}"
	echo "-- $(pwd) NAME=${NAME}, BASE=${BASE}, CMD=$C"
	$C
}






function do_ubuntu(){
	local NAME="${BASE}_apt"

	local DEPS=""
		DEPS+=" libasound2-dev"
		DEPS+=" libsqlite-dev"
		DEPS+=" libsqlite3-dev"
		DEPS+=" libssl-dev"
		DEPS+=" libgnutls-dev"
#		DEPS+=" libsslcommon2-dev" #Not in Debian
		DEPS+=" build-essential"
		DEPS+=" perl"
		DEPS+=" python"
		DEPS+=" git"
		DEPS+=" wget"
		DEPS+=" libgl1-mesa-dev"
		DEPS+=" clang"
		DEPS+=" pigz"
		DEPS+=" libgl1-mesa-dev"
		DEPS+=" libgl1-mesa-dev"
		DEPS+=" ca-certificates"
		DEPS+=" libasyncns-dev"
		DEPS+=" libc-dev"
		DEPS+=" libdbus-1-dev"
		DEPS+=" libdrm-dev"
		DEPS+=" libexpat-dev"
		DEPS+=" libffi-dev"
		DEPS+=" libFLAC-dev"
		DEPS+=" libfontconfig-dev"
		DEPS+=" libgbm-dev"
		DEPS+=" libICE-dev"
		DEPS+=" libjson-c-dev"
		DEPS+=" liblzma-dev"
		DEPS+=" libnspr4-dev"
		DEPS+=" libnss3-dev"
		DEPS+=" libogg-dev"
		DEPS+=" libpng12-dev"
		DEPS+=" libpulse-dev"
		DEPS+=" libSM-dev"
		DEPS+=" libsndfile-dev"
		DEPS+=" libvorbis-dev"
		DEPS+=" libX11-dev"
		DEPS+=" libX11-xcb-dev"
		DEPS+=" libXau-dev"
		DEPS+=" libxcb-dri3-dev"
		DEPS+=" libxcb-present-dev"
		DEPS+=" libxcb-sync-dev"
		DEPS+=" libXcomposite-dev"
		DEPS+=" libXcursor-dev"
		DEPS+=" libXdamage-dev"
		DEPS+=" libXdmcp-dev"
		DEPS+=" libXext-dev"
		DEPS+=" libXi-dev"
		DEPS+=" libxml2-dev"
		DEPS+=" libXrender-dev"
		DEPS+=" libxshmfence-dev"
		DEPS+=" libxslt-dev"
		DEPS+=" libXtst-dev"
		DEPS+=" libXxf86vm-dev"
		DEPS+=" libz-dev"
		
		# Module spesific for qt3d
		DEPS+=" libassimp-dev"						# Asset management (loading/saving of 3D content) library
		
		# Module spesific for qtconnectivity (bluetooth++)
		DEPS+=" bluez"						# Bluetooth support (UBUNTU)
		DEPS+=" libbluetooth-dev"
		
		# Module spesific for qtimageformats 
		DEPS+=" libjasper-dev"						# Jpeg2000 image support
		DEPS+=" libmng-dev"							# mng image support
		DEPS+=" libtiff-dev"						# tiff image support
		DEPS+=" libwebp-dev"						# webp image support
		DEPS+=" libwebp-dev"						# webp image support

		# Module spesific for qtlocation
		DEPS+=" libgypsy-dev"						# libgypsy GPS multiplexing daemon (UBUNTU)
#		DEPS+=" libgps-dev"						# libgypsy GPS multiplexing daemon (DEBIAN)

		# Module spesific for qtmultimedia
		DEPS+=" libopenal-dev"						# OpenAL portable hardware accelerated autio library
		DEPS+=" libasound2-dev"						# ALSA2 low level sound library for Linux 

		#NOTE: one of these might provide "gst/interfaces/photography.h
		DEPS+=" libgstreamer-plugins-bad1.0-dev"	# GStreamer development files for libraries from the "ba
		DEPS+=" libgstreamer-plugins-base1.0-dev"	# GStreamer development files for libraries from the "ba
		DEPS+=" libgstreamer-plugins-good1.0-dev"	# GStreamer development files for libraries from the "go (UBUNTU)
		
		DEPS+=" libgstreamer0.10-dev"
		DEPS+=" libgstreamer1.0-dev"
		DEPS+=" libgstreamer-plugins-base0.10-dev"

		
		DEPS+=" libgstbuzztard-dev"					# Buzztard - Support plugins for GStreamer (UBUNTU)
		DEPS+=" libgstreamer-vaapi1.0-dev"			# VA-API stuff for Gstreamer (interface for hardware acceleration of media encoding/decoding)

		# Module spesific for qtwayland
		DEPS+=" libwayland-dev"						# Wayland compositor infrastructure - development files
		DEPS+=" libxkbcommon-dev"					# Library interface to the XKB compiler
		DEPS+=" libxcomposite-dev"					# X11 Composite extension library (development headers)
		
		# Module spesific for qtlinuxfb
		DEPS+=" libdirectfb-dev"					# direct framebuffer stuff 
		
		DEPS+=" libgles2-mesa-dev"
		DEPS+=" mesa-common-dev"
		DEPS+=" libgl1-mesa-dev"
		DEPS+=" libegl1-mesa-dev"
		

		# Module spesific for qtwebengine
		DEPS+=" libcap-dev" # Development and header files for libcap
		DEPS+=" libsnappy-dev" # Fast compression/decompression library
		DEPS+=" libsrtp-dev" # Secure RTP (SRTP) and UST Reference Implementations

	
	cd $PW
	mkdir -p "$NAME"
	cd "$NAME"
	
	
	cat << EOT >Dockerfile
FROM		ubuntu:14.04
MAINTAINER	${maintainer}

RUN ["/bin/bash", "-c", "unlink /bin/sh"]
RUN ["/bin/bash", "-c", "ln -s /bin/bash /bin/sh"]

RUN apt-key update \
 && apt-get update \
 && apt-get build-dep -y qt5-default \
 && apt-get install -y  $DEPS \

RUN apt-get autoremove -y && apt-get clean -y
RUN rm -rf /var/lib/apt/lists/* /usr/share/man

EOT
	cd $PW
	C="docker build $CACHE -t ${NAME} ${NAME}"
	echo "-- $(pwd) NAME=${NAME}, BASE=${BASE}, CMD=$C"
	$C
}






function do_qt_src(){
	local NAME="${BASE}_qt_src"

	mkdir -p "$NAME"
	cd "$NAME"
	
	if [ -d src ]
	then
		echo "QT5 SRC DIR FOUND, SKIPPING DOWNLOAD"
	else
		mkdir -p src && cd src && wget -O - http://download.qt.io/official_releases/qt/${qt_version_short}/${qt_version}/single/qt-everywhere-opensource-src-${qt_version}.tar.gz | pigz -d | tar pxf - && cd ..
	fi
	cd "$PW/$NAME"
	
	cat << EOT >Dockerfile
FROM		${BASE}_apt
MAINTAINER	${maintainer}

ADD src /src
RUN ln -s /src/qt-everywhere-opensource-src-${qt_version} /src/qt


EOT
	cd $PW
	C="docker build $CACHE -t ${NAME} ${NAME}"
	echo "-- $(pwd) NAME=${NAME}, BASE=${BASE}, CMD=$C"
	$C

}


function do_qt_conf(){
	local NAME="${BASE}_qt_conf"
	
	mkdir -p "$NAME"
	cd "$NAME"
	
	cd "$PW/$NAME"
	
	local OPTS=""
	
	if true
	then
		OPTS+=" -opensource"
		OPTS+=" -confirm-license"
		OPTS+=" -nomake examples"
		OPTS+=" -nomake tests"
		OPTS+=" -nomake tools" 
		OPTS+=" -no-compile-examples"
		OPTS+=" -no-xcb"
		OPTS+=" -prefix /usr/local/Qt"
		OPTS+=" -no-warnings-are-errors"
		
		OPTS+=" -skip qtwebengine"
		OPTS+=" -skip qtwebview"
		OPTS+=" -skip qtwinextras"
		OPTS+=" -skip qtx11extras"
		OPTS+=" -skip qtwebsockets"
		OPTS+=" -skip qtwebchannel"
		OPTS+=" -skip qtquickcontrols"
		OPTS+=" -skip qtquickcontrols2"
		OPTS+=" -skip qtmacextras"
		OPTS+=" -skip qtandroidextras"
		OPTS+=" -skip qtcanvas3d"
		OPTS+=" -skip qtdeclarative"
		OPTS+=" -skip qtenginio"



		OPTS+=" -dbus-runtime"
		OPTS+=" -no-gtkstyle"
		OPTS+=" -no-sql-db2" 
		OPTS+=" -no-sql-ibase" 
		OPTS+=" -no-sql-mysql" 
		OPTS+=" -no-sql-oci" 
		OPTS+=" -no-sql-odbc" 
		OPTS+=" -no-sql-psql" 
		OPTS+=" -no-sql-sqlite2" 
		OPTS+=" -no-sql-tds"
		OPTS+=" -no-gif"
		OPTS+=" -no-nis"
		OPTS+=" -no-cups" 

		
	else

		# BASIC
#		OPTS+=" -silent"
		OPTS+=" -release"
	#	OPTS+=" -static" 
		OPTS+=" -prefix \"${qt_prefix_dir}\""
		OPTS+=" -continue"
		OPTS+=" -no-warnings-are-errors"

		# LICENCE
		OPTS+=" -opensource"
		OPTS+=" -confirm-license"

		# PLATFORM		
		OPTS+=" -qpa wayland"
		OPTS+=" -no-qpa-platform-guard"
		OPTS+=" -largefile" 

		# TOOLS
		OPTS+=" -c++11"
		OPTS+=" -platform linux-clang"
		OPTS+=" -no-use-gold-linker"
		OPTS+=" -no-qml-debug"

		# OPTIONAL MODULES, COMPONENTS AND FEATURES
		OPTS+=" -no-xcb"
		OPTS+=" -no-pulseaudio"
		OPTS+=" -no-gtkstyle"
		OPTS+=" -no-sql-db2" 
		OPTS+=" -no-sql-ibase" 
		OPTS+=" -no-sql-mysql" 
		OPTS+=" -no-sql-oci" 
		OPTS+=" -no-sql-odbc" 
		OPTS+=" -no-sql-psql" 
		OPTS+=" -no-sql-sqlite2" 
		OPTS+=" -no-sql-tds"
	#	OPTS+=" -no-iconv"
		OPTS+=" -no-gif"
		OPTS+=" -no-nis"
		OPTS+=" -no-cups" 
		OPTS+=" -no-eglfs"
		OPTS+=" -no-directfb" 
	#	OPTS+=" -no-linuxfb" # Sun tzu: To a surrounded enemy, you must leave a way of escape. 
		OPTS+=" -no-kms"
		OPTS+=" -no-strip"

	
		OPTS+=" -dbus-runtime" # runtime is important because dbus test fails for linked
		OPTS+=" -glib"
	
		OPTS+=" -alsa"
		OPTS+=" -opengl es2"
		OPTS+=" -gui"
		OPTS+=" -widgets"
		OPTS+=" -gui"
		OPTS+=" -gstreamer"
	
		OPTS+=" -nomake examples"
		OPTS+=" -nomake tools"  
		OPTS+=" -nomake tests"
		OPTS+=" -no-compile-examples"
		OPTS+=" -skip qtwebengine"
		OPTS+=" -skip qtwebview"
		OPTS+=" -skip qtwinextras"
		OPTS+=" -skip qtx11extras"
		OPTS+=" -skip qtwebsockets"
		OPTS+=" -skip qtwebchannel"
		OPTS+=" -skip qtquickcontrols"
		OPTS+=" -skip qtquickcontrols2"
		OPTS+=" -skip qtmacextras"
		OPTS+=" -skip qtandroidextras"
		OPTS+=" -skip qtcanvas3d"
		OPTS+=" -skip qtdeclarative"
		OPTS+=" -skip qtenginio"
	#	OPTS+=" -skip qtgraphicaleffects"
	#	OPTS+=" -skip qtwayland"
	fi
	
	
	cat << EOT >log.sh

echo "################ config.log files found during QT CONF: ###########"
for i in $(find | grep -i "config\.log")
do
echo "----------------- $i --------------"
cat $i
done
echo "###################################################################"

EOT

	chmod +x log.sh

	cat << EOT >Dockerfile
FROM		${BASE}_qt_src
MAINTAINER	${maintainer}


RUN apt-key update \
 && apt-get update \
 && apt-get remove -y  libdirectfb-dev \
 && apt-get autoremove -y  \
 && apt-get clean -y  


RUN cd /src/qt && ./configure --help && ./configure $OPTS 

ADD log.sh /tmp/
RUN cd /src/qt && /tmp/log.sh

EOT
	cd $PW
	C="docker build $CACHE -t ${NAME} ${NAME}"
	echo "-- $(pwd) NAME=${NAME}, BASE=${BASE}, CMD=$C"
	$C

}



function do_qt_build(){
	local NAME="${BASE}_qt_build"
	
	mkdir -p "$NAME"
	cd "$PW/$NAME"
	
	cat << EOT >log.sh

echo "################ config.log files found during QT BUILD: ##########"
for i in $(find | grep -i "config\.log")
do
echo "----------------- $i --------------"
cat $i
done
echo "###################################################################"

EOT

	chmod +x log.sh

	cat << EOT >Dockerfile
FROM		${BASE}_qt_conf
MAINTAINER	${maintainer}

RUN cd /src/qt && make -j $(grep -c ^processor /proc/cpuinfo) all ; exit 0
#> ${qt_build_log}

ADD log.sh /tmp/
RUN cd /src/qt && /tmp/log.sh

EOT
	cd $PW
	C="docker build $CACHE -t ${NAME} ${NAME}"
	echo "-- $(pwd) NAME=${NAME}, BASE=${BASE}, CMD=$C"
	$C

}





function do_zoo(){
	local NAME="${BASE}_zoo"
	local octomy_bin_dir="/OctoMY"
	local qt_qmake="/src/qt/qtbase/bin/qmake"
	mkdir -p "$NAME"
	
	cd "$PW/$NAME"
	
	cat << EOT >Dockerfile
FROM		${BASE}_qt_build
MAINTAINER	${maintainer}

RUN cd /src/qt && make -j $(grep -c ^processor /proc/cpuinfo) all && make install ; exit 0

RUN cd /src && git clone https://github.com/mrdeveloperdude/OctoMY.git
RUN echo "QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE "
RUN ${qt_qmake} --version
RUN echo "QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE QMAKE "
RUN mkdir -p ${octomy_bin_dir} && cd ${octomy_bin_dir} && ${qt_qmake} /src/OctoMY/OctoMY.pro && make


EXPOSE 8080:80

CMD ["/usr/sbin/apache2", "-D",  "FOREGROUND"]

EOT
	cd $PW
	C="docker build $CACHE -t ${NAME} ${NAME}"
	echo "-- $(pwd) NAME=${NAME}, BASE=${BASE}, CMD=$C"
	$C

}





if [ ! "${1+defined}" ]
then
	echo ""
	echo "   USAGE: $0 <commands>"
	echo ""
	echo "      Where commands is a list of one or more of the following:"
	echo ""
	echo "       -v <QT-VERSION> (default: ${qt_version})"
	echo "       + debian    - use debootstraped minbase debian as base image WANRING: DOES NOT WORK PROPERLY"
	echo "       + ubuntu    - use ubuntu 14.04 from docker hub as base image"
	echo "       + qt_src    - download qt source (if missing) and import into image"
	echo "       + qt_conf   - configure qt and prepare for build"
	echo "       + qt_build  - build qt"
	echo "       + zoo       - clone OctoMY from github and build it"
	echo ""
	exit 1
fi

while [ "${1+defined}" ]
do
    case $1 in
        -v* )	shift; qt_version="$"; qt_version_short="${qt_version%.*}" ;;
        -c* )	CACHE=" --no-cache" ;;
        debian* )	do_debian ;;
        ubuntu* )	do_ubuntu ;;
        qt_src* )	do_qt_src ;;
        qt_conf* )	do_qt_conf ;;
        qt_build* )	do_qt_build ;;
        zoo* )	do_zoo ;;
        *) echo "UNKNWON COMMAND: '$1', SKIPPING..."    ;;
    esac
    shift
done

docker ps -a

