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
qt_qmake="${qt_prefix_dir}/${qt_version_short}/clang_64/bin/qmake"


function do_apt(){
	local NAME="${BASE}_apt"
	local STRAP_DIR="strap"
	local sources="/etc/apt/sources.list"
	local temp_sources="/etc/apt/sources.list_tmp"
	local COMPONENTS="main, universe, contrib, non-free"
	local MIRROR="http://ftp.no.debian.org/debian"

	local EXCLUDE=""
	local PKGS="";

	cd $PW
	mkdir -p "$NAME"
	cd "$NAME"

	echo "YOU ARE HERE:"
	pwd

	PKGS+=" systemd"
	EXCLUDE+=" --exclude=sysvinit"

	if [ -f "$STRAP_DIR/bin/bash" ]
	then
		# Skip
		echo "FOUND EXISTING BASH BINARY, ABORTING DEBOOTSTRAP STEP"
	else
		# Install debian into the dir
		debootstrap --resolve-deps --include="$PKGS" $EXCLUDE --variant="$VARIANT" --components="$COMPONENTS" --verbose --arch "$ARCH" "$RELEASE" "$STRAP_DIR" "$MIRROR"
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
 && apt-get install -y build-essential perl python git wget libgl1-mesa-dev clang pigz \
#RUN apt-get install -y aptitude
#RUN apt-get purge \$(aptitude search '~i!~M!~prequired!~pimportant!~R~prequired!~R~R~prequired!~R~pimportant!~R~R~pimportant!busybox!grub!initramfs-tools' | awk '{print $2}')
#RUN apt-get purge -y aptitude
RUN apt-get --purge remove x11-* -y && apt-get autoremove -y && apt-get clean -y
RUN rm -rf /var/lib/apt/lists/* /usr/share/man

EOT
	cd $PW
	echo "YOU ARE HERE:"
	pwd
	echo "NAME=${NAME}"
	echo "BASE=${BASE}"
	C="docker build $CACHE -t ${NAME} ${NAME}"
	echo "CMD=$C"
	$C
}




function do_qt(){
	local NAME="${BASE}_qt"

	
	mkdir -p "$NAME"
	cd "$NAME"
	
	if [ -d src ]
	then
		echo "QT5 SRC DIR FOUND, SKIPPING DOWNLOAD"
	else
		mkdir -p src && cd src && wget -O - http://download.qt.io/official_releases/qt/${qt_version_short}/${qt_version}/single/qt-everywhere-opensource-src-${qt_version}.tar.gz | pigz -d | tar pxf - && cd ..
	fi
	cd "$PW/$NAME"
	
	local OPTS=""
	OPTS+=" -release"
	OPTS+=" -opensource"
#	OPTS+=" -static" 
	OPTS+=" -no-use-gold-linker"
	OPTS+=" -prefix \"${qt_prefix_dir}\""
	OPTS+=" -confirm-license"
	OPTS+=" -qpa minimal"
	OPTS+=" -no-qpa-platform-guard"
	OPTS+=" -c++11"
	OPTS+=" -platform linux-clang"
	OPTS+=" -largefile" 
	OPTS+=" -continue"
	OPTS+=" -silent"
	OPTS+=" -qpa xcb"
	OPTS+=" -no-xcb"
	OPTS+=" -no-opengl"
	OPTS+=" -no-pulseaudio"
	OPTS+=" -no-gtkstyle"
	OPTS+=" -no-sql-db2" 
	OPTS+=" -no-sql-ibase" 
	OPTS+=" -no-sql-mysql" 
	OPTS+=" -no-sql-oci" 
	OPTS+=" -no-sql-odbc" 
	OPTS+=" -no-sql-psql" 
#	OPTS+=" -no-sql-sqlite" 
	OPTS+=" -no-sql-sqlite2" 
	OPTS+=" -no-sql-tds"
	#OPTS+=" -no-iconv"
#	OPTS+=" -no-dbus"
#	OPTS+=" -no-glib"
	OPTS+=" -no-gif"
	OPTS+=" -no-nis"
	OPTS+=" -no-cups" 
	OPTS+=" -no-eglfs"
	OPTS+=" -no-directfb" 
	OPTS+=" -no-linuxfb"
	OPTS+=" -no-kms"
	OPTS+=" -nomake examples" 
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
	OPTS+=" -skip qtgraphicaleffects"
	OPTS+=" -skip qtwayland"

	cat << EOT >Dockerfile
FROM		${BASE}_apt
MAINTAINER	${maintainer}

ADD src /src
RUN ln -s /src/qt-everywhere-opensource-src-${qt_version} /src/qt
RUN cd /src/qt && ./configure $OPTS 
#> ${qt_build_log}

RUN cd /src/qt && make -j $(grep -c ^processor /proc/cpuinfo) all 
#> ${qt_build_log}

#EXPOSE 80

#CMD ["/usr/sbin/apache2", "-D",  "FOREGROUND"]

EOT
	cd $PW
	echo "YOU ARE HERE:"
	pwd
	echo "NAME=${NAME}"
	echo "BASE=${BASE}"
	C="docker build $CACHE -t ${NAME} ${NAME}"
	echo "CMD=$C"
	$C

}



function do_zoo(){
	local NAME="${BASE}_zoo"
	mkdir -p "$NAME"
	
	cd "$PW/$NAME"
	
	cat << EOT >Dockerfile
FROM		${BASE}_qt
MAINTAINER	${maintainer}

RUN cd / && git clone https://github.com/mrdeveloperdude/OctoMY.git
RUN cd OctoMY && ${qt_qmake} && make

CMD 

#EXPOSE 80

#CMD ["/usr/sbin/apache2", "-D",  "FOREGROUND"]

EOT
	cd $PW
	echo "YOU ARE HERE:"
	pwd
	echo "NAME=${NAME}"
	echo "BASE=${BASE}"
	C="docker build $CACHE -t ${NAME} ${NAME}"
	echo "CMD=$C"
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
	echo "       + apt"
	echo "       + qt"
	echo "       + zoo"
	echo ""
	exit 1
fi

while [ "${1+defined}" ]
do
    case $1 in
        -v* )	shift; qt_version="$"; qt_version_short="${qt_version%.*}" ;;
        -c* )	CACHE=" --no-cache" ;;
        apt* )	do_apt ;;
        qt* )	do_qt ;;
        zoo* )	do_zoo ;;
        *) echo "UNKNWON COMMAND: '$1', SKIPPING..."    ;;
    esac
    shift
done
 
