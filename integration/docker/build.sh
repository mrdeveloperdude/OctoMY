#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

DIOC_TOKEN=$(cat "DIGITAL_OCEAN_OAUTH_TOKEN.var")

pw=$(pwd)
app=$(basename $0)

BASE="octomy"
CACHE=""
RELEASE="jessie"
ARCH="amd64"
VARIANT="minbase"

maintainer="OctoMY™ Project by Lennart Rolland <lennartrolland@gmail.com>"

qt_version_long="5.11.2"
qt_version="${qt_version_long%.*}"
qt_build_log="qt-${qt_version}-build-log.txt"
#qt_prefix_dir="/usr/local/Qt"
qt_install_dir="/qt_$qt_version/install"
qt_qmake="$qt_install_dir/bin/qmake"

apt_initial="apt_initial.txt"
apt_early="apt_early.txt"
apt_early_new="apt_early_new.txt"
apt_qt="apt_qt.txt"
apt_qt_new="apt_qt_new.txt"


deb_ver="1.0-1"
deb_name="octomy"
deb_path="./agent"
deb_base="$(basename $deb_path)"
deb_file="$deb_name-$deb_base.deb"

deb_deps=""
deb_deps+="libgstreamer1.0-0"
deb_deps+=", libgstreamer-plugins-base1.0-0"
deb_deps+=", libasound2"
deb_deps+=", libwayland-egl1-mesa"
deb_deps+=", libwayland-cursor0"
deb_deps+=", libxcb-xkb1"
deb_deps+=", libxcb-render-util0"
deb_deps+=", libxcb-xinerama0"
deb_deps+=", libxcb-randr0"
deb_deps+=", libxcb-image0"
deb_deps+=", libxcb-keysyms1"
deb_deps+=", libxcb-icccm4"
deb_deps+=", libpulse0"
deb_deps+=", libharfbuzz0b"
deb_deps+=", libpcre2-16-0"
deb_deps+=", libdouble-conversion1v5"
deb_deps+=", libgles2-mesa"

#DEPS+=", libgles2-mesa"

#librt, libpthread, libstdc++, libm, libgcc_s, libc, libcups, libgstaudio-1.0, libgstvideo-1.0, libgstpbutils-1.0, libgstapp-1.0, libgstbase-1.0, libgstreamer-1.0, libgobject-2.0, libasound, libwayland-egl, libXcomposite, libwayland-client, libwayland-cursor, libXi, libSM, libICE, libXrender, libxcb-xkb, libxcb-render-util, libxcb-sync, libxcb-xfixes, libxcb-xinerama, libxcb-randr, libxcb-render, libxcb-image, libxcb-shm, libxcb-keysyms, libxcb-icccm, libxcb-shape, libxkbcommon-x11, libjpeg, libgbm, libdrm, libfontconfig, libfreetype, libxkbcommon, libX11-xcb, libX11, libxcb, libEGL, libmysqlclient, libodbc, libpq, libsybdb, libpulse, libpng12, libharfbuzz, libudev, libdbus-1, libz, libicui18n, libicuuc, libpcre2-16, libdouble-conversion, libdl, libglib-2.0, libGLESv2, libgssapi_krb5, libgnutls, libavahi-common, libavahi-client, libgsttag-1.0, liborc-0.4, libgmodule-2.0, libffi, libXext, libuuid, libxcb-util, libexpat, libwayland-server, libXau, libXdmcp, libxcb-dri2, libxcb-dri3, libxcb-present, libxshmfence, libmirclient, libltdl, libssl, libcrypto, libldap_r-2.4, libjson-c, libpulsecommon-8.0, libgraphite2, libsystemd, libicudata, libpcre, libglapi, libkrb5, libk5crypto, libcom_err, libkrb5support, libp11-kit, libidn, libtasn1, libnettle,libhogweed, libgmp, libmircommon ,libmirprotobuf, libcapnp-0.5.3, libmircore, libboost_system, libprotobuf-lite, liblber-2.4, libresolv, libsasl2, libgssapi, libwrap, libsndfile, libasyncns, libselinux, liblzma, libgcrypt, libkeyutils, libboost_filesystem, libkj-0.5.3, libheimntlm, libkrb5, libasn1, libhcrypto, libroken, libnsl, libFLAC,libvorbisenc, libgpg-error, libwind, libheimbase, libhx509, libsqlite3, libcrypt, libogg, libvorbis"


deb_dir=${deb_name}_${deb_ver}
tmp_dir="/tmp/$deb_dir"



EDEPS=""
BDEPS=""
DEPS=""
OPTS=""

acmd="apt-get"
lacmd="aptitude search -F '%p' '~i!~M'"
aptgetops="-y --allow-unauthenticated -o Dpkg::Options::=--force-confdef -o Dpkg::Options::=--force-confold"

ephemeral_tag="ephemeral"

function do_test(){
	echo "TROLOLOLOO"
	
	
	cat DROPLET.json | jq -r '.droplet.ip'
}


function do_build(){
	echo ""
	echo ""
	echo ""
	echo ""
	echo "--------------------------------------------------------"
	echo "--------------------------------------------------------"
	echo "--------------------------------------------------------"
	echo "--------------------------------------------------------"
	echo ""
	build_with_dockers
}

function build_with_dockers(){
	#
	#   image naming convention:
	#
	#   <target_os>/<target_os_version>/<target_os_arch>/<qt_version>/<qt_linkade>/<checkout>
	#
	#   Where:
	#
	#   + <target_os> - The OS for which the artifacts are supposed to be built and run. Example: android
	#   + <target_os_version> - The version of the target OS. Example: 28 (Android 9, API level 28)
	#   + <target_os_arch> - The hardware architecture for which the artifacts are supposed to be built and run. Example: armv7.
	#   + <qt_version> - The version of Qt we want to use. NOTE: Qt will be built from source.
	#   + <qt_linkage> - One of "shared", "os", and "static" which determines how to link Qt. NOTE: os means simply use the prebuilt Qt available in OS (not always possible).
	#   + <checkout/branch> - The git checkout hash or branch name that we want to build
	#
	# NOTE: The host os/ver/arch is inferred from the target in question, and internally will be referred to as follows:
	#
	#   + <host_os> - The OS that hosts the build. Example: ubuntu. NOTE: the host OS is not necessarily the OS where the artifacts will run.
	#   + <host_os_version> - The version of the host OS. Example: 18.04. 
	#   + <host_os_arch> - The hardware architecture of the OS that hosts the build. Example: amd64.


	checkout="master"
	linkage="static"

#	build_with_docker "debian-stretch-amd64-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-stretch-x86-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-jessie-amd64-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-jessie-x86-$qt_version-$linkage-$checkout"#
#	build_with_docker "ubuntu-16.04-amd64-$qt_version-$linkage-$checkout"
#	build_with_docker "ubuntu-16.04-x86-$qt_version-$linkage-$checkout"
	build_with_docker "ubuntu-18.04-amd64-$qt_version-$linkage-$checkout"
#	build_with_docker "ubuntu-18.04-x86-$qt_version-$linkage-$checkout"
#	build_with_docker "android-4-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "android-5-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "android-6-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "android-7-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "android-8-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "android-9-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "windows-10-amd64-$qt_version-$linkage-$checkout"
#	build_with_docker "windows-10-x86-$qt_version-$linkage-$checkout"
#	build_with_docker "osx-10.5-amd64-$qt_version-$linkage-$checkout"
#	build_with_docker "osx-10.5-x86-$qt_version-$linkage-$checkout"
#	build_with_docker "ios-10-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "ios-11-arm7-$qt_version-$linkage-$checkout"
	
}

function infer_host_os(){
	local target_os="$1"
	local target_version="$2"
	local target_arch="$3"
	local same=false
	if [ "debian" == "$target_os" ] || [ "ubuntu" == "$target_os" ]
	then
		# Use actual target OS for building
		echo "$target_os" "$target_version" "$target_arch"
		return
	elif [ "android" == "$target_os" ]
	then
		# Our trusty old horse
		echo "debian" "stretch" "amd64"
		return
	elif [ "windows" == "$target_os" ]
	then
		echo "microsoft/windowsservercore" "10.0.14393.1715" "$target_arch"
		return
	elif [ "osx" == "$target_os" ] || [ "ios" == "$target_os" ]
	then
		# This is just to host qemu for running osx
		# https://github.com/Cleafy/sxkdvm/blob/master/Dockerfile
		echo "voidlinux/voidlinux" "latest" "amd64"
		return
	fi
	
	# Unable to infer
	echo "Unable" "to" "infer"
}

function build_with_docker(){
	local spec="$1"
	IFS='-' read -r target_os target_os_version target_os_arch qt_version qt_linkage checkout <<<"$spec"	
	local dir="builds/$target_os/$target_os_version/$target_os_arch/$qt_version/$qt_linkage/$checkout"
	local doc="$pw/$dir/Dockerfile"
	local src_overrides="/src/octomy_$checkout/pri/overrides/release.pri"
	local dst_overrides="/src/octomy_$checkout/pri/local_overrides.pri"
	local octomy_git_version="octomy_version.txt"
	local host_os="Not"
	local host_os_version="set"
	local host_os_arch="yet"
	read host_os host_os_version host_os_arch < <(infer_host_os "$target_os" "$target_os_version" "$target_os_arch")	
	local sources="/etc/apt/sources.list"
	local temp_sources="/etc/apt/sources.list_tmp"

	mkdir -p "$dir"
	pushd "$dir"

	if [ "debian" == "$host_os" ]
	then
		do_common_opts
		do_debian_deps

	elif [ "ubuntu" == "$host_os" ]
	then
		do_common_opts
		do_ubuntu_deps

	elif [ "windows" == "$host_os" ]
	then
		echo "Host OS \'$host_os\' not supported yet"
		exit 1
	else
		echo "Host OS \'$host_os\' not recognized, maybe check your spelling?"
		exit 1
	fi

	git describe --tags --long --dirty --always > "$octomy_git_version"
	
	echo "###################################################"
	echo "  HOST-OS: $host_os-$host_os_version[$host_os_arch]"
	echo "TARGET-OS: $target_os-$target_os_version[$target_os_arch]"
	echo "      PWD: $(pwd)"
	echo "LS GITVER: $(ls -halt "$octomy_git_version")"
	echo "   GITVER: $(cat "$octomy_git_version")"
	echo "###################################################"
	
#	clean_deps $DEPS

	cat << EOF > "$doc"
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
# Autogenerated by $app @ $(date).
#
# Your manual changes will be erased at next run
#
#
# Spec: $spec
#
FROM  $host_os:$host_os_version
LABEL maintainer="$maintainer"

EOF


	if [ "debian" == "$target_os" ] || [ "ubuntu" == "$target_os" ]
	then
		cat << EOF >> "$doc"

RUN >&2 printf "\n\n---- FIX SHELL ------------------------\n"
SHELL ["/bin/bash", "-c"]

RUN echo "CORES AVAILABLE: \$(nproc)"

RUN >&2 printf "\n\n---- ADD APT SRC ----------------------\n" && \
	sed "s/^deb/deb-src/g" ${sources} > ${temp_sources} && \
	cat ${temp_sources} >> ${sources}

# From https://wiki.debian.org/ReduceDebian
ARG DEBIAN_FRONTEND=noninteractive
RUN >&2 printf "\n\n---- INITIALIZE APT -------------------\n" && \
	echo 'APT::Install-Recommends "0" ; APT::Install-Suggests "0" ;' >> /etc/apt/apt.conf && \
	export DEBIAN_FRONTEND=noninteractive && \
	$acmd update $aptgetops && \
	$acmd upgrade $aptgetops && \
	$acmd autoremove $aptgetops && \
	$acmd clean $aptgetops && \
	$acmd install $aptgetops aptitude

WORKDIR /OctoMY

RUN	>&2 printf "\n\n---- RECORD INITIAL DEPENDENCIES ------\n" && \	
	$lacmd | sort | uniq -u > "/OctoMY/$apt_initial"

RUN	>&2 printf "\n\n---- INSTALL EARLY DEPENDENCIES -------\n" && \
	$acmd install $aptgetops $EDEPS

RUN	>&2 printf "\n\n---- RECORD EARLY DEPENDENCIES --------\n" && \	
	$lacmd | sort | uniq -u > "/OctoMY/apt_early.txt" && \
	comm -2 -3 "/OctoMY/$apt_early" "/OctoMY/$apt_initial" > "/OctoMY/$apt_early_new"


RUN	>&2 printf "\n\n---- FIX SSL CERTIFICATES -------------\n" && \
	rm -f /usr/local/share/ca-certificates/certificate.crt; \
	update-ca-certificates --fresh

EOF

	else
		echo "$target_os not supported at this time."
		exit 1
	fi


	cat << EOF >> "$doc"
	
WORKDIR /src

RUN >&2 printf "\n\n---- CLONE QT -------------------------\n" && \
	git clone https://github.com/qt/qt5.git -j\$(nproc) --recurse-submodules -b $qt_version /src/qt_$qt_version

RUN	>&2 printf "\n\n---- CLONE OCTOMY ---------------------\n" && \
	git clone https://github.com/mrdeveloperdude/OctoMY.git -j\$(nproc) --recurse-submodules -b "$checkout" /src/octomy_$checkout

EOF

	if [ "debian" == "$target_os" ] || [ "ubuntu" == "$target_os" ]
	then
		cat << EOF >> "$doc"

RUN >&2 printf "\n\n---- GET QT DEPENDENCIES --------------\n" && \
	$acmd build-dep $aptgetops qt5-default && \
	$acmd install $aptgetops $BDEPS $DEPS

RUN >&2 printf "\n\n---- CLEAN APT ------------------------\n" && \
	$acmd autoremove $aptgetops && \
	$acmd clean $aptgetops && \
	rm -rf  /usr/share/man # /var/lib/apt/lists/*


RUN	>&2 printf "\n\n---- RECORD QT DEPENDENCIES -----------\n" && \	
	$lacmd | sort | uniq -u > "/OctoMY/$apt_qt" && \
	comm -2 -3 "/OctoMY/$apt_qt" "/OctoMY/$apt_early" > "/OctoMY/$apt_qt_new"

WORKDIR /OctoMY


RUN	>&2 printf "\n\n---- SHOW PACKAGES FOR INITIAL --------\n" && \
	cat "/OctoMY/$apt_initial"

RUN	>&2 printf "\n\n---- SHOW PACKAGES FOR EARLY NEW ------\n" && \
	cat "/OctoMY/$apt_early_new"

RUN	>&2 printf "\n\n---- SHOW PACKAGES FOR EARLY ----------\n" && \
	cat "/OctoMY/$apt_early"

RUN	>&2 printf "\n\n---- SHOW PACKAGES FOR QT -------------\n" && \
	cat "/OctoMY/$apt_qt"

RUN	>&2 printf "\n\n---- SHOW PACKAGES FOR QT NEW ---------\n" && \
	cat "/OctoMY/$apt_qt_new"

EOF

	else
		echo "$target_os not supported at this time."
		exit 1
	fi

	cat << EOF >> "$doc"


WORKDIR /qt_$qt_version

RUN >&2 printf "\n\n---- HELP QT --------------------------\n" && \
	/src/qt_$qt_version/configure --help

RUN >&2 printf "\n\n---- CONFIGURE QT ---------------------\n" && \
	/src/qt_$qt_version/configure $OPTS

RUN >&2 printf "\n\n---- INSPECT CONFIGURATION OF QT ------\n" && \
	ls -halt

RUN >&2 printf "\n\n---- BUILD QT -------------------------\n" && \
	MAKEFLAGS=-j\$(nproc) make -k -j \$(nproc) >  qt_build_log.txt ; \
	MAKEFLAGS=-j\$(nproc) make -k -j \$(nproc) >> qt_build_log.txt 

RUN >&2 printf "\n\n---- INSTALL QT -----------------------\n" && \
	mkdir -p "$qt_install_dir"; \
	MAKEFLAGS=-j\$(nproc) make -k -j \$(nproc) install

RUN >&2 printf "\n\n---- SHOW QMAKE VERSION ---------------\n" && \
	"$qt_qmake" --version

WORKDIR /src/octomy_$checkout

# This step is a cache buster that forces rebuild on changes in git repo
ADD "$octomy_git_version" "git_version"

RUN	>&2 echo "\n\n---- UPDATE OCTOMY --------------------\n" && \
		git checkout "$checkout" && \
		git pull
	
WORKDIR /qmake_test

RUN	>&2 printf "\n\n---- BUILD QMAKE TEST -----------------\n" && \
	ls -halt /src/octomy_$checkout/integration/docker/ && \
	MAKEFLAGS=-j\$(nproc) "$qt_qmake" /src/octomy_$checkout/integration/docker/qmake_test/QmakeTest.pro && \
	MAKEFLAGS=-j\$(nproc) make -j \$(nproc); \

RUN	>&2 printf "\n\n---- INSPECT QMAKE TEST ARTEFACTS -----\n" && \
	find

WORKDIR /OctoMY

RUN	>&2 printf "\n\n---- BUILD OCTOMY ---------------------\n" && \
	cp -a "$src_overrides" "$dst_overrides" && \
	cat "$dst_overrides" && \
	MAKEFLAGS=-j\$(nproc) "$qt_qmake" /src/octomy_$checkout/OctoMY.pro; \
	MAKEFLAGS=-j\$(nproc) make -j \$(nproc); \


WORKDIR /OctoMY/bin

RUN	>&2 printf "\n\n---- COLLECT OCTOMY ARTEFACTS ---------\n" && \
	cp -a "/OctoMY/src/agent/agent" ./ && \
	cp -a "/OctoMY/src/remote/remote" ./ && \
	cp -a "/OctoMY/src/hub/hub" ./ && \
	cp -a "/OctoMY/src/zoo/zoo" ./ && \
	ls -halt



WORKDIR /src/upx

RUN >&2 printf "\n\n---- CLONE UPX ------------------------\n" && \
	git clone https://github.com/upx/upx.git -j\$(nproc) --recurse-submodules .

RUN >&2 printf "\n\n---- BUILD UPX ------------------------\n" && \
	$acmd build-dep $aptgetops upx-ucl && \
	MAKEFLAGS=-j\$(nproc) make -k -j \$(nproc) >  upx_build_log.txt

	
WORKDIR /OctoMY/deb

echo "###################################################"
echo "     Using deb package:'$deb_file' from '$deb_path'"
echo "     Using deb version:'$deb_ver'"
echo "Using deb dependencies:'$deb_deps'"
echo "     Using deb tmp dir:'$tmp_dir'"
echo "    Using deb base dir:'$deb_base'"
echo "###################################################"


cat << EOF > "$deb_control"
Package: $deb_name-$deb_base
Version: $deb_ver
Section: base
Priority: optional
Architecture: amd64
Depends: $deb_deps
Maintainer: Lennart Rolland <lennartrolland@gmail.com>
Description: OctoMY™ $deb_name
 Part of a suite of programs for controlling robots of all sizes and shapes.
 Official website: http://octomy.org

EOF

RUN >&2 printf "\n\n---- INSPECT DEB CONTROL FILE ---------\n" && \
	echo "$deb_control"

RUN >&2 printf "\n\n---- CREATE DEB PACKAGE ---------------\n" && \
	mkdir -p "$tmp_dir" && \
	cd "$tmp_dir"  && \
	mkdir -p usr/local/bin  && \
	cp -a "$deb_path" usr/local/bin  && \
	mkdir DEBIAN  && \
	echo "$deb_control" > DEBIAN/control && \
	cd "$deb_base" && \
	dpkg-deb --version && \
	dpkg-deb --build -v -z9 -Zxz -Sextreme --uniform-compression "$tmp_dir" "$deb_file" && \
	dpkg-deb -c "$deb_file" && \
	dpkg-deb -I "$deb_file" && \
	rm -rf "$tmp_dir"

	
# 	cd /src/qt_$qt_version && \
#EXPOSE 8080:80
#CMD ["/usr/sbin/apache2", "-D",  "FOREGROUND"]

EOF

	local tag="octomy:${spec}"
	echo "_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-="
	cat "$doc"
	echo "_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-=_=-="
	# build
	docker build . -t "$tag"

	popd
	
	artefacts=()
	for artefact in agent remote hub zoo
	do
		artefacts=("${artefacts[@]}" "/OctoMY/src/$artefact/$artefact")
	done

	extract_artefacts "$tag" ${artefacts[@]}
	

}

function extract_artefacts(){
	local tag="$1"
	shift
	local artefacts=$@
#	>&2 echo "EXTRACTING ARTEFACTS $artefacts FROM IMAGE '$tag'"
	# Create but don't run container from resulting image
	local container_id=$(docker create "${tag}")
	# Copy artefacts from image
	for artefact in $artefacts
	do
		docker cp "${container_id}:$artefact" "$(basename $artefact)"
	done

	# Container be gone
	docker rm "${container_id}"
	docker images
}


function do_extract(){
	local tag="$1"
	shift
	>&2 echo "EXTRACTING ARTEFACTS WITH TAG=$tag"
	artefacts=()
	for artefact in agent remote hub zoo
	do
		artefacts=("${artefacts[@]}" "/OctoMY/src/$artefact/$artefact")
	done
	extract_artefacts "$tag" ${artefacts[@]}
}




function do_common_opts(){
	
		# INSTALL LOCATION
		OPTS+=" -prefix \"$qt_install_dir\""
		# VERBOSITY
		OPTS+=" -silent"
#		OPTS+=" -verbose"

## https://wiki.qt.io/Qt5_platform_configurations

		# LICENCE
		OPTS+=" -opensource"
		OPTS+=" -confirm-license"

		# BASIC
		OPTS+=" -recheck-all"
		OPTS+=" -c++std c++14"
#		OPTS+=" -continue"
#		OPTS+=" -largefile" 
		OPTS+=" -opengl es2"
		OPTS+=" -qpa xcb"
		OPTS+=" -widgets"
#		OPTS+=" -platform linux-clang"
#		OPTS+=" -qpa wayland"

		# BUNDLED LIBS
		OPTS+=" -qt-pcre"
		OPTS+=" -qt-zlib" # ........... Use the zlib bundled with Qt.
		OPTS+=" -qt-libpng" # ......... Use the libpng bundled with Qt.
		OPTS+=" -qt-libjpeg" # ........ Use the libjpeg bundled with Qt.
		OPTS+=" -qt-freetype" # ....... Use the freetype bundled with Qt.
		OPTS+=" -qt-harfbuzz" # ....... Use the freetype bundled with Qt.
#		OPTS+=" -qt-pcre" # ........... Use the PCRE library bundled with Qt.
		OPTS+=" -qt-xcb" # ............ Use xcb- libraries bundled with Qt.
		OPTS+=" -qt-xkbcommon" # ...... Use xkb libraries bundled with Qt.
		OPTS+=" -qt-doubleconversion"
		OPTS+=" -openssl-linked"
		OPTS+=" -ssl"
	
		
		# LINKAGE
		OPTS+=" -release"
#		OPTS+=" -strip"
#		OPTS+=" -debug"
#		OPTS+=" -no-strip"
		OPTS+=" -static" 
		
		# OPTIMIZATION
		OPTS+=" -optimize-size"
		OPTS+=" -ltcg"
		OPTS+=" -use-gold-linker"
		OPTS+=" -optimized-qmake"
		OPTS+=" -reduce-relocations"
		OPTS+=" -reduce-exports"
		
		# OPTIONAL PARTS
		OPTS+=" -make libs"
		OPTS+=" -nomake examples"
		OPTS+=" -nomake tests"
		OPTS+=" -nomake tools"
#		OPTS+=" -nomake demos" # OLD


		# OPTIONAL FEATURES
		OPTS+=" -dbus-runtime" # runtime is important because dbus test fails for linked
		OPTS+=" -alsa"
		OPTS+=" -glib"
		OPTS+=" -gstreamer"
		OPTS+=" -gui"
		OPTS+=" -inotify"
		OPTS+=" -eventfd"

#		OPTS+=" -no-exceptions" #DOES NOT WORK? INVENSTIGATE
#		OPTS+=" -no-nis"
		OPTS+=" -no-pulseaudio"
#		OPTS+=" -no-qml-debug" #Does not work? invenstigate
#		OPTS+=" -no-qpa-platform-guard"
		OPTS+=" -no-sql-db2" 
		OPTS+=" -no-sql-ibase" 
		OPTS+=" -no-sql-mysql" 
		OPTS+=" -no-sql-oci" 
		OPTS+=" -no-sql-odbc" 
		OPTS+=" -no-sql-psql" 
		OPTS+=" -no-sql-sqlite2" 
		OPTS+=" -no-sql-tds"
		OPTS+=" -no-warnings-are-errors"
#		OPTS+=" -no-xcb"
		OPTS+=" -no-compile-examples"
		OPTS+=" -no-gif"
#		OPTS+=" -no-gtkstyle"
#		OPTS+=" -no-iconv"
		OPTS+=" -no-kms"
		OPTS+=" -no-linuxfb"
		OPTS+=" -no-sql-db2" 
		OPTS+=" -no-sql-ibase" 
		OPTS+=" -no-sql-mysql" 
		OPTS+=" -no-sql-oci" 
		OPTS+=" -no-sql-odbc" 
		OPTS+=" -no-sql-psql" 
#		OPTS+=" -no-sql-sqlite"  # we want sqlite3 support
		OPTS+=" -no-sql-sqlite2" 
		OPTS+=" -no-sql-tds"
		
		OPTS+=" -no-cups" 
		OPTS+=" -no-iconv"
		OPTS+=" -no-dbus"
		OPTS+=" -no-eglfs"
		OPTS+=" -no-directfb" 
		OPTS+=" -no-linuxfb"
		OPTS+=" -no-glib"
		OPTS+=" -no-kms"
#		OPTS+=" -no-openssl"

		# OPTIONAL MODULES AND COMPONENTS
		OPTS+=" -skip qtandroidextras"
		OPTS+=" -skip qtcanvas3d"
		OPTS+=" -skip qtdeclarative"
		OPTS+=" -skip qtenginio"
#		OPTS+=" -skip qtgraphicaleffects"
		OPTS+=" -skip qtmacextras"
#		OPTS+=" -skip qtquickcontrols"
#		OPTS+=" -skip qtquickcontrols2"
		OPTS+=" -skip qtwayland"
		OPTS+=" -skip qtwebchannel"
		OPTS+=" -skip qtwebengine"
		OPTS+=" -skip qtwebsockets"
#		OPTS+=" -skip qtwebkit" # Old
#		OPTS+=" -skip qtwebkit-examples"
		OPTS+=" -skip qtwebview"
		OPTS+=" -skip qtwayland"
		OPTS+=" -skip qtwinextras"
#		OPTS+=" -skip qtx11extras"

}

function do_debian_deps(){
	local os_ver="$1"
	local os_arch="$2"

# Early dependencies
	EDEPS+=" gnupg2"
#	EDEPS+=" apt-utils"
	EDEPS+=" wget"
	EDEPS+=" ca-certificates"
	EDEPS+=" apt-transport-https"
	EDEPS+=" curl"
	EDEPS+=" software-properties-common"
	EDEPS+=" git"

	# Build dependencies
	BDEPS+=" perl"
	BDEPS+=" python"
	BDEPS+=" build-essential"
	BDEPS+=" clang"
	
	
		DEPS+=" libclang-dev"
		DEPS+=" libc-dev"

	# Window system
	DEPS+=" ^libxcb.*-dev"
	DEPS+=" libxcb-dri3-dev"
	DEPS+=" libxcb-present-dev"
	DEPS+=" libxcb-sync-dev"
	DEPS+=" libx11-xcb-dev"
	DEPS+=" libglu1-mesa-dev"
	DEPS+=" libxrender-dev"
	DEPS+=" libxi-dev"

	# GLX
	DEPS+=" libgl1-mesa-dev"
#	DEPS+=" glx-alternative-mesa"
	DEPS+=" libgl1-mesa-dev"
	DEPS+=" libgl1-mesa-dev"
	DEPS+=" libgles2-mesa-dev"
	DEPS+=" mesa-common-dev"
	DEPS+=" libgl1-mesa-dev"
	DEPS+=" libegl1-mesa-dev"
	DEPS+=" libdrm-dev"
	DEPS+=" libgbm-dev"
	
	
	# Network
	DEPS+=" libasyncns-dev"
	
	# Bus
	DEPS+=" libdbus-1-dev"
	
	
	# Xml/ Json
	DEPS+=" libexpat-dev"
	DEPS+=" libjson-c-dev"

	# Security
	DEPS+=" libssl-dev"
	DEPS+=" gnutls-dev"
	DEPS+=" libnss3-dev"
	
	# Database /sqlite
	DEPS+=" libsqlite-dev"
	DEPS+=" libsqlite3-dev"

	# Font
	DEPS+=" libfontconfig-dev"
	
	# Framework
	DEPS+=" libnspr4-dev"

	# Compression	
	DEPS+=" liblzma-dev"
	
		
	# For qtmultimedia
	DEPS+=" libasound2-dev"
	DEPS+=" libffi-dev"
#	DEPS+=" libFLAC-dev"
	DEPS+=" libflac-dev"
	DEPS+=" libgstreamer1.0-dev"
	DEPS+=" libgstreamer-plugins-base1.0-dev"
	DEPS+=" libogg-dev"
	DEPS+=" libpng-dev"
	DEPS+=" libpulse-dev"
	DEPS+=" libsndfile-dev"
	DEPS+=" libvorbis-dev"
	DEPS+=" libxml2-dev"
	
	#NOTE: one of these might provide "gst/interfaces/photography.h
	DEPS+=" libgstreamer-plugins-bad1.0-dev"	# GStreamer development files for libraries from the "ba
	DEPS+=" libgstreamer-plugins-base1.0-dev"	# GStreamer development files for libraries from the "ba
#	DEPS+=" libgstreamer-plugins-good1.0-dev"	# GStreamer development files for libraries from the "go (UBUNTU)
	
	DEPS+=" libgstreamer1.0-dev"
#	DEPS+=" libgstbuzztard-dev"					# Buzztard - Support plugins for GStreamer (UBUNTU)
#	DEPS+=" libgstreamer1.0-vaapi-dev"			# VA-API stuff for Gstreamer (interface for hardware acceleration of media encoding/decoding)

	DEPS+=" libxshmfence-dev"
	DEPS+=" libxslt-dev"
	DEPS+=" libz-dev"
	
	# Module spesific for qt3d
	DEPS+=" libassimp-dev"						# Asset management (loading/saving of 3D content) library
	
	# Module spesific for qtconnectivity (bluetooth++)
#	DEPS+=" libbluez-dev"						# Bluetooth support (UBUNTU)
	DEPS+=" libbluetooth-dev"						# Bluetooth support (DEBIAN)
	
	# Module spesific for qtimageformats 
#	DEPS+=" libjasper-dev"						# Jpeg2000 image support
	DEPS+=" libmng-dev"							# mng image support
	DEPS+=" libtiff-dev"						# tiff image support
	DEPS+=" libwebp-dev"						# webp image support
	DEPS+=" libwebp-dev"						# webp image support

	# Module spesific for qtlocation
#	DEPS+=" libgypsy-dev"						# libgypsy GPS multiplexing daemon (UBUNTU)
	DEPS+=" libgps-dev"						# libgypsy GPS multiplexing daemon (DEBIAN)

	# Module spesific for qtmultimedia
	DEPS+=" libopenal-dev"						# OpenAL portable hardware accelerated autio library
	DEPS+=" libasound2-dev"						# ALSA2 low level sound library for Linux 


	# Module spesific for qtwayland
	#DEPS+=" libwayland-dev"						# Wayland compositor infrastructure - development files
	#DEPS+=" libxkbcommon-dev"					# Library interface to the XKB compiler
	#DEPS+=" libxcomposite-dev"					# X11 Composite extension library (development headers)
	
	# Module spesific for qtlinuxfb
	#DEPS+=" libdirectfb-dev"					# direct framebuffer stuff 
	
	# Module spesific for qtwebengine
	#DEPS+=" libcap-dev" # Development and header files for libcap
	#DEPS+=" libsnappy-dev" # Fast compression/decompression library
	#DEPS+=" libsrtp-dev" # Secure RTP (SRTP) and UST Reference Implementations

}

function comment_for_dep(){
	local dep="$1"
	local dep_desc_dir=".dep_desc"
	local dep_desc_file="$dep_desc_dir/$dep"
	local dep_desc_html_file="$dep_desc_dir/${dep}.html"
	local ret=""
	local pdesc_regex='id="pdesc"\s*>\s*<\s*h2\s*>\s*([^<]*)\s*</h2>' #<h2><h2>\s*\(.*\)\s*<\/h2>'
	
	
	

#  <div id="pdeps"><h2>Packages providing libc-dev</h2><dl>
#    <dt><a href="/stretch/libc6-dev">libc6-dev</a></dt>
#    <dd lang="en">GNU C Library: Development Libraries and Header Files</dd>
#  </dl></div>
  
  
	local pdeps_regex='id="pdeps"\s*>\s*<h2>[^<]*<\/h2>\s*<dl>\s*<dt>\s*<a\s*href="[^"]*">\s*([^<]*)\s*<\/a>\s*<\/dt>\s*<dd\s*lang="en"\s*>\s*([^<]*)\s*<\/dd\s*'
	if [ -f "$dep_desc_file" ]
	then
		ret=$(cat "$dep_desc_file")
	else
		mkdir -p "$dep_desc_dir"
		if [ -d "$dep_desc_dir" ]
		then
			local html=""
			if [ -f "$dep_desc_html_file" ]
			then
				html=$(cat "$dep_desc_html_file")
			else
				html=$(wget -qO- "https://packages.debian.org/stretch/$dep")
				echo "$html" > "$dep_desc_html_file"
			fi
			oneline=$(echo "$html" | tr -d '\r\n')
			if [[ $oneline =~ $pdesc_regex ]]
			then
				ret="${BASH_REMATCH[1]}"
			elif [[ $oneline =~ $pdeps_regex ]]
			then
				ret="${BASH_REMATCH[2]} (by ${BASH_REMATCH[1]})"
			else
				ret="No match"
			fi
		fi
	fi
	echo "$ret"
}



function clean_deps(){
	local deps=$@
	for dep in $deps
	do
		local comment=$(comment_for_dep "$dep")
		#echo "COMMENT: $comment"
		printf "\t%-.50s %s\n" "DEPS+=\" $dep\"                                           " "# $comment"
		#exit 0
	done
}



#Package libgnutls-dev is not available, but is referred to by another package.
#This may mean that the package is missing, has been obsoleted, or
#is only available from another source

#Package libpng12-dev is not available, but is referred to by another package.
#This may mean that the package is missing, has been obsoleted, or
#is only available from another source

##E: Unable to locate package libSM-dev
#E: Unable to locate package libX11-dev
#E: Unable to locate package libX11-xcb-dev
#E: Unable to locate package libXau-dev
#E: Unable to locate package libXcomposite-dev
#E: Unable to locate package libXcursor-dev
#E: Unable to locate package libXdamage-dev
#E: Unable to locate package libXdmcp-dev
#E: Unable to locate package libXext-dev
#E: Unable to locate package libXi-dev
#E: Unable to locate package libXrender-dev
#E: Unable to locate package libXtst-dev
#E: Unable to locate package libXxf86vm-dev
#E: Package 'libgnutls-dev' has no installation candidate
#E: Unable to locate package libsslcommon2-dev
#E: Unable to locate package libgstbuzztard-dev
#E: Unable to locate package libgstreamer1.0-vaapi-dev
#E: Couldn't find any package by glob 'libgstreamer1.0-vaapi-dev'
#E: Couldn't find any package by regex 'libgstreamer1.0-vaapi-dev'
#E: Unable to locate package libFLAC-dev
#E: Unable to locate package libFLAC-dev
#E: Unable to locate package libICE-dev
#E: Package 'libpng12-dev' has no installation candidate
#E: Unable to locate package libSM-dev
#E: Unable to locate package libjasper-dev
#E: Unable to locate package libgstreamer0.10-dev
#E: Couldn't find any package by glob 'libgstreamer0.10-dev'
#E: Couldn't find any package by regex 'libgstreamer0.10-dev'
#E: Unable to locate package libgstreamer-plugins-base0.10-dev
#E: Couldn't find any package by glob 'libgstreamer-plugins-base0.10-dev'
#E: Couldn't find any package by regex 'libgstreamer-plugins-base0.10-dev'
#E: Unable to locate package libgstbuzztard-dev
#E: Unable to locate package libgstreamer-vaapi1.0-dev
#E: Couldn't find any package by glob 'libgstreamer-vaapi1.0-dev'
#E: Couldn't find any package by regex 'libgstreamer-vaapi1.0-dev'


function do_ubuntu_deps(){
	local os_ver="$1"
	local os_arch="$2"
	

	# Early dependencies
	EDEPS+=" gnupg2"
#	EDEPS+=" apt-utils"
	EDEPS+=" wget"
	EDEPS+=" ca-certificates"
	EDEPS+=" apt-transport-https"
	EDEPS+=" curl"
	EDEPS+=" software-properties-common"
	EDEPS+=" git"

	# Build dependencies
	BDEPS+=" perl"
	BDEPS+=" python"
	BDEPS+=" build-essential"
	BDEPS+=" clang"
	
	DEPS+=" libc-dev"
	DEPS+=" libclang-dev"
	
	# Window system
	DEPS+=" libglu1-mesa-dev"
	DEPS+=" ^libxcb.*-dev"
	DEPS+=" libxcb-dri3-dev"
	DEPS+=" libxcb-present-dev"
	DEPS+=" libxcb-sync-dev"
	DEPS+=" libxshmfence-dev"
	

	# Qt5 dev deps (TODO: TEST)
#	DEPS+=" ^libqt.*5-dev"
#	DEPS+=" ^qt.*5-dev"
		
	# GLX
#	DEPS+=" glx-alternative-mesa"
	DEPS+=" libdrm-dev"
	DEPS+=" libegl1-mesa-dev"
	DEPS+=" libgbm-dev"
	DEPS+=" libgl1-mesa-dev"
	DEPS+=" libgles2-mesa-dev"
	DEPS+=" mesa-common-dev"


	# Compression
	DEPS+=" pigz"
	DEPS+=" liblzma-dev"
	DEPS+=" libz-dev"

	# Network
	DEPS+=" libasyncns-dev"
		
	# Bus
	DEPS+=" libdbus-1-dev"

	#Font
	DEPS+=" libfontconfig-dev"
	

	# Frameworks
	DEPS+=" libnspr4-dev"


	# XML / Json
	DEPS+=" libexpat-dev"
	DEPS+=" libjson-c-dev"
	DEPS+=" libxml2-dev"
	DEPS+=" libxslt-dev"

	
	# Security
	DEPS+=" gnutls-dev"
	DEPS+=" libexpat-dev"
	DEPS+=" libnss3-dev"
	DEPS+=" libssl-dev"
	
	# Database/sqlite	
	DEPS+=" libsqlite-dev"
	DEPS+=" libsqlite3-dev"

	
	#NOTE: one of these might provide "gst/interfaces/photography.h
	DEPS+=" libgstreamer-plugins-bad1.0-dev"	# GStreamer development files for libraries from the "ba
	DEPS+=" libgstreamer-plugins-base1.0-dev"	# GStreamer development files for libraries from the "ba
#	DEPS+=" libgstreamer-plugins-good1.0-dev"	# GStreamer development files for libraries from the "go (UBUNTU)
	
	DEPS+=" libgstreamer1.0-dev"


	# For qtmultimedia
	DEPS+=" libasound2-dev"
	DEPS+=" libffi-dev"
	DEPS+=" libflac-dev"
	DEPS+=" libgbm-dev"
	DEPS+=" libgstreamer1.0-dev"
	DEPS+=" libgstreamer-plugins-base1.0-dev"
	DEPS+=" libogg-dev"
	DEPS+=" libpng-dev"
	DEPS+=" libpulse-dev"
	DEPS+=" libsndfile-dev"
	DEPS+=" libvorbis-dev"

# /msg chanserv op #octomy

	DEPS+=" libxshmfence-dev"
	DEPS+=" libxslt-dev"
	DEPS+=" libz-dev"
	
	# Module spesific for qt3d
	DEPS+=" libassimp-dev"						# Asset management (loading/saving of 3D content) library
	
	# Module spesific for qtconnectivity (bluetooth++)
	DEPS+=" bluez"						# Bluetooth support (UBUNTU)
	DEPS+=" libbluetooth-dev"
	
	# Module spesific for qtimageformats 
	DEPS+=" libmng-dev"							# mng image support
	DEPS+=" libtiff-dev"						# tiff image support
	DEPS+=" libwebp-dev"						# webp image support
	DEPS+=" libwebp-dev"						# webp image support

	# Module spesific for qtlocation
#	DEPS+=" libgypsy-dev"						# libgypsy GPS multiplexing daemon (UBUNTU)
	DEPS+=" libgps-dev"						# libgypsy GPS multiplexing daemon (DEBIAN)

	# Module spesific for qtmultimedia
	DEPS+=" libopenal-dev"						# OpenAL portable hardware accelerated autio library
	DEPS+=" libasound2-dev"						# ALSA2 low level sound library for Linux 

	#NOTE: one of these might provide "gst/interfaces/photography.h
	DEPS+=" libgstreamer-plugins-bad1.0-dev"	# GStreamer development files for libraries from the "ba
	DEPS+=" libgstreamer-plugins-base1.0-dev"	# GStreamer development files for libraries from the "ba
	DEPS+=" libgstreamer-plugins-good1.0-dev"	# GStreamer development files for libraries from the "go (UBUNTU)

	DEPS+=" libgstreamer1.0-dev"

	
#	DEPS+=" libgstbuzztard-dev"					# Buzztard - Support plugins for GStreamer (UBUNTU)
#	DEPS+=" libgstreamer-vaapi1.0-dev"			# VA-API stuff for Gstreamer (interface for hardware acceleration of media encoding/decoding)

	# Module spesific for qtwayland
#	DEPS+=" libwayland-dev"						# Wayland compositor infrastructure - development files
#	DEPS+=" libxkbcommon-dev"					# Library interface to the XKB compiler
#	DEPS+=" libxcomposite-dev"					# X11 Composite extension library (development headers)
	
	# Module spesific for qtlinuxfb
#	DEPS+=" libdirectfb-dev"					# direct framebuffer stuff 
	

	# Module spesific for qtwebengine
#	DEPS+=" libcap-dev" # Development and header files for libcap
#	DEPS+=" libsnappy-dev" # Fast compression/decompression library
#	DEPS+=" libsrtp-dev" # Secure RTP (SRTP) and UST Reference Implementations
	
	

}




#		OPTS+=" -no-abstractbutton .......... Widgets: Abstract base class of button widgets, providing functionality common to buttons."
#		OPTS+=" -no-abstractslider .......... Widgets: Common super class for widgets like QScrollBar, QSlider and QDial."
#		OPTS+=" -no-accessibility ........... Utilities: Provides accessibility support."
#		OPTS+=" -no-action .................. Kernel: Provides widget actions."
#		OPTS+=" -no-animation ............... Utilities: Provides a framework for animations."
#		OPTS+=" -no-appstore-compliant ...... Disables code that is not allowed in platform app stores"
#		OPTS+=" -no-bearermanagement ........ Networking: Provides bearer management for the network stack."
#		OPTS+=" -no-big_codecs .............. Internationalization: Supports big codecs, e.g. CJK."
#		OPTS+=" -no-buttongroup ............. Widgets: Supports organizing groups of button widgets."
#		OPTS+=" -no-calendarwidget .......... Widgets: Provides a monthly based calendar widget allowing the user to select a date."
#		OPTS+=" -no-checkbox ................ Widgets: Provides a checkbox with a text label."
#		OPTS+=" -no-clipboard ............... Kernel: Provides cut and paste operations."
#		OPTS+=" -no-codecs .................. Internationalization: Supports non-unicode text conversions."
#		OPTS+=" -no-colordialog ............. Dialogs: Provides a dialog widget for specifying colors."
#		OPTS+=" -no-colornames .............. Painting: Supports color names such as "red", used by QColor and by some HTML documents."
#		OPTS+=" -no-columnview .............. ItemViews: Provides a model/view implementation of a column view."
#		OPTS+=" -no-combobox ................ Widgets: Provides drop-down boxes presenting a list of options to the user."
#		OPTS+=" -no-commandlineparser ....... Utilities: Provides support for command line parsing."
#		OPTS+=" -no-commandlinkbutton ....... Widgets: Provides a Vista style command link button."
#		OPTS+=" -no-completer ............... Utilities: Provides completions based on an item model."
#		OPTS+=" -no-concurrent .............. Kernel: Provides a high-level multi-threading API."
#		OPTS+=" -no-contextmenu ............. Widgets: Adds pop-up menus on right mouse click to numerous widgets."
#		OPTS+=" -no-cssparser ............... Kernel: Provides a parser for Cascading Style Sheets."
#		OPTS+=" -no-cups .................... Painting: Provides support for the Common Unix Printing System."
#		OPTS+=" -no-cursor .................. Kernel: Provides mouse cursors."
#		OPTS+=" -no-datawidgetmapper ........ ItemViews: Provides mapping between a section of a data model to widgets."
#		OPTS+=" -no-datestring .............. Data structures: Provides convertion between dates and strings."
#		OPTS+=" -no-datetimeedit ............ Widgets: Supports editing dates and times."
#		OPTS+=" -no-datetimeparser .......... Utilities: Provides support for parsing date-time texts."
#		OPTS+=" -no-desktopservices ......... Utilities: Provides methods for accessing common desktop services."
#		OPTS+=" -no-dial .................... Widgets: Provides a rounded range control, e.g., like a speedometer."
#		OPTS+=" -no-dialog .................. Dialogs: Base class of dialog windows."
#		OPTS+=" -no-dialogbuttonbox ......... Dialogs: Presents buttons in a layout that is appropriate for the current widget style."
#		OPTS+=" -no-dirmodel ................ ItemViews: Provides a data model for the local filesystem."
#		OPTS+=" -no-dnslookup ............... Networking: Provides API for DNS lookups."
#		OPTS+=" -no-dockwidget .............. Widgets: Supports docking widgets inside a QMainWindow or floated as a top-level window on the desktop."
#		OPTS+=" -no-dom ..................... File I/O: Supports the Document Object Model."
#		OPTS+=" -no-draganddrop ............. Kernel: Supports the drag and drop mechansim."
#		OPTS+=" -no-effects ................. Kernel: Provides special widget effects (e.g. fading and scrolling)."
#		OPTS+=" -no-errormessage ............ Dialogs: Provides an error message display dialog."
#		OPTS+=" -no-filedialog .............. Dialogs: Provides a dialog widget for selecting files or directories."
#		OPTS+=" -no-filesystemiterator ...... File I/O: Provides fast file system iteration."
#		OPTS+=" -no-filesystemmodel ......... File I/O: Provides a data model for the local filesystem."
#		OPTS+=" -no-filesystemwatcher ....... File I/O: Provides an interface for monitoring files and directories for modifications."
#		OPTS+=" -no-fontcombobox ............ Widgets: Provides a combobox that lets the user select a font family."
#		OPTS+=" -no-fontdialog .............. Dialogs: Provides a dialog widget for selecting fonts."
#		OPTS+=" -no-formlayout .............. Widgets: Manages forms of input widgets and their associated labels."
#		OPTS+=" -no-freetype ................ Fonts: Supports the FreeType 2 font engine (and its supported font formats)."
#		OPTS+=" -no-fscompleter ............. Utilities: Provides file name completion in QFileDialog."
#		OPTS+=" -no-ftp ..................... Networking: Provides support for the File Transfer Protocol in QNetworkAccessManager."
#		OPTS+=" -no-future .................. Kernel: Provides QFuture and related classes."
#		OPTS+=" -no-geoservices_esri ........ Location: Provides access to Esri geoservices"
#		OPTS+=" -no-geoservices_here ........ Location: Provides access to HERE geoservices"
#		OPTS+=" -no-geoservices_itemsoverlay . Location: Provides access to the itemsoverlay maps"
#		OPTS+=" -no-geoservices_mapbox ...... Location: Provides access to Mapbox geoservices"
#		OPTS+=" -no-geoservices_mapboxgl .... Location: Provides access to the Mapbox vector maps"
#		OPTS+=" -no-geoservices_osm ......... Location: Provides access to OpenStreetMap geoservices"
#		OPTS+=" -no-gestures ................ Utilities: Provides a framework for gestures."
#		OPTS+=" -no-graphicseffect .......... Widgets: Provides various graphics effects."
#		OPTS+=" -no-graphicsview ............ Widgets: Provides a canvas/sprite framework."
#		OPTS+=" -no-groupbox ................ Widgets: Provides widget grouping boxes with frames."
#		OPTS+=" -no-highdpiscaling .......... Kernel: Provides automatic scaling of DPI-unaware applications on high-DPI displays."
#		OPTS+=" -no-http .................... Networking: Provides support for the Hypertext Transfer Protocol in QNetworkAccessManager."
#		OPTS+=" -no-iconv ................... Internationalization: Provides internationalization on Unix."
#		OPTS+=" -no-identityproxymodel ...... ItemViews: Supports proxying a source model unmodified."
#		OPTS+=" -no-im ...................... Kernel: Provides complex input methods."
#		OPTS+=" -no-image_heuristic_mask .... Images: Supports creating a 1-bpp heuristic mask for images."
#		OPTS+=" -no-image_text .............. Images: Supports image file text strings."
#		OPTS+=" -no-imageformat_bmp ......... Images: Supports Microsoft's Bitmap image file format."
#		OPTS+=" -no-imageformat_jpeg ........ Images: Supports the Joint Photographic Experts Group image file format."
#		OPTS+=" -no-imageformat_png ......... Images: Supports the Portable Network Graphics image file format."
#		OPTS+=" -no-imageformat_ppm ......... Images: Supports the Portable Pixmap image file format."
#		OPTS+=" -no-imageformat_xbm ......... Images: Supports the X11 Bitmap image file format."
#		OPTS+=" -no-imageformat_xpm ......... Images: Supports the X11 Pixmap image file format."
#		OPTS+=" -no-imageformatplugin ....... Images: Provides a base for writing a image format plugins."
#		OPTS+=" -no-inputdialog ............. Dialogs: Provides a simple convenience dialog to get a single value from the user."
#		OPTS+=" -no-itemmodel ............... ItemViews: Provides the item model for item views"
#		OPTS+=" -no-itemmodeltester ......... Provides a utility to test item models."
#		OPTS+=" -no-itemviews ............... ItemViews: Provides the model/view architecture managing the relationship between data and the way it is presented to the user."
#		OPTS+=" -no-keysequenceedit ......... Widgets: Provides a widget for editing QKeySequences."
#		OPTS+=" -no-label ................... Widgets: Provides a text or image display."
#		OPTS+=" -no-lcdnumber ............... Widgets: Provides LCD-like digits."
#		OPTS+=" -no-library ................. File I/O: Provides a wrapper for dynamically loaded libraries."
#		OPTS+=" -no-lineedit ................ Widgets: Provides single-line edits."
#		OPTS+=" -no-listview ................ ItemViews: Provides a list or icon view onto a model."
#		OPTS+=" -no-listwidget .............. Widgets: Provides item-based list widgets."
#		OPTS+=" -no-localserver ............. Networking: Provides a local socket based server."
#		OPTS+=" -no-location-labs-plugin .... Location: Provides experimental QtLocation QML types"
#		OPTS+=" -no-mainwindow .............. Widgets: Provides main application windows."
#		OPTS+=" -no-mdiarea ................. Widgets: Provides an area in which MDI windows are displayed."
#		OPTS+=" -no-menu .................... Widgets: Provides popup-menus."
#		OPTS+=" -no-menubar ................. Widgets: Provides pull-down menu items."
#		OPTS+=" -no-messagebox .............. Dialogs: Provides message boxes displaying informative messages and simple questions."
#		OPTS+=" -no-mimetype ................ Utilities: Provides MIME type handling."
#		OPTS+=" -no-movie ................... Images: Supports animated images."
#		OPTS+=" -no-networkdiskcache ........ Networking: Provides a disk cache for network resources."
#		OPTS+=" -no-networkinterface ........ Networking: Supports enumerating a host's IP addresses and network interfaces."
#		OPTS+=" -no-networkproxy ............ Networking: Provides network proxy support."
#		OPTS+=" -no-paint_debug ............. Painting: Enabled debugging painting with the environment variables QT_FLUSH_UPDATE and QT_FLUSH_PAINT."
#		OPTS+=" -no-pdf ..................... Painting: Provides a PDF backend for QPainter."
#		OPTS+=" -no-picture ................. Painting: Supports recording and replaying QPainter commands."
#		OPTS+=" -no-printdialog ............. Dialogs: Provides a dialog widget for specifying printer configuration."
#		OPTS+=" -no-printer ................. Painting: Provides a printer backend of QPainter."
#		OPTS+=" -no-printpreviewdialog ...... Dialogs: Provides a dialog for previewing and configuring page layouts for printer output."
#		OPTS+=" -no-printpreviewwidget ...... Widgets: Provides a widget for previewing page layouts for printer output."
#		OPTS+=" -no-process ................. File I/O: Supports external process invocation."
#		OPTS+=" -no-processenvironment ...... File I/O: Provides a higher-level abstraction of environment variables."
#		OPTS+=" -no-progressbar ............. Widgets: Supports presentation of operation progress."
#		OPTS+=" -no-progressdialog .......... Dialogs: Provides feedback on the progress of a slow operation."
#		OPTS+=" -no-properties .............. Kernel: Supports scripting Qt-based applications."
#		OPTS+=" -no-proxymodel .............. ItemViews: Supports processing of data passed between another model and a view."
#		OPTS+=" -no-pushbutton .............. Widgets: Provides a command button."
#		OPTS+=" -no-qt3d-animation .......... Aspects: Use the 3D Animation Aspect library"
#		OPTS+=" -no-qt3d-extras ............. Aspects: Use the 3D Extra library"
#		OPTS+=" -no-qt3d-input .............. Aspects: Use the 3D Input Aspect library"
#		OPTS+=" -no-qt3d-logic .............. Aspects: Use the 3D Logic Aspect library"
#		OPTS+=" -no-qt3d-opengl-renderer .... Qt 3D Renderers: Use the OpenGL renderer"
#		OPTS+=" -no-qt3d-render ............. Aspects: Use the 3D Render Aspect library"
#		OPTS+=" -no-qt3d-simd-avx2 .......... Use AVX2 SIMD instructions to accelerate matrix operations"
#		OPTS+=" -no-qt3d-simd-sse2 .......... Use SSE2 SIMD instructions to accelerate matrix operations"
#		OPTS+=" -no-radiobutton ............. Widgets: Provides a radio button with a text label."
#		OPTS+=" -no-regularexpression ....... Kernel: Provides an API to Perl-compatible regular expressions."
#		OPTS+=" -no-resizehandler ........... Widgets: Provides an internal resize handler for dock widgets."
#		OPTS+=" -no-rubberband .............. Widgets: Supports using rubberbands to indicate selections and boundaries."
#		OPTS+=" -no-scrollarea .............. Widgets: Supports scrolling views onto widgets."
#		OPTS+=" -no-scrollbar ............... Widgets: Provides scrollbars allowing the user access parts of a document that is larger than the widget used to display it."
#		OPTS+=" -no-scroller ................ Widgets: Enables kinetic scrolling for any scrolling widget or graphics item."
#		OPTS+=" -no-sessionmanager .......... Kernel: Provides an interface to the windowing system's session management."
#		OPTS+=" -no-settings ................ File I/O: Provides persistent application settings."
#		OPTS+=" -no-sha3-fast ............... Utilities: Optimizes SHA3 for speed instead of size."
#		OPTS+=" -no-sharedmemory ............ Kernel: Provides access to a shared memory segment."
#		OPTS+=" -no-shortcut ................ Kernel: Provides keyboard accelerators and shortcuts."
#		OPTS+=" -no-sizegrip ................ Widgets: Provides corner-grips for resizing top-level windows."
#		OPTS+=" -no-slider .................. Widgets: Provides sliders controlling a bounded value."
#		OPTS+=" -no-socks5 .................. Networking: Provides SOCKS5 support in QNetworkProxy."
#		OPTS+=" -no-sortfilterproxymodel .... ItemViews: Supports sorting and filtering of data passed between another model and a view."
#		OPTS+=" -no-spinbox ................. Widgets: Provides spin boxes handling integers and discrete sets of values."
#		OPTS+=" -no-splashscreen ............ Widgets: Supports splash screens that can be shown during application startup."
#		OPTS+=" -no-splitter ................ Widgets: Provides user controlled splitter widgets."
#		OPTS+=" -no-stackedwidget ........... Widgets: Provides stacked widgets."
#		OPTS+=" -no-standarditemmodel ....... ItemViews: Provides a generic model for storing custom data."
#		OPTS+=" -no-statemachine ............ Utilities: Provides hierarchical finite state machines."
#		OPTS+=" -no-statusbar ............... Widgets: Supports presentation of status information."
#		OPTS+=" -no-statustip ............... Widgets: Supports status tip functionality and events."
#		OPTS+=" -no-stringlistmodel ......... ItemViews: Provides a model that supplies strings to views."
#		OPTS+=" -no-style-stylesheet ........ Styles: Provides a widget style which is configurable via CSS."
#		OPTS+=" -no-syntaxhighlighter ....... Widgets: Supports custom syntax highlighting."
#		OPTS+=" -no-systemsemaphore ......... Kernel: Provides a general counting system semaphore."
#		OPTS+=" -no-systemtrayicon .......... Utilities: Provides an icon for an application in the system tray."
#		OPTS+=" -no-tabbar .................. Widgets: Provides tab bars, e.g., for use in tabbed dialogs."
#		OPTS+=" -no-tabletevent ............. Kernel: Supports tablet events."
#		OPTS+=" -no-tableview ............... ItemViews: Provides a default model/view implementation of a table view."
#		OPTS+=" -no-tablewidget ............. Widgets: Provides item-based table views."
#		OPTS+=" -no-tabwidget ............... Widgets: Supports stacking tabbed widgets."
#		OPTS+=" -no-temporaryfile ........... File I/O: Provides an I/O device that operates on temporary files."
#		OPTS+=" -no-textbrowser ............. Widgets: Supports HTML document browsing."
#		OPTS+=" -no-textcodec ............... Internationalization: Supports conversions between text encodings."
#		OPTS+=" -no-textdate ................ Data structures: Supports month and day names in dates."
#		OPTS+=" -no-textedit ................ Widgets: Supports rich text editing."
#		OPTS+=" -no-texthtmlparser .......... Kernel: Provides a parser for HTML."
#		OPTS+=" -no-textodfwriter ........... Kernel: Provides an ODF writer."
#		OPTS+=" -no-timezone ................ Utilities: Provides support for time-zone handling."
#		OPTS+=" -no-toolbar ................. Widgets: Provides movable panels containing a set of controls."
#		OPTS+=" -no-toolbox ................. Widgets: Provides columns of tabbed widget items."
#		OPTS+=" -no-toolbutton .............. Widgets: Provides quick-access buttons to commands and options."
#		OPTS+=" -no-tooltip ................. Widgets: Supports presentation of tooltips."
#		OPTS+=" -no-topleveldomain .......... Utilities: Provides support for extracting the top level domain from URLs."
#		OPTS+=" -no-translation ............. Internationalization: Supports translations using QObject::tr()."
#		OPTS+=" -no-treeview ................ ItemViews: Provides a default model/view implementation of a tree view."
#		OPTS+=" -no-treewidget .............. Widgets: Provides views using tree models."
#		OPTS+=" -no-tuiotouch ............... Provides the TuioTouch input plugin."
#		OPTS+=" -no-udpsocket ............... Networking: Provides access to UDP sockets."
#		OPTS+=" -no-undocommand ............. Utilities: Applies (redo or) undo of a single change in a document."
#		OPTS+=" -no-undogroup ............... Utilities: Provides the ability to cluster QUndoCommands."
#		OPTS+=" -no-undostack ............... Utilities: Provides the ability to (redo or) undo a list of changes in a document."
#		OPTS+=" -no-undoview ................ Utilities: Provides a widget which shows the contents of an undo stack."
#		OPTS+=" -no-validator ............... Widgets: Supports validation of input text."
#		OPTS+=" -no-whatsthis ............... Widget Support: Supports displaying "What's this" help."
#		OPTS+=" -no-wheelevent .............. Kernel: Supports wheel events."
#		OPTS+=" -no-widgettextcontrol ....... Widgets: Provides text control functionality to other widgets."
#		OPTS+=" -no-wizard .................. Dialogs: Provides a framework for multi-page click-through dialogs."
#		OPTS+=" -no-xml-schema .............. QtXmlPatterns: Provides XML schema validation."
#		OPTS+=" -no-xmlstream ............... Kernel: Provides a simple streaming API for XML."
#		OPTS+=" -no-xmlstreamreader ......... Kernel: Provides a well-formed XML parser with a simple streaming API."
#		OPTS+=" -no-xmlstreamwriter ......... Kernel: Provides a XML writer with a simple streaming API."




function do_prep(){
	export DEBIAN_FRONTEND=noninteractive
	$acmd update $aptgetops
	$acmd upgrade $aptgetops
	$acmd install $aptgetops git curl jq nmap
	$acmd install $aptgetops apt-transport-https ca-certificates curl gnupg2 software-properties-common 
	curl -fsSL https://download.docker.com/linux/debian/gpg | sudo apt-key add -
	add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/debian $(lsb_release -cs) stable"
	$acmd update $aptgetops
	apt-cache policy docker-ce
	$acmd install $aptgetops docker-ce
	systemctl status docker
}

function do_keys(){

	echo "SIZES -----------------------------"
	curl -X GET\
		-H "Content-Type: application/json"\
		-H "Authorization: Bearer $DIOC_TOKEN"\
		"https://api.digitalocean.com/v2/sizes"
	
	echo "SSH KEYS---------------------------"
	curl -X GET\
		-H "Content-Type: application/json"\
		-H "Authorization: Bearer $DIOC_TOKEN"\
		"https://api.digitalocean.com/v2/account/keys"
}

function dioc_api(){
	local mathod="$1"
	local path="$2"
	local body="$3"
	local P="https://api.digitalocean.com/v2$path"
	#>&2 echo "CURL $mathod '$P' '$body'"
	local res=$(curl -s -X "$mathod" \
		-H "Content-Type: application/json" \
		-H "Authorization: Bearer $DIOC_TOKEN" \
		-d "$body" \
		$P)
	echo "$res"
}

function wait_for_ssh(){
	local ip="$1"
	while true
	do
		#>&2 echo "WAITING FOR SSH $ip"
		sleep 1
		local ssh_port_status=$(nmap "$ip" -PN -p ssh | egrep 'open|closed|filtered' | awk '{print $2}')
		if [ "open" == "$ssh_port_status" ]
		then
			>&2 echo "SSH PORT OPEN"
			break
		fi
	done
}

function wait_for_droplet(){
	local droplet_id="$1"
	while true
	do
		#>&2 echo "WAITING FOR DROPLET $droplet_id"
		sleep 1
		local droplet=$(dioc_api "GET" "/droplets/$droplet_id")
		echo "$droplet" > "DROPLET_STATUS.json"
		local status=$(echo "$droplet" | jq -r '.droplet.status')
		local ip=$(echo "$droplet" | jq -r '.droplet.networks.v4[0].ip_address')
		#echo "DROPLET status=$status, ip=$ip";
		if [ "active" == "$status" ]
		then
			>&2 echo "DROPLET $droplet_id READY WITH IP: $ip"
			echo "$ip"
			break
		fi
	done
}


function dioc_provision(){
	local region="$1"
	local image="$2"
	local size="$3"

	>&2 echo "CREATING DROPLET region=$region, image=$image, size=$size"
	local body=$(cat <<EOT
{
	 "name":		"octomy-quick-build"
	,"region":		"$region"
	,"size":		"$size"
	,"image":		"$image"
	,"ssh_keys":	["14:8f:e8:cc:4c:f3:ef:d4:da:c9:c9:4f:67:65:57:eb"]
	,"backups":		false
	,"ipv6":		false
	,"user_data":	null
	,"private_networking":null
	,"volumes":		null
	,"tags":		["$ephemeral_tag"]
}

EOT
)
	#>&2 echo "BODY=$body"
	local res=$(dioc_api "POST" "/droplets" "$body")
	echo "$res" > "DROPLET.json"
	local droplet_id=$(echo "$res" | jq -r '.droplet.id')
	if [ "" == "$droplet_id" ]
	then
		>&2 echo "DROPLET CREATION FAILED"
		echo ""
		return
	fi
	>&2 echo "DROPLET $droplet_id CREATED"
	local ip=$(wait_for_droplet "$droplet_id")
	wait_for_ssh "$ip"	
	echo "$droplet_id" "$ip"
}

function dioc_delete(){
	local droplet_id="$1"
	dioc_api "DELETE" "/droplets/$droplet_id"
}


function dioc_delete_by_tag(){
	local tag="$1"
	dioc_api "DELETE" "/droplets?tag_name=$tag"
}

function ssh_cmd(){
	local host="$1"
	local cmd="$2"
	ssh -o "StrictHostKeyChecking no" -o "UserKnownHostsFile /dev/null"  "$host" "$cmd"
}

function do_provision(){
	local shutdowntime=55
	local region="fra1"
	local image="debian-9-x64"
	local size="c-32"
	# Give it 1 minute(s) to be gone
	local killtime=$((shutdowntime+1))
	read droplet_id ip < <(dioc_provision "$region" "$image" "$size")
	if [ "" == "$droplet_id" ] || [ "" == "$ip" ]
	then
		>&2 echo "Could not provision (droplet_id=$droplet_id, ip=$ip)"
		exit 1
	fi
	echo "PROVISIONED '$droplet_id' with IP $ip"
	# SAFETY: The droplet we just provisioned can be really expensive. So as a safe-guard we schedule its demise in 55 minutes no matter what happens:
	(echo "KILL TIMER SET FOR ${killtime} minutes"; sleep "${killtime}m"; echo "${killtime} minutes is up, TIME TO DIE!!1!"; dioc_delete "$droplet_id"; exit) &
	
	local cmd=$(cat <<EOT
(echo "SHUTDOWN TIMER SET FOR ${shutdowntime} minutes"; sleep "${shutdowntime}m"; echo "${shutdowntime} minutes is up, TIME TO SHUTDOWN!!1!"; shutdown -h now; exit) & \
echo 'APT::Install-Recommends "0" ; APT::Install-Suggests "0" ;' >> /etc/apt/apt.conf && \
export DEBIAN_FRONTEND=noninteractive && \
$acmd update $aptgetops && \
$acmd upgrade $aptgetops && \
$acmd install -f $aptgetops && \
$acmd install $aptgetops gnupg2 wget ca-certificates apt-transport-https curl software-properties-common git && \
$acmd autoremove $aptgetops && \
rm -f /usr/local/share/ca-certificates/certificate.crt; \
update-ca-certificates --fresh && \
$acmd update $aptgetops && \
$acmd upgrade $aptgetops && \
$acmd install -f $aptgetops && \
$acmd install $aptgetops gnupg2 wget ca-certificates apt-transport-https curl software-properties-common git && \
$acmd autoremove $aptgetops && \
$acmd clean $aptgetops && \
git clone https://github.com/mrdeveloperdude/OctoMY.git -j\$(nproc) --recurse-submodules && \
pushd OctoMY/integration/docker && \
./build.sh prep && \
./build.sh build && \
popd

EOT
)
	ssh_cmd "root@$ip" "$cmd"
}

function do_prune(){
	>&2 echo "PRUNING DROPLETS WITH TAG=$ephemeral_tag"
	dioc_delete_by_tag "$ephemeral_tag"
}

if [ ! "${1+defined}" ]
then

cat << EOT
   USAGE: $0 <commands>

      Where commands is a list of one or more of the following:

       -v <QT-VERSION> (default: ${qt_version})
       + build     - Build OctoMY
       + prep      - Prepare local environment for running build by instalkling necessary tools such as docker
       + provision - Run developers test. NOTE: ONly for developer, not for production use!
       + test      - Provision droplet at digital ocean
       + extract   - Extract artifacts from named image

EOT
	exit 1
	
fi

while [ "${1+defined}" ]
do
    case $1 in
        -v* )	shift; qt_version_long="$1"; qt_version="${qt_version_long%.*}" ;;
        -c* )	CACHE=" --no-cache" ;;
        build )		do_build ;;
        test)		do_test ;;
        prep)		do_prep ;;
        provision)	do_provision ;;
        prune)		do_prune ;;
        extract)    shift; tag="$1"; do_extract "$tag" ;;
        *) echo "UNKNWON COMMAND: '$1', SKIPPING..."    ;;
    esac
    shift
done


