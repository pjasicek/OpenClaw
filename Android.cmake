set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

#set(tools ~/android-toolchain)
#set(CMAKE_STAGING_PREFIX stage)

set(CMAKE_C_COMPILER ${tools}/bin/arm-linux-androideabi-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/arm-linux-androideabi-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
