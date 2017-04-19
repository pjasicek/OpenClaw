#!/bin/sh

coverity() {
    if [ ${TRAVIS_JOB_NUMBER##*.} -ne 1 ]; then
        exit 0
    fi

    curl -s "https://scan.coverity.com/scripts/travisci_build_coverity_scan.sh" | bash || true
}

prepare_linux() {
    sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-gfx-dev
    sudo apt-get install libtinyxml-dev
    sudo apt-get install cmake
}

prepare_android() {
    git clone 'https://github.com/loblik/captainclaw-arm-linux-androideabi-4.9.git' ~/android-toolchain
}

prepare_osx() {
    brew install sdl2 sdl2_image sdl2_mixer sdl2_ttf sdl2_gfx tinyxml;
}

build() {
    case "$PLATFORM" in
        linux|android|osx)
            prepare_${PLATFORM}
            ;;
    esac

    git config user.email "travis@build.bot" && git config user.name "Travis CI"
    git tag -a -m "Travis build" init

    mkdir build; cd build; cmake $CMAKE_ARGS ..
    make -j8
}

case "$TRAVIS_EVENT_TYPE" in
    cron)
        coverity
        ;;
    *)
        build
        ;;
esac
