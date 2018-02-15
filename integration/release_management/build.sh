#!/bin/bash

TARGET_PLATFORMS=""
TARGET_PLATFORMS+="Dockerfile.qt5.10.0_static_ubuntu_amd64"
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
	docker build -f "$OCTOMY" --build-arg "oc_base=$qt_target" -t "$oc_target" $cache .
	if [ ! $? -eq 0 ]; then
		echo "Fail Octomy build"
		exit
	fi

	echo "----------------------------------- DEB: '$deb_target'"
	docker build -f "$DEB"    --build-arg "deb_base=$oc_target" -t "$deb_target" --no-cache .
	if [ ! $? -eq 0 ]; then
		echo "Fail deb build"
		exit
	fi

	echo "----------------------------------- DEBTEST BUILD: '$debtest_target'"
	docker build -f "$DEBTEST"    --build-arg "debtest_base=$deb_target" -t "$debtest_target" .
	if [ ! $? -eq 0 ]; then
		echo "Fail debtest build"
		exit
	fi

	echo "----------------------------------- DEBTEST RUN: '$debtest_target'"
	docker run -p 5900 "$debtest_target" x11vnc -forever -nopw -ncache_cr -ncache 10 -create -display :0
	if [ ! $? -eq 0 ]; then
		echo "Fail debtest start"
		exit
	fi

	#cont=$(docker create "$deb_target")
	#docker cp "$cont:/root/deb/agent.deb" .
	#docker rm "$cont"
done

