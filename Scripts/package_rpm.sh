#!/bin/bash

PROJECT_ROOT=..
BINARY_BUILD_DIR=$PROJECT_ROOT/Build_Release

TARGET_DIR=/tmp/captainclaw
TARGET_BINARY_DIR=$TARGET_DIR/bin
TARGET_ASSETS_DIR=$TARGET_DIR/usr/share/captainclaw

function ensure_exists 
{
	if [ ! -f $1 ]
	then
		echo "$1 does not exist ! Exiting..."
		exit 1
	fi
}

ensure_exists $BINARY_BUILD_DIR/captainclaw
ensure_exists $BINARY_BUILD_DIR/CLAW.REZ
ensure_exists $BINARY_BUILD_DIR/ASSETS.ZIP
ensure_exists $BINARY_BUILD_DIR/console02.tga
ensure_exists $BINARY_BUILD_DIR/clacon.ttf
ensure_exists $BINARY_BUILD_DIR/SAVES.XML

mkdir -p $TARGET_BINARY_DIR
mkdir -p $TARGET_ASSETS_DIR

cp $BINARY_BUILD_DIR/captainclaw $TARGET_BINARY_DIR
cp $BINARY_BUILD_DIR/CLAW.REZ $TARGET_ASSETS_DIR
cp $BINARY_BUILD_DIR/ASSETS.ZIP $TARGET_ASSETS_DIR
cp $BINARY_BUILD_DIR/console02.tga $TARGET_ASSETS_DIR
cp $BINARY_BUILD_DIR/clacon.ttf $TARGET_ASSETS_DIR
cp $BINARY_BUILD_DIR/SAVES.XML $TARGET_ASSETS_DIR


