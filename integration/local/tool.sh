#!/bin/bash

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root"
	exit 1
fi


if [ $# -lt 1 ]
then
	echo "USAGE: $0 <args>"
	echo "where args is any combination of the following:"
	echo "-v VER   :   Set the Qt version to VER on the form 5.9.0"
	echo "debug    :   Select debug build"
	echo "prep     :   Run package manager to install all dependencies (for recent Ubuntu/Debian based distros only)"
	echo "debug    :   Select debug build"
	echo "rebuild  :   Rebuild executables for test-rig"
	echo "exe      :   Distribute last set of executables to local test rig"
	echo "bg       :   Distribute wallpaper images to local test rig"
	echo "hubsyn   :   Send HUP signal to synegy client in local test-rig"
	echo "reboot   :   Reboot nodes of local test-rig"
	echo "shutdown :   Shutdown nodes of local test-rig"
	echo "cssh     :   Access nodes of local test-rig using cssh"
	exit 1
fi




dir=$(readlink -f "$(pwd)/${0%/*}/../..")

echo "USING PROJECT DIR: $dir"



jail="/home/lennart/debian_jail_jessie"
base="/root/OctoMY"
qmake="/root/waterwell-dispenser-client/image/qt/build/5.9.0/release/qtbase/bin/qmake"
build_dir="$jail$base/build"
pro="/OctoMY.pro"
user="octomy"


# Release mode
REL="release"

# Change this variable to suit your need
VER="5.9.0"



do_prep(){
	# Dependencies:
	local DEPS=""
	# Build tools
	DEPS+=" build-essential"
	DEPS+=" perl"
	DEPS+=" python"
	DEPS+=" git"
	# XCB/X11 support
	DEPS+=" ^libxcb.*"
	DEPS+=" libx11-xcb-dev"
	DEPS+=" libglu1-mesa-dev"
	DEPS+=" libxrender-dev"
	# Sound support
	DEPS+=" libasound2-dev"
	 # G-Streamer support
if [ "USE_OLD" == "" ]
then
		DEPS+=" libgstreamer0.10-dev"
		DEPS+=" libgstreamer-plugins-base0.10-dev"
		DEPS+=" libgstreamer-plugins-bad0.10-dev"
		DEPS+=" libgstreamer0.10-dev"
else
		DEPS+=" libgstreamer1.0-dev"
		DEPS+=" libgstreamer-plugins-base1.0-dev"
		DEPS+=" gstreamer1.0-plugins-good"
fi
	# XKB fixes for Qt5.4x
	DEPS+=" libxkbfile1"
	DEPS+=" x11-xkb-utils"
	DEPS+=" xkb-data"
	DEPS+=" libxkbfile-dev"
	# SQLite 3 support
	DEPS+=" libsqlite3-dev"
	# SSL/TLS Support
	DEPS+=" gnutls-dev"
#	DEPS+=" libgnutls-dev"
#	DEPS+=" libgnutls-openssl-dev" # Not available on debian
#	DEPS+=" libsslcommon2-dev"
	# E-Speack suppoart
	DEPS+=" libespeak-ng-dev"
	# OpenCV support
if [ "USE_OPENCV" == "" ]
then
	DEPS+=" libopencv-calib3d-dev"
	DEPS+=" libopencv-contrib-dev"
	DEPS+=" libopencv-core-dev"
	DEPS+=" libopencv-dev"
	DEPS+=" libopencv-features2d-dev"
	DEPS+=" libopencv-flann-dev"
	DEPS+=" libopencv-gpu-dev"
	DEPS+=" libopencv-highgui-dev"
	DEPS+=" libopencv-imgproc-dev"
	DEPS+=" libopencv-legacy-dev"
	DEPS+=" libopencv-ml-dev"
	DEPS+=" libopencv-objdetect-dev"
	DEPS+=" libopencv-video-dev"
fi

if [ "USE_OPENCL" == "" ]
then
	DEPS+=" opencl-headers"
fi
	DEPS+=" flex"
	DEPS+=" libpulse-dev"

	sudo DEBIAN_FRONTEND=noninteractive dpkg --configure -a
	sudo DEBIAN_FRONTEND=noninteractive apt-get install --yes --fix-missing
	sudo DEBIAN_FRONTEND=noninteractive apt-get update --yes
	sudo DEBIAN_FRONTEND=noninteractive apt-get upgrade --yes --fix-missing
	sudo DEBIAN_FRONTEND=noninteractive apt-get install $DEPS --yes --fix-missing
}


function send_one_exe(){
	local tag="$1"
	local target="$2"
	local src="$build_dir/src/$tag/$tag"
	mtime_fn="mtime_$tag.txt"
	mtime=$(stat -c %Y "$src")
	mtime_old=$(cat "$mtime_fn");
	if [ "$mtime" != "$mtime_old" ]
	then
		echo "mtime updated to $mtime for $tag"
		echo "$mtime" > "$mtime_fn";

		ssh "$target" "[ -e ~/$tag ] && unlink ~/$tag"
		scp "$src" "$target:~/$tag"
		ssh "$target" "pgrep $tag && killall $tag"
	fi
}



function send_one_bg(){
	local tag="$1"
	local target="$2"
	local img="${tag}_1024x768.png"
	local src="${dir}/design/wallpapers/1024x768/${img}"
	ssh "$target" "mkdir -p ~/wallpapers"
	ssh "$target" "[ -L ~/wallpapers/bg.png ] && unlink ~/wallpapers/bg.png"
	scp "$src" "$target:~/wallpapers/$img"
	ssh "$target" "ln -s ~/wallpapers/${img} ~/wallpapers/bg.png"

}



function hub_syn_one(){
	local tag="$1"
	local target="$2"
	ssh "$target" "killall -HUP synergyc"
}


function do_rebuild_jail(){
	echo "REBUILDING IN JAIL"
	if false
	then
	rm -rf "$build_dir"
	mkdir -p "$build_dir"
	cd "$jail$base"
	git pull
	chroot "$jail" /bin/bash -c "cd $base/build && $qmake $base$pro && make -j $(nproc)"
	fi
}

function do_send_exe(){
	echo "SENDING EXECUTABLES TO HW"
	send_one_exe "remote"	"$user@10.0.0.152"
	send_one_exe "agent"	"$user@10.0.0.82"
	send_one_exe "hub"		"$user@10.0.0.81"
}

function do_send_bg(){
	echo "SENDING WALLPAPERS TO HW"
	send_one_bg "remote"	"$user@10.0.0.152"
	send_one_bg "agent"		"$user@10.0.0.82"
	send_one_bg "hub"		"$user@10.0.0.81"
}



function reboot_one(){
	local target="$1"
	ssh "$target" "sudo reboot"
}


function do_reboot(){
	echo "REBOOTING CLIENTS"
	reboot_one "$user@10.0.0.152"
	reboot_one "$user@10.0.0.82"
	reboot_one "$user@10.0.0.81"
}


function shutdown_one(){
	local target="$1"
	ssh "$target" "sudo shutdown -h now"
}


function do_reboot(){
	echo "SHUTTING DOWN CLIENTS"
	shutdown_one "$user@10.0.0.152"
	shutdown_one "$user@10.0.0.82"
	shutdown_one "$user@10.0.0.81"
}


function do_hup_syn(){
	echo "HUP-ING SYNERGY CLIENTS"

	hub_syn_one "remote"	"$user@10.0.0.152"
	hub_syn_one "agent"		"$user@10.0.0.82"
	hub_syn_one "hub"		"$user@10.0.0.81"
}



function do_cssh(){
	cssh  "$user@10.0.0.152"  "$user@10.0.0.82" "$user@10.0.0.81" -K 1
}


while [ "${1+defined}" ];
do
	case $1 in
		-v* ) shift; OLD=$VER; VER="$1"; echo "VERSION CHANGED FROM $OLD --> $VER"; ;;
		debug*)		OLD=$REL; REL="debug"; echo "BUILD CHANGED FROM $OLD --> $REL"; ;;
		release*)	OLD=$REL; REL="release"; echo "BUILD CHANGED FROM $OLD --> $REL"; ;;
		prep* )		do_prep ;;
		rebuild* )	do_rebuild ;;
		exe* )		do_send_exe ;;
		bg* )		do_send_bg ;;
		hubsyn* )	do_hup_syn ;;
		reboot* )	do_reboot ;;
		shutdown* )	do_shutdown ;;
		cssh* )		do_cssh ;;
		*) echo "UNKNWON COMMAND: '$1', SKIPPING..."	;;
	esac
	shift
done


echo "DONE"

