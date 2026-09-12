#!/bin/bash
#
# Build & run OpenClaw on macOS (Apple Silicon / Intel).
#
# Handles the two macOS-specific gotchas the default build misses:
#   1. Modern CMake (>=4) refuses the project's old `cmake_minimum_required`
#      -> pass -DCMAKE_POLICY_VERSION_MINIMUM=3.5
#   2. Homebrew SDL2 libs live in $(brew --prefix)/lib, which is not on the
#      default linker/compiler search path -> pass -L / -I explicitly.
#
set -e
cd "$(dirname "$0")"

BREW_PREFIX="$(brew --prefix)"
CORES="$(sysctl -n hw.ncpu)"

echo ">>> Ensuring SDL2 dependencies are installed..."
brew list --versions sdl2 sdl2_image sdl2_mixer sdl2_ttf sdl2_gfx >/dev/null 2>&1 \
  || brew install sdl2 sdl2_image sdl2_mixer sdl2_ttf sdl2_gfx

echo ">>> Configuring (CMake)..."
mkdir -p build
cd build
cmake \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
  -DCMAKE_EXE_LINKER_FLAGS="-L${BREW_PREFIX}/lib" \
  -DCMAKE_CXX_FLAGS="-O2 -g -I${BREW_PREFIX}/include" \
  ..

echo ">>> Building with ${CORES} cores..."
make -j"${CORES}"

cd ../Build_Release

echo ">>> Recreating ASSETS.ZIP..."
rm -f ASSETS.ZIP
( cd ASSETS && zip -qr ../ASSETS.ZIP . )

if [ "$(realpath openclaw)" != "$(realpath OpenClaw 2>/dev/null || echo /dev/null)" ]; then
  cp -f openclaw OpenClaw
fi

if [ ! -f CLAW.REZ ]; then
  echo
  echo "!!! CLAW.REZ is missing from $(pwd)"
  echo "!!! Copy the CLAW.REZ archive from your original Captain Claw (1997)"
  echo "!!! game into this folder, then run this script again to play."
  exit 1
fi

echo ">>> Running..."
./OpenClaw
