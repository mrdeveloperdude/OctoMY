Release management
------------------

This is the OctoMY™ release management.

How does it work?

It starts with the [build.sh](build.sh) script which orchestrates several builds using a layered system of Dockerfiles.

The layers are as follows:

1. [Dockerfile.qt5.10.0_static_ubuntu_amd64](Dockerfile.qt5.10.0_static_ubuntu_amd64): Qt5 static build
2. [Dockerfile.octomy](Dockerfile.octomy): OctoMY™ build from source
2. [Dockerfile.deb_test](Dockerfile.deb_test): OctoMY™ run tests in virtual desktop environment (xvnc)
2. [Dockerfile.deb_file](Dockerfile.deb_file): OctoMY™ package into OS packages.

Please note that the current version only support Ubuntu 16.04LTS but will extend to include more debian derived OS versions and eventually rpm/packman/other package systems.

For building on other platforms like Android/Windows/OSX/iOS other methods are in the sketch stage.
