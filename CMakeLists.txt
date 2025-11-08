cmake_minimum_required(VERSION 3.10)

# =============================================
# 1. Project Configuration
# =============================================
project(boiler-hue-edk LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =============================================
# 2. EDK Dependency
# =============================================
include(FetchContent)

# EDK build options
set(BUILD_CURL ON CACHE BOOL "Build Curl" FORCE)
set(BUILD_TEST OFF CACHE BOOL "Build tests" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(BUILD_TOOLS OFF CACHE BOOL "Build tools" FORCE)
set(BUILD_SWIG OFF CACHE BOOL "Build swig" FORCE)
set(BUILD_WRAPPERS OFF CACHE BOOL "Build wrappers" FORCE)
set(BUILD_TEST_COV OFF CACHE BOOL "Build tests with coverage" FORCE)
set(LIB_BUILD_MODE "STATIC" CACHE STRING "Library build mode" FORCE)

# Fetch and build EDK
FetchContent_Declare(
    philips_edk
    GIT_REPOSITORY https://github.com/PhilipsHue/EDK.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
message(STATUS "Fetching Philips EDK...")
FetchContent_MakeAvailable(philips_edk)

# =============================================
# 3. NDI Dependency
# =============================================
set(NDI_INCLUDE_DIR "./lib/ndi/include" CACHE PATH "NDI include dir" FORCE)
set(NDI_LIBRARY_DIR "./lib/ndi/bin" CACHE PATH "NDI library dir" FORCE)

# Find and link NDI library
find_library(NDI_LIB
    NAMES ndi libndi
    HINTS ${NDI_LIBRARY_DIR}
)
if(NOT EXISTS "${NDI_LIB}")
    message(FATAL_ERROR "NDI library not found. Please install it before")
else()
    message(STATUS "Found NDI library at: ${NDI_LIB}")
endif()

# =============================================
# 4. OpenCV Dependency
# =============================================
find_package(OpenCV REQUIRED)

# =============================================
# 5. Source Files
# =============================================
file(GLOB SOURCES
    "main.cpp"
    "effects/*.cpp"
    "hue/*.cpp"
    "ndi/*.cpp"
    "utils/*.cpp"
)

# =============================================
# 6. Create Executable and Link All Dependencies
# =============================================
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${philips_edk_SOURCE_DIR}/libhuestream/include
    ${NDI_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${NDI_LIB}
    huestream
    mbedtls
    mbedx509
    mbedcrypto
    ${OpenCV_LIBS}
)

# =============================================
# 7. Platform-Specific Libraries
# =============================================
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
        message(FATAL_ERROR "libidn2 not found. Please install it before")
    endif()
endif()
