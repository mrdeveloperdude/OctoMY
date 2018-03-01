#!/bin/bash

docker_prep_ok_file="DOCKER_PREP_OK"

function apt_install(){
	local deps="$@"
	# Repair prior bad state
	DEBIAN_FRONTEND=noninteractive dpkg --configure -a
	DEBIAN_FRONTEND=noninteractive apt-get install -y --fix-missing
	# Update & upgrade 
	DEBIAN_FRONTEND=noninteractive apt-get update -y
	DEBIAN_FRONTEND=noninteractive apt-get upgrade -y --fix-missing
	# Actually install desired packages
	DEBIAN_FRONTEND=noninteractive apt-get install $deps -y --no-install-recommends --fix-missing
}



function apt_remove(){
	local deps="$@"
	# Repair prior bad state
	DEBIAN_FRONTEND=noninteractive dpkg --configure -a
	DEBIAN_FRONTEND=noninteractive apt-get install -y --fix-missing
	# Update & upgrade 
	DEBIAN_FRONTEND=noninteractive apt-get update -y
	DEBIAN_FRONTEND=noninteractive apt-get upgrade -y --fix-missing
	# Actually remove desired packages
	DEBIAN_FRONTEND=noninteractive apt-get remove $deps -y --no-install-recommends --fix-missing
	# Autoremove orphaned packages
	DEBIAN_FRONTEND=noninteractive apt-get autoremove -y
}


function ensure_prep(){
	# Check if we already prepped 
	if [ ! -e "$docker_prep_ok_file" ]
	then
		echo "Performing prep!"
		do_prep $@
	else
		echo "Prep already done, skipping..."
		return
	fi
}

function do_prep(){

	if [[ $EUID -ne 0 ]]; then
		echo "The prep command must be run as root" 
		exit 1
	fi
	# Dependencies:
	apt_install "git" "build-essential" "apt-transport-https" "ca-certificates" "curl" "software-properties-common"
	
	# From https://docs.docker.com/install/linux/docker-ce/ubuntu/#install-docker-ce
	
	# Remove old docker
	service docker stop
	apt_remove docker docker-engine docker.io
	rm -rf "/var/lib/docker"
	    
	# Add docker GPG key
	curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
	
	# Verify fingerprint
	if apt-key fingerprint 0EBFCD88 | grep -q "9DC8 5822 9FC7 DD38 854A E2D8 8D81 803C 0EBF CD88";
	then
		echo "matched"
	fi
	
	# Set up stable repo for docker
	add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
	
	# 6. Install specific version of docker-ce (to list version use apt-cache madison docker-ce)
	apt_install "docker-ce=17.12.1~ce-0~ubuntu"
	
	# 7. Test docker install
	service docker start
	sleep 3
	docker build hello-world
	docker run hello-world

	date > "$docker_prep_ok_file"
	echo "Setting prep done:"
	cat "$docker_prep_ok_file"
}


function do_build(){
	ensure_prep
	TARGET_PLATFORMS=""
	TARGET_PLATFORMS+="Dockerfile.qt5.10.0_static_ubuntu_amd64"

	TARGET_PACKAGES=""
	TARGET_PACKAGES+=" agent"
	TARGET_PACKAGES+=" remote"
	TARGET_PACKAGES+=" hub"
	TARGET_PACKAGES+=" zoo"


	OCTOMY="Dockerfile.octomy"
	DEB="Dockerfile.deb_file"
	DEBTEST="Dockerfile.deb_test"

	cache=""
	if [ ! -z $1 ]
	then
		cache="--no-cache"
	fi


	cp ../deb/make_deb.sh .

	for df in $TARGET_PLATFORMS
	do
		qt_target=$(echo "$df" | cut -c 12-)
		oc_target="${qt_target}.octomy"
		deb_target="${oc_target}.deb"
		debtest_target="${deb_target}.test"
		echo "    Qt target: $qt_target"
		echo "OctoMY target: $oc_target"
		echo "   Deb target: $deb_target"
		echo "------------------------------------ QT: '$qt_target'"
		docker build -f "$df"     -t "$qt_target"  .
		if [ ! $? -eq 0 ]; then
			echo "Fail Qt build"
			exit
		fi
		echo "-------------------------------- OCTOMY: '$oc_target'"
		docker build -f "$OCTOMY"	--build-arg "oc_base=$qt_target"		-t "$oc_target" $cache .
		if [ ! $? -eq 0 ]; then
			echo "Fail Octomy build"
			exit
		fi

		echo "----------------------------------- DEB: '$deb_target'"
		docker build -f "$DEB"		--build-arg "deb_base=$oc_target"		-t "$deb_target" .
		if [ ! $? -eq 0 ]; then
			echo "Fail deb build"
			exit
		fi

		echo "----------------------------------- DEBTEST BUILD: '$debtest_target'"
		docker build -f "$DEBTEST"	--build-arg "debtest_base=$deb_target"	-t "$debtest_target" .
		if [ ! $? -eq 0 ]; then
			echo "Fail debtest build"
			exit
		fi



		echo "----------------------------------- DEBTEST RUN: '$debtest_target'"
		#docker run -p 5900 "$debtest_target" x11vnc -forever -nopw -ncache_cr -ncache 10 -display :99 -auth guess
		docker run -p 5900:5900 "$debtest_target" -geometry 1024x768
		if [ ! $? -eq 0 ]; then
			echo "Fail debtest start"
			exit
		fi
	done
}



while [ "${1+defined}" ];
do
	case $1 in
		prep* )		do_prep ;;
		build* )	do_build ;;
		*) echo "UNKNWON COMMAND: '$1', SKIPPING..."	;;
	esac
	shift
done


echo "DONE"

