#!/bin/bash

sudo apt -y install apt-transport-https ca-certificates curl
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
service lxcfs stop
sudo apt remove -y -q lxc-common lxcfs lxd lxd-client
sudo apt update -q
sudo apt -y install docker-ce
