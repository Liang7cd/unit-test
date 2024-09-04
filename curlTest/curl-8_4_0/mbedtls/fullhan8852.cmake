# the name of the target operating system
set(CMAKE_SYSTEM_NAME Linux)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER   arm-fullhanv3-linux-uclibcgnueabi-gcc)
set(CMAKE_CXX_COMPILER arm-fullhanv3-linux-uclibcgnueabi-g++)
set(CMAKE_AR arm-fullhanv3-linux-uclibcgnueabi-ar)
set(CMAKE_C_FLAGS "-g -Wno-type-limits -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4")
set(CMAKE_CXX_FLAGS "-g -Wno-type-limits -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4")

# where is the target environment located
set(CMAKE_FIND_ROOT_PATH  /opt/fuhan/toolchain/arm-fullhanv3-linux-uclibcgnueabi-b6/)

# adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)