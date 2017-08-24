#!/bin/bash

PROJECT_ROOT=..
BINARY_BUILD_DIR=$PROJECT_ROOT/Build_Release

TARGET_TOPDIR=/tmp/openclaw
TARGET_DIR=$TARGET_TOPDIR/BUILDROOT
TARGET_BINARY_DIR=$TARGET_DIR/usr/bin
TARGET_ASSETS_DIR=$TARGET_DIR/usr/share/openclaw

function ensure_exists 
{
	if [ ! -f $1 ]
	then
		echo "$1 does not exist ! Exiting..."
		exit 1
	fi
}

ensure_exists $BINARY_BUILD_DIR/openclaw
ensure_exists $BINARY_BUILD_DIR/CLAW.REZ
ensure_exists $BINARY_BUILD_DIR/ASSETS.ZIP
ensure_exists $BINARY_BUILD_DIR/console02.tga
ensure_exists $BINARY_BUILD_DIR/clacon.ttf
ensure_exists $BINARY_BUILD_DIR/SAVES.XML
ensure_exists $BINARY_BUILD_DIR/config_linux_release.xml
ensure_exists $BINARY_BUILD_DIR/clawlauncher
ensure_exists $BINARY_BUILD_DIR/ClawLauncher.exe

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

echo "rpmbuild --define "_topdir $TARGET_TOPDIR" --define "buildroot $TARGET_DIR" -ba openclaw.spec"
rpmbuild --define "_topdir $TARGET_TOPDIR" --define "buildroot $TARGET_DIR" -ba openclaw.spec

cp $TARGET_TOPDIR/RPMS/*/*.rpm .
