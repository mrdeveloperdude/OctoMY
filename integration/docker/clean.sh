#!/bin/bash
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

function do_dirs(){
	rm -rf octomy_*
}


function do_containers(){
	docker rm $(docker ps -a -q)
}

function do_images(){
	docker rmi $(docker images -a -q)
}

function do_all(){
	do_dirs
	do_containers
	do_images
}

if [ ! "${1+defined}" ]
then
	echo ""
	echo "   USAGE: $0 <commands>"
	echo ""
	echo "      Where commands is a list of one or more of the following:"
	echo ""
	echo "       + dirs        - clean files such generate Docker files, downloaded qt sources and debootstrap files"
	echo "       + containers  - clean dockr containers"
	echo "       + images      - clean docker images. NOTE: clean containrs first"
	echo "       + all         - clean all of the above in one fell swoop"
	echo ""
	exit 1
fi



while [ "${1+defined}" ]
do
    case $1 in
        dirs* )        do_dirs ;;
        containers* )  do_containers ;;
        images* )      do_images ;;
        all* )         do_all ;;
        *) echo "UNKNWON COMMAND: '$1', SKIPPING..."    ;;
    esac
    shift
done
