#!/bin/bash

if [ ! -d build ]; then
    mkdir build
    (cd build && cmake ..)
fi

echo "Build binary ..."
cd build
make -j$(nproc)

cd ../Build_Release

echo "Recreate ASSETS.ZIP ..."
rm -f ASSETS.ZIP
(cd ASSETS && zip -r ../ASSETS.ZIP .)

echo "Run ..."
cp openclaw OpenClaw
./OpenClaw
