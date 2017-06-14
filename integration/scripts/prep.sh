

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
if [ "USE_OLD" == "USE_OLD" ]
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
	DEPS+=" libsqlite-dev"
	DEPS+=" libsqlite3-dev"
	# SSL/TLS Support
	DEPS+=" libssl-dev"
	DEPS+=" gnutls-dev"
	DEPS+=" libgnutls-dev"
#	DEPS+=" libgnutls-openssl-dev" # Not available on debian
	DEPS+=" libsslcommon2-dev"
	# OpenCV support (for bottle rig)
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
	sudo DEBIAN_FRONTEND=noninteractive dpkg --configure -a
	sudo DEBIAN_FRONTEND=noninteractive apt-get install -y --force-yes --fix-missing
	sudo DEBIAN_FRONTEND=noninteractive apt-get update -y --force-yes 
	sudo DEBIAN_FRONTEND=noninteractive apt-get upgrade -y --force-yes --fix-missing
	sudo DEBIAN_FRONTEND=noninteractive apt-get install $DEPS -y --force-yes --fix-missing
}



#
do_prep

