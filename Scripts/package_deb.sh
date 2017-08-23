#!/bin/bash

PROJECT_ROOT=..
BINARY_BUILD_DIR=$PROJECT_ROOT/Build_Release

TARGET_TOPDIR=/tmp/openclaw

PACKAGE_NAME="openclaw"
PACKAGE_VERSION="1.0-1"

function ensure_exists 
{
	if [ ! -f $1 ]
	then
		echo "$1 does not exist ! Exiting..."
		exit 1
	fi
}

if [ $# -ne 0 ]
then
    PACKAGE_VERSION="$1"
fi

PACKAGE="$PACKAGE_NAME"_"$PACKAGE_VERSION"
TARGET_DIR=$TARGET_TOPDIR/$PACKAGE
TARGET_BINARY_DIR=$TARGET_DIR/usr/bin
TARGET_ASSETS_DIR=$TARGET_DIR/usr/share/openclaw

ensure_exists $BINARY_BUILD_DIR/openclaw
ensure_exists $BINARY_BUILD_DIR/CLAW.REZ
ensure_exists $BINARY_BUILD_DIR/ASSETS.ZIP
ensure_exists $BINARY_BUILD_DIR/console02.tga
ensure_exists $BINARY_BUILD_DIR/clacon.ttf
ensure_exists $BINARY_BUILD_DIR/SAVES.XML
ensure_exists $BINARY_BUILD_DIR/config_linux_release.xml
ensure_exists $BINARY_BUILD_DIR/clawlauncher
ensure_exists $BINARY_BUILD_DIR/ClawLauncher.exe
ensure_exists control
ensure_exists postinst
ensure_exists postrm

mkdir -p $TARGET_BINARY_DIR
mkdir -p $TARGET_ASSETS_DIR

cp $BINARY_BUILD_DIR/openclaw $TARGET_BINARY_DIR
cp $BINARY_BUILD_DIR/CLAW.REZ $TARGET_ASSETS_DIR
cp $BINARY_BUILD_DIR/ASSETS.ZIP $TARGET_ASSETS_DIR
cp $BINARY_BUILD_DIR/console02.tga $TARGET_ASSETS_DIR
cp $BINARY_BUILD_DIR/clacon.ttf $TARGET_ASSETS_DIR
cp $BINARY_BUILD_DIR/SAVES.XML $TARGET_ASSETS_DIR
cp $BINARY_BUILD_DIR/config_linux_release.xml $TARGET_ASSETS_DIR/config.xml
cp $BINARY_BUILD_DIR/ClawLauncher.exe $TARGET_BINARY_DIR
cp $BINARY_BUILD_DIR/clawlauncher $TARGET_BINARY_DIR

mkdir -p $TARGET_DIR/DEBIAN
cp control $TARGET_DIR/DEBIAN
cp postinst $TARGET_DIR/DEBIAN
cp postrm $TARGET_DIR/DEBIAN

echo "dpkg-deb --build $TARGET_DIR"
dpkg-deb --build $TARGET_DIR

cp $TARGET_TOPDIR/*.deb .
