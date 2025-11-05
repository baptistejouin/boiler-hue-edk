cmake_minimum_required(VERSION 3.10)
project(boiler-hue-edk LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Configure EDK build options BEFORE fetching
set(BUILD_CURL ON CACHE BOOL "Build Curl" FORCE)
set(BUILD_TEST OFF CACHE BOOL "Build tests" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(BUILD_TOOLS OFF CACHE BOOL "Build tools" FORCE)
set(BUILD_SWIG OFF CACHE BOOL "Build swig" FORCE)
set(BUILD_WRAPPERS OFF CACHE BOOL "Build wrappers" FORCE)
set(BUILD_TEST_COV OFF CACHE BOOL "Build tests with coverage" FORCE)
set(LIB_BUILD_MODE "STATIC" CACHE STRING "Library build mode" FORCE)

# Download and build EDK
FetchContent_Declare(
    philips_edk
    GIT_REPOSITORY https://github.com/PhilipsHue/EDK.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

message(STATUS "Fetching Philips EDK...")
FetchContent_MakeAvailable(philips_edk)

# The sources
file(GLOB SOURCES
    "main.cpp"
    "effects/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Include EDK headers
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${philips_edk_SOURCE_DIR}/libhuestream/include
)

# Link against the EDK library and its dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE 
    huestream
    mbedtls
    mbedx509
    mbedcrypto
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 wsock32)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread dl)
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        pthread
        "-framework CoreFoundation"
        "-framework Security"
        "-framework SystemConfiguration"
        "-framework CFNetwork"
        "-framework Foundation"
    )
    
    find_library(IDN2_LIBRARY idn2)
    if(IDN2_LIBRARY)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${IDN2_LIBRARY})
    else()
        message(WARNING "libidn2 not found. Please install it before")
    endif()
endif()