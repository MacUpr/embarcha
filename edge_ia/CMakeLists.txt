# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13.1)

set(PROJECT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})

# Import Pico SDK
include(pico_sdk_import.cmake)

project(sea_state_classifier C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# Initialize the Pico SDK
pico_sdk_init()

# Edge Impulse specific definitions
add_definitions(-DEIDSP_QUANTIZE_FILTERBANK=0
                -DEI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN=0
                -DARM_MATH_LOOPUNROLL
                )

# Main executable - only main.cpp needed now
add_executable(app
    ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
)

# Include Edge Impulse SDK utilities
include(${PROJECT_FOLDER}/edge-impulse-sdk/cmake/utils.cmake)

# Link required libraries
target_link_libraries(app 
    pico_stdlib
    hardware_i2c
    hardware_gpio
)

# Include directories
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${PROJECT_FOLDER}/tflite-model
    ${PROJECT_FOLDER}/model-parameters
)

# Enable USB output, disable UART output
pico_enable_stdio_usb(app 1)
pico_enable_stdio_uart(app 0)

# Find all Edge Impulse SDK source files
RECURSIVE_FIND_FILE(SOURCE_FILES "${PROJECT_FOLDER}/edge-impulse-sdk" "*.cpp")
RECURSIVE_FIND_FILE(MODEL_FILES "${PROJECT_FOLDER}/tflite-model" "*.cpp")
RECURSIVE_FIND_FILE(CC_FILES "${PROJECT_FOLDER}/edge-impulse-sdk" "*.cc")
RECURSIVE_FIND_FILE(C_FILES "${PROJECT_FOLDER}/edge-impulse-sdk" "*.c")
RECURSIVE_FIND_FILE(S_FILES "${PROJECT_FOLDER}/edge-impulse-sdk" "*.s")

list(APPEND SOURCE_FILES ${S_FILES})
list(APPEND SOURCE_FILES ${C_FILES})
list(APPEND SOURCE_FILES ${CC_FILES})
list(APPEND SOURCE_FILES ${MODEL_FILES})

# Add all Edge Impulse sources to the project
target_sources(app PRIVATE ${SOURCE_FILES})

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(app)

# Print debug information
message(STATUS "App sources: ${APP_SOURCES}")
message(STATUS "Include directories: ${CMAKE_CURRENT_SOURCE_DIR}")