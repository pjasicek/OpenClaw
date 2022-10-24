#!/bin/bash

# variables
canonical_file_name=`readlink -f ${0}`
project_dir=`dirname ${canonical_file_name}`
build_dir="${project_dir}/build"
release_dir="${project_dir}/Build_Release"
assets_dir="${release_dir}/ASSETS"

assets_zip_filename="ASSETS.ZIP"
assets_zip_file="${release_dir}/${assets_zip_filename}"
openclaw_binary_file="${release_dir}/openclaw"
compatible_binary_file="${release_dir}/OpenClaw"

# functions

##
# Prepares the building process by creating a directory for cmake native build process. 
# return: the result of [mkdir], [0] for success and [1] for failure.
##
function prepare() {
    echo "Preparing the build directory ..."
    if [ ! -d $build_dir ]; then
        mkdir $build_dir
    fi
    return $?
}

##
# Generates the native build system for the project.
# return: the result of [cmake] command.
##
function generateNativeBuildSystem() {
    if [ ! -d $build_dir ]; then
        return prepare
    fi
    echo "Generating the OpenClaw native build system ..."
    cmake -S $project_dir -B $build_dir
    return $?
}

##
# Builds the openclaw engine and binaries into object files.
# return: the result of [make] command.
##
function build() {
    if [ ! -d $build_dir ]; then
        return compile
    fi
    echo "Building the binary ..."
    make -j$(nproc) -C "${build_dir}" 
    return $?
}   

##
# Zips the assets folder for the engine.
# return: the result of [zip] command.
## 
function releaseAssets() {
    if [ ! -d $release_dir ]; then
        return compile
    fi
    echo "Recreating ASSETS.ZIP ..."
    rm -f $assets_zip_file
    cd ${assets_dir}
    zip -r $assets_zip_filename .
    cp $assets_zip_filename ..
    rm $assets_zip_filename
    return $?
}

##
# Creates a claw binary to be compatible with the ClawLauncher.
# return: the result of [cp] command, [0] for success and [1] for failure.
##
function createLauncherBinary() {
    if [ ! -f $openclaw_binary_file ]; then
        return build
    fi
    echo "Creating a compatible binary ..."
    cp $openclaw_binary_file $compatible_binary_file
    rm $openclaw_binary_file
}

##
# Runs the openclaw binary.
##
function runOpenClaw() {
    if [ ! -d $release_dir ]; then
        return createLauncherBinary
    fi
    echo "Runing OpenClaw ..."
    $compatible_binary_file
}

# command and execute

prepare

generateNativeBuildSystem

build

releaseAssets

createLauncherBinary

runOpenClaw
