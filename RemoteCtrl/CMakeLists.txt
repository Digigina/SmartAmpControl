cmake_minimum_required(VERSION 3.24)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Pull in Raspberry Pi Pico SDK (must be before project)
#include(pico_sdk_import.cmake)

project(RemoteCtrl C CXX ASM)


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

add_library(${PROJECT_NAME} STATIC Protocol.cpp IRQ_Handler.cpp)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/nec_receive.pio)
target_include_directories( ${PROJECT_NAME} 
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE   ${CMAKE_CURRENT_BINARY_DIR}
    )

# pull in common dependencies
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib 
    hardware_pio
    VolChDevice
)
