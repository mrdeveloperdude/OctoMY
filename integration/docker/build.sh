#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi


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


EDEPS=""
BDEPS=""
DEPS=""
OPTS=""



function do_test(){
	echo "TROLOLOLOO"
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
	#   <host_os>/<host_os_version>/<target_os>/<target_os_version>/<qt_version>/<qt_linkade>/<checkout>
	#
	#   Where:
	#
	#   + <host_os> - The OS that hosts the build. Example: ubuntu. NOTE: the host OS is not necessarily the OS where the artifacts will run.
	#   + <host_os_arch> - The hardware architecture of the OS that hosts the build. Example: amd64.
	#   + <host_os_version> - The version of the host OS. Example: 18.04. 
	#   + <target_os> - The OS for which the artifacts are supposed to be built and run. Example: android
	#   + <target_os_arch> - The hardware architecture for which the artifacts are supposed to be built and run. Example: armv7.
	#   + <target_os_version> - The version of the target OS. Example: 28 (Android 9, API level 28)
	#   + <qt_version> - The version of Qt we want to use. NOTE: Qt will be built from source.
	#   + <qt_linkage> - One of "shared", "os", and "static" which determines how to link Qt. NOTE: os means simply use the prebuilt Qt available in OS (not always possible).
	#   + <checkout/branch> - The git checkout hash or branch name that we want to build
	#

	checkout="master"
	linkage="static"

	
	build_with_docker "debian-stretch-amd64-debian-stretch-amd64-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-stretch-amd64-debian-stretch-x86-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-stretch-amd64-debian-jessie-amd64-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-stretch-amd64-debian-jessie-x86-$qt_version-$linkage-$checkout"#
#	build_with_docker "debian-stretch-amd64-ubuntu-16.04-amd64-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-stretch-amd64-ubuntu-16.04-x86-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-stretch-amd64-ubuntu-18.04-amd64-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-stretch-amd64-ubuntu-18.04-x86-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-stretch-amd64-android-4-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-stretch-amd64-android-5-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-stretch-amd64-android-6-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-stretch-amd64-android-7-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-stretch-amd64-android-8-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "debian-stretch-amd64-android-9-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "windows-10-amd64-windows-10-amd64-$qt_version-$linkage-$checkout"
#	build_with_docker "windows-10-amd64-windows-10-x86-$qt_version-$linkage-$checkout"
#	build_with_docker "osx-10.5-amd64-osx-10.5-amd64-$qt_version-$linkage-$checkout"
#	build_with_docker "osx-10.5-amd64-osx-10.5-x86-$qt_version-$linkage-$checkout"
#	build_with_docker "osx-10.5-amd64-ios-10-arm7-$qt_version-$linkage-$checkout"
#	build_with_docker "osx-10.5-amd64-ios-11-arm7-$qt_version-$linkage-$checkout"
	
}


function build_with_docker(){
	local spec="$1"
	IFS='-' read -r host_os host_os_version host_os_arch target_os target_os_version target_os_arch qt_version qt_linkage checkout <<<"$spec"
	local dir="builds/$host_os/$host_os_version/$host_os_arch/$target_os/$target_os_version/$target_os_arch/$qt_version/$qt_linkage/$checkout"
	local doc="$pw/$dir/Dockerfile"
	local src_overrides="/src/octomy_$checkout/pri/overrides/release.pri"
	local dst_overrides="/src/octomy_$checkout/pri/local_overrides.pri"
	local octomy_git_ver="octomy_version.txt"
	
	mkdir -p "$dir"
	pushd "$dir"
	
	local sources="/etc/apt/sources.list"
	local temp_sources="/etc/apt/sources.list_tmp"

	
	if [ "debian" == "$host_os" ]
	then
		
		do_debian_deps
		do_common_opts

	elif [ "ubuntu" == "$host_os" ]
	then
		do_ubuntu_deps
		echo "Host OS \'$host_os\' not supported yet"
		exit 1
		
	elif [ "windows" == "$host_os" ]
	then
		echo "Host OS \'$host_os\' not supported yet"
		exit 1
	else
		echo "Host OS \'$host_os\' not recognized, maybe check your spelling?"
		exit 1

	fi

	git describe --tags --long --dirty --always > "$octomy_git_ver"
	echo "OCTOMY GIT VERSION #############################"
	pwd
	ls -halt "$octomy_git_ver"
	cat "$octomy_git_ver"
	echo "OCTOMY GIT VERSION #############################"

	
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


	if [ "debian" == "$host_os" ]
	then
		cat << EOF >> "$doc"

RUN echo "---- FIX SHELL ------------------------"
RUN ["/bin/bash", "-c", "unlink /bin/sh"]
RUN ["/bin/bash", "-c", "ln -s /bin/bash /bin/sh"]
RUN echo "CORES AVAILABLE: \$(nproc)"

RUN echo "---- ADD APT SRC ----------------------" && \
	sed "s/^deb/deb-src/g" ${sources} > ${temp_sources} && \
	cat ${temp_sources} >> ${sources}

# From https://wiki.debian.org/ReduceDebian
RUN echo "---- INITIALIZE APT -------------------" && \
	echo 'APT::Install-Recommends "0" ; APT::Install-Suggests "0" ;' >> /etc/apt/apt.conf && \
	apt-get update && \
	apt-get upgrade -y && \
	apt-get autoremove -y && \
	apt-get clean -y

RUN	echo "---- INSTALL EARLY DEPENDENCIES -------" && \
	apt-get install -y --allow-unauthenticated $EDEPS

RUN	echo "---- FIX SSL CERTIFICATES -------------" && \
	rm -f /usr/local/share/ca-certificates/certificate.crt; \
	update-ca-certificates --fresh

WORKDIR /src

RUN echo "---- CLONE QT -------------------------" && \
	git clone https://github.com/qt/qt5.git -j\$(nproc) --recurse-submodules -b $qt_version /src/qt_$qt_version

RUN	echo "---- CLONE OCTOMY ---------------------" && \
	git clone https://github.com/mrdeveloperdude/OctoMY.git -j\$(nproc) --recurse-submodules -b "$checkout" /src/octomy_$checkout

RUN echo "---- GET QT DEPENDENCIES --------------" && \
	apt-get build-dep -y --allow-unauthenticated qt5-default && \
	apt-get install -y --allow-unauthenticated $BDEPS $DEPS

RUN echo "---- CLEAN APT ------------------------" && \
	apt-get autoremove -y && apt-get clean -y && \
	rm -rf /var/lib/apt/lists/* /usr/share/man

EOF
	else
		echo "$host_os not supported at this time."
		exit 1
	fi

	cat << EOF >> "$doc"


WORKDIR /qt_$qt_version

RUN echo "---- HELP QT --------------------------" && \
	/src/qt_$qt_version/configure --help

RUN echo "---- CONFIGURE QT ---------------------" && \
	/src/qt_$qt_version/configure $OPTS

RUN echo "---- INSPECT CONFIGURATION OF QT ------" && \
	ls -halt

RUN echo "---- BUILD QT -------------------------" && \
	MAKEFLAGS=-j\$(nproc) make -k -j \$(nproc)

RUN echo "---- INSTALL QT -----------------------" && \
	mkdir -p "$qt_install_dir"; \
	MAKEFLAGS=-j\$(nproc) make -k -j \$(nproc) install

RUN echo "---- SHOW QMAKE VERSION ---------------" && \
	"$qt_qmake" --version

WORKDIR /src/octomy_$checkout

# This step si a cache buster that forces rebuild on changes in git
ADD "$octomy_git_ver" "git_version"

RUN	echo "---- UPDATE OCTOMY --------------------" && \
		git checkout "$checkout" && \
		git pull
	
WORKDIR /qmake_test

RUN	echo "---- BUILD QMAKE TEST -----------------" && \
	ls -halt /src/octomy_$checkout/integration/docker/ && \
	MAKEFLAGS=-j\$(nproc) "$qt_qmake" /src/octomy_$checkout/integration/docker/qmake_test/QmakeTest.pro && \
	MAKEFLAGS=-j\$(nproc) make -j \$(nproc); \

WORKDIR /OctoMY

RUN	echo "---- BUILD OCTOMY ---------------------" && \
	cp -a "$src_overrides" "$dst_overrides" && \
	cat "$dst_overrides" && \
	MAKEFLAGS=-j\$(nproc) "$qt_qmake" /src/octomy_$checkout/OctoMY.pro && \
	MAKEFLAGS=-j\$(nproc) make -j \$(nproc)

# 	cd /src/qt_$qt_version && \
#EXPOSE 8080:80
#CMD ["/usr/sbin/apache2", "-D",  "FOREGROUND"]

EOF

	local cmd="docker build . -t octomy:${spec}"
	docker build --help
	echo "Executing: $cmd --------- "
	$cmd
	popd
}


function do_ubuntu_deps(){
	#		DEPS+=" libsslcommon2-dev" #Not in Debian
	echo "NO"
}


function do_common_deps(){
	
	echo	-confirm-license \
			-dbus-runtime \
			-no-compile-examples \
			-no-cups \
			-no-gif \
			-nomake examples \
			-nomake tests \
			-nomake tools \
			-no-qml-debug \
			-no-sql-db2 \
			-no-sql-ibase \
			-no-sql-mysql \
			-no-sql-oci \
			-no-sql-odbc \
			-no-sql-psql \
			-no-sql-sqlite2 \
			-no-sql-tds \
			-no-warnings-are-errors \
			-no-xcb -prefix /usr/local/Qt \
			-opensource \
			-skip qtandroidextras \
			-skip qtcanvas3d \
			-skip qtdeclarative \
			-skip qtenginio \
			-skip qtmacextras \
			-skip qtquickcontrols \
			-skip qtquickcontrols2 \
			-skip qtwebchannel \
			-skip qtwebengine \
			-skip qtwebsockets \
			-skip qtwebview \
			-skip qtwinextras \
			-skip qtx11extras \
			-verbose  >> /dev/null

		# INSTALL LOCATION
		OPTS+=" -prefix \"$qt_install_dir\""
		# VERBOSITY
		OPTS+=" -silent"
#		OPTS+=" -verbose"

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
		
		# OPTIONAL PARTS
		OPTS+=" -make libs"
		OPTS+=" -nomake examples"
		OPTS+=" -nomake tests"
		OPTS+=" -nomake tools"


		# OPTIONAL FEATURES
		OPTS+=" -dbus-runtime" # runtime is important because dbus test fails for linked
		OPTS+=" -alsa"
		OPTS+=" -glib"
		OPTS+=" -gstreamer"
		OPTS+=" -gui"
		OPTS+=" -inotify"
		OPTS+=" -eventfd"

#		OPTS+=" -no-nis"
		OPTS+=" -no-pulseaudio"
#		OPTS+=" -no-qml-debug"
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
		OPTS+=" -no-cups" 
		OPTS+=" -no-directfb"
		OPTS+=" -no-eglfs"
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
		#OPTS+=" -no-sql-sqlite"  # we want sqlite3 support
		OPTS+=" -no-sql-sqlite2" 
		OPTS+=" -no-sql-tds"
		OPTS+=" -no-gif"
		OPTS+=" -no-cups" 
		OPTS+=" -no-iconv"
		OPTS+=" -no-dbus"
		OPTS+=" -no-eglfs"
		OPTS+=" -no-directfb" 
		OPTS+=" -no-linuxfb"
		OPTS+=" -no-glib"
		OPTS+=" -no-kms"
		OPTS+=" -nomake examples" 
		#OPTS+=" -nomake demos" NOT AVAILABLE ANYMORE
		#OPTS+=" -no-openssl"

		# OPTIONAL MODULES AND COMPONENTS
		OPTS+=" -skip qtandroidextras"
		OPTS+=" -skip qtcanvas3d"
		OPTS+=" -skip qtdeclarative"
		OPTS+=" -skip qtenginio"
#		OPTS+=" -skip qtgraphicaleffects"
		OPTS+=" -skip qtmacextras"
		OPTS+=" -skip qtquickcontrols"
		OPTS+=" -skip qtquickcontrols2"
		OPTS+=" -skip qtwayland"
		OPTS+=" -skip qtwebchannel"
		OPTS+=" -skip qtwebengine"
		OPTS+=" -skip qtwebsockets"
		OPTS+=" -skip qtwebview"
		OPTS+=" -skip qtwayland"
		OPTS+=" -skip qtwinextras"
#		OPTS+=" -skip qtx11extras"
 
}

function do_debian_deps(){

	
		EDEPS+=" gnupg2"
		EDEPS+=" apt-utils"
		EDEPS+=" wget"
		EDEPS+=" ca-certificates"
		EDEPS+=" apt-transport-https"
		EDEPS+=" curl"
		EDEPS+=" software-properties-common"
		EDEPS+=" git"

		BDEPS+=" perl"
		BDEPS+=" python"
		BDEPS+=" build-essential"
		BDEPS+=" clang"
		

		# Window system
		DEPS+=" ^libxcb.*-dev"
		DEPS+=" libxcb-dri3-dev"
		DEPS+=" libxcb-present-dev"
		DEPS+=" libxcb-sync-dev"
		DEPS+=" libx11-xcb-dev"
		DEPS+=" libglu1-mesa-dev"
		DEPS+=" libxrender-dev"
		DEPS+=" libxi-dev"
#		DEPS+=" libX11-dev"
#		DEPS+=" libX11-xcb-dev"
#		DEPS+=" libXau-dev"
#		DEPS+=" libXcomposite-dev"
#		DEPS+=" libXcursor-dev"
#		DEPS+=" libXdamage-dev"
#		DEPS+=" libXdmcp-dev"
#		DEPS+=" libXext-dev"
#		DEPS+=" libXi-dev"
#		DEPS+=" libXrender-dev"
#		DEPS+=" libXtst-dev"
#		DEPS+=" libXxf86vm-dev"



		# GLX
		DEPS+=" libgl1-mesa-dev"
#		DEPS+=" glx-alternative-mesa"
		DEPS+=" libgl1-mesa-dev"
		DEPS+=" libgl1-mesa-dev"
		DEPS+=" libgles2-mesa-dev"
		DEPS+=" mesa-common-dev"
		DEPS+=" libgl1-mesa-dev"
		DEPS+=" libegl1-mesa-dev"

		
		DEPS+=" libasyncns-dev"
		DEPS+=" libc-dev"
		DEPS+=" libdbus-1-dev"
		DEPS+=" libdrm-dev"
		DEPS+=" libexpat-dev"
		
		
		DEPS+=" libclang-dev"
		
		DEPS+=" libsqlite-dev"
		DEPS+=" libsqlite3-dev"
		DEPS+=" libssl-dev"
		DEPS+=" gnutls-dev"


		DEPS+=" libfontconfig-dev"
		DEPS+=" libgbm-dev"
#		DEPS+=" libICE-dev"
		DEPS+=" libjson-c-dev"
		DEPS+=" liblzma-dev"
		DEPS+=" libnspr4-dev"
		DEPS+=" libnss3-dev"
				
		# For qtmultimedia
		DEPS+=" libasound2-dev"
		DEPS+=" libgstreamer1.0-dev"
		DEPS+=" libgstreamer-plugins-base1.0-dev"
		DEPS+=" libffi-dev"
#		DEPS+=" libFLAC-dev"
		DEPS+=" libflac-dev"
		DEPS+=" libogg-dev"
		DEPS+=" libpng-dev"
		DEPS+=" libpulse-dev"
#		DEPS+=" libSM-dev"
		DEPS+=" libsndfile-dev"
		DEPS+=" libvorbis-dev"
		DEPS+=" libxml2-dev"
		
		#NOTE: one of these might provide "gst/interfaces/photography.h
		DEPS+=" libgstreamer-plugins-bad1.0-dev"	# GStreamer development files for libraries from the "ba
		DEPS+=" libgstreamer-plugins-base1.0-dev"	# GStreamer development files for libraries from the "ba
#		DEPS+=" libgstreamer-plugins-good1.0-dev"	# GStreamer development files for libraries from the "go (UBUNTU)
		
		DEPS+=" libgstreamer1.0-dev"
#		DEPS+=" libgstbuzztard-dev"					# Buzztard - Support plugins for GStreamer (UBUNTU)
#		DEPS+=" libgstreamer1.0-vaapi-dev"			# VA-API stuff for Gstreamer (interface for hardware acceleration of media encoding/decoding)

		DEPS+=" libxshmfence-dev"
		DEPS+=" libxslt-dev"
		DEPS+=" libz-dev"
		
		# Module spesific for qt3d
		DEPS+=" libassimp-dev"						# Asset management (loading/saving of 3D content) library
		
		# Module spesific for qtconnectivity (bluetooth++)
#		DEPS+=" libbluez-dev"						# Bluetooth support (UBUNTU)
		DEPS+=" libbluetooth-dev"						# Bluetooth support (DEBIAN)
		
		# Module spesific for qtimageformats 
#		DEPS+=" libjasper-dev"						# Jpeg2000 image support
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




function do_ubuntu_deps(){

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

}





function do_prep(){
	apt update
	apt upgrade
	apt install git
	
	apt install apt-transport-https ca-certificates curl gnupg2 software-properties-common
	curl -fsSL https://download.docker.com/linux/debian/gpg | sudo apt-key add -
	add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/debian $(lsb_release -cs) stable"
	apt update
	apt-cache policy docker-ce
	apt install docker-ce
	systemctl status docker

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
        -v* )	shift; qt_version_long="$"; qt_version="${qt_version_long%.*}" ;;
        -c* )	CACHE=" --no-cache" ;;
        build )		do_build ;;
        test)		do_test ;;
        prep)		do_prep ;;
        *) echo "UNKNWON COMMAND: '$1', SKIPPING..."    ;;
    esac
    shift
done


