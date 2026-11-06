cmake_minimum_required(VERSION 3.24)
project(StepEncoder)

add_library( ${PROJECT_NAME}  
    StepEncBase.cpp
    AdafruitRotaryQT.cpp
)

target_include_directories( ${PROJECT_NAME}
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}  pico_stdlib hardware_i2c)