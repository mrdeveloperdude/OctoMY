#!/bin/bash

qmake="/home/lennart/Qt/5.8/gcc_64/bin/qmake"
overrides_src="pri/travis.pri"
overrides_dst="pri/local_overrides.pri"
project_file="OctoMY.pro"
build_dir="build"
cp -a "$overrides_src" "$overrides_dst"
cat "$overrides_dst"
mkdir -p "$build_dir"
pushd "$build_dir"
"$qmake" "../$project_file"
make -j9
make check
popd


