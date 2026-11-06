# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.24)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(PigControl C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(${PROJECT_NAME}
    PigControl.cpp
)

pico_enable_stdio_usb(PigControl 1)
#pico_generate_pio_header(PigControl ${CMAKE_CURRENT_LIST_DIR}/nec_receive.pio)
target_include_directories(PigControl PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# pull in common dependencies and additional i2c hardware support
target_link_libraries(${PROJECT_NAME} StepEncoder DispUnit MenueNodes VolChDevice RemoteCtrl pico_stdlib hardware_pio)
#target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_pio)
    
add_subdirectory(StepEncoder)
add_subdirectory(DispUnit)
add_subdirectory(MenueNodes)
add_subdirectory(VolChDevice)
add_subdirectory(RemoteCtrl)

# create map/bin/hex file etc.
pico_add_extra_outputs(PigControl)

# add url via pico_set_program_url
