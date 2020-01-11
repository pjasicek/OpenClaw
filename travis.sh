#!/bin/sh

coverity() {
    if [ ${TRAVIS_JOB_NUMBER##*.} -ne 1 ]; then
        exit 0
    fi

    prepare_linux

    curl -s "https://scan.coverity.com/scripts/travisci_build_coverity_scan.sh" | bash || true
}

prepare_linux() {
    sudo apt-get update
    sudo apt-get install libsdl2-2.0-0
    sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-gfx-dev
#    sudo apt-get install libtinyxml-dev
    sudo apt-get install cmake
}

prepare_android() {
    git clone 'https://github.com/loblik/captainclaw-arm-linux-androideabi-4.9.git' ~/android-toolchain
}

prepare_osx() {
    brew install sdl2 sdl2_image sdl2_mixer sdl2_ttf sdl2_gfx;
#    brew install tinyxml;
}

prepare_emscripten() {
  git clone https://github.com/emscripten-core/emsdk.git
  cd emsdk
  ./emsdk install latest
  ./emsdk activate latest
  . ./emsdk_env.sh
  cd ..

  emcc -v
}

build() {
    case "$PLATFORM" in
        linux|android|osx|emscripten)
            prepare_${PLATFORM}
            ;;
    esac

    git config user.email "travis@build.bot" && git config user.name "Travis CI"
    git tag -a -m "Travis build" init

    mkdir build; cd build; $CMAKE_EXECUTOR cmake $CMAKE_ARGS ..

    case "$PLATFORM" in
        windows)
            # TODO: MSBuild.exe command not found. Why???
            "/C/Program Files (x86)/Microsoft Visual Studio/2017/BuildTools/MSBuild/15.0/Bin/MSBuild.exe" OpenClaw.sln //p:Configuration=Debug //p:Platform="Win32"
            ;;
        *)
            make -j8
            ;;
    esac
}

case "$TRAVIS_EVENT_TYPE" in
    cron)
        coverity
        ;;
    *)
        build
        ;;
esac
