#!/bin/bash

# terminal colors
RED_C='\033[38;2;255;50;50m'
# success color
GREEN_C='\e[38;2;0;180;0m'
# extra
WHITE_C='\e[38;2;255;255;255m'
RESET_Cs='\033[0;0m'

# variables
canonical_file_name=`readlink -f ${0}`
project_dir=`dirname ${canonical_file_name}`
build_dir="${project_dir}/build"
release_dir="${project_dir}/Build_Release"
assets_dir="${release_dir}/ASSETS"

assets_zip_filename="ASSETS.ZIP"
assets_zip_file="${release_dir}/${assets_zip_filename}"
openclaw_binary_file="openclaw"
compatible_binary_file="OpenClaw"

# functions

##
# Prints to the console using a color.
##
function print() {
    local color=$1
    local text=$2
    echo -e "${color} ${text}"
    echo -e ${RESET_Cs}
}

##
# Prepares the SDL-2-dev binaries to enable development of OpenClaw. 
# return: the result of the apt package manager, [0] for success and [1] for failure.
##
function prepareSDL2() {
    print ${WHITE_C} "Preparing SDL-2-dev dependencies ..."
    
    sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-gfx-dev 

    return $?
}

##
# Prepares the building process by creating a directory for cmake native build process. 
# return: the result of [mkdir], [0] for success and [1] for failure.
##
function prepareBuildDirectory() {
    print ${WHITE_C} "Preparing the build directory ..."
    
    if [ -d $build_dir ]; then
        return 0
    fi

    mkdir $build_dir

    return $?
}

##
# Generates the native build system for the project.
# return: the result of [cmake] command.
##
function generateNativeBuildSystem() {
    if [ ! -d $build_dir ]; then
        prepare
    fi

    print ${WHITE_C} "Generating the OpenClaw native build system ..."
    cmake -S $project_dir -B $build_dir

    return $?
}

##
# Builds the openclaw engine and binaries into object files.
# return: the result of [make] command.
##
function build() {
    if [ ! -d $build_dir ]; then
        generateNativeBuildSystem
    fi

    print ${WHITE_C} "Building the binary ..."
    make -j$(nproc) -C "${build_dir}" 

    return $?
}   

##
# Zips the assets folder for the engine.
# return: the implicit result of [zip] command.
## 
function releaseAssets() {
    if [ ! -d $release_dir ]; then
        build
    fi

    print ${WHITE_C} "Recreating ASSETS.ZIP ..."
    rm -v $assets_zip_file

    cd ${assets_dir}
    zip -v -r $assets_zip_filename .

    cp -v $assets_zip_filename ..
    rm -v $assets_zip_filename

    return $?
}

##
# Creates a claw binary to be compatible with the ClawLauncher.
# return: the result of [cp] command, [0] for success and [1] for failure.
##
function createLauncherBinary() {
    if [ ! -f "${release_dir}/${openclaw_binary_file}" ]; then
        build
    fi

    print ${WHITE_C} "Creating a compatible binary ..."

    cp -v "${release_dir}/${openclaw_binary_file}" "${release_dir}/${compatible_binary_file}"
    rm -v "${release_dir}/${openclaw_binary_file}"

    return $?
}

##
# Runs the openclaw binary.
##
function runOpenClaw() {
    if [ ! -f "${release_dir}/${compatible_binary_file}" ]; then
        createLauncherBinary
    fi

    print ${WHITE_C} "Runing OpenClaw ..."

    # change directory for the engine to load actor prototypes !
    cd $release_dir
    "./${compatible_binary_file}"
}

# command and execute
prepareSDL2

if [[ $? -ge 1 ]]; then 
    print ${RED_C} "Preparing SDL-2-dev failed ..."
    exit $?
else
    print ${GREEN_C} "Preparing SDL-2-dev succeeded ..."
fi

prepareBuildDirectory

if [[ $? -ge 1 ]]; then 
    print ${RED_C} "Preparing build directory failed ..."
    exit $?
else
    print ${GREEN_C} "Preparing build directory succeeded ..."
fi

generateNativeBuildSystem

if [[ $? -ge 1 ]]; then 
    print ${RED_C} "Failed to generate the native build directory ..."
    exit $?
else
    print ${GREEN_C} "Generated native build directory successfully ..."
fi

build

if [[ $? -gt 1 ]]; then 
    print ${RED_C} "Binary build failed  ..."
    exit $?
else
    print ${GREEN_C} "Binary build succeeded ..."
fi

releaseAssets

if [[ $? -gt 1 ]]; then 
    print ${RED_C} "Releasing Assests failed  ..."
    exit $?
else
    print ${GREEN_C} "Releasing Assests succeeded ..."
fi

createLauncherBinary

if [[ $? -gt 1 ]]; then 
    print ${RED_C} "Creating launcher binary failed  ..."
    exit $?
else
    print ${GREEN_C} "Creating launcher binary succeeded ..."
fi

runOpenClaw

if [[ $? -gt 1 ]]; then 
    print ${RED_C} "Running launcher binary failed ..."
    exit $?
else
    print ${GREEN_C} "Running launcher binary succeeded ..."
fi