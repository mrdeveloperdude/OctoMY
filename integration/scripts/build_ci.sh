#!/bin/bash

qmk="/home/lennart/Qt/5.8/gcc_64/bin/qmake"
overrides_src="integrations/travis_ci/overrides.pri"
overrides_dst="pri/local_overrides.pri"
pro="OctoMY.pro"
bdir="build"
cp -a "$overrides_src" "$overrides_dst"
cat "$overrides_dst"
mkdir -p "$bdir"
pushd "$bdir"
"$qmk" "../$pro"
make -j9
make check
popd


