cmake_minimum_required(VERSION 3.10)
project(boiler-hue-edk LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent module for downloading dependencies
include(FetchContent)

# Set the EDK directory path
set(EDK_DIR "${CMAKE_BINARY_DIR}/_deps/philips-edk")

# Download and build Philips Hue EDK if not already present
if(NOT EXISTS "${EDK_DIR}")
    message(STATUS "Downloading Philips Hue EDK...")
    message(STATUS "Note: This requires SSH access to the private repository")
    message(STATUS "Make sure your SSH keys are configured with GitHub")
    
    FetchContent_Declare(
        philips_edk
        GIT_REPOSITORY git@github.com:PhilipsHue/EDK.git
        GIT_TAG master
        SOURCE_DIR ${EDK_DIR}
    )
    
    FetchContent_MakeAvailable(philips_edk)
    
    # Build the EDK
    message(STATUS "Building Philips Hue EDK...")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -S ${EDK_DIR} -B ${EDK_DIR}/build 
            -DBUILD_TEST=OFF 
            -DBUILD_EXAMPLES=OFF 
            -DBUILD_TOOLS=OFF 
            -DBUILD_WRAPPERS=OFF 
            -DLIB_BUILD_MODE=STATIC
        RESULT_VARIABLE EDK_CONFIG_RESULT
    )
    
    if(NOT EDK_CONFIG_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to configure Philips Hue EDK")
    endif()
    
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ${EDK_DIR}/build --target huestream support bridge_discovery
        RESULT_VARIABLE EDK_BUILD_RESULT
    )
    
    if(NOT EDK_BUILD_RESULT EQUAL 0)
        message(WARNING "Failed to build specific EDK targets, trying to build all libraries...")
        execute_process(
            COMMAND ${CMAKE_COMMAND} --build ${EDK_DIR}/build
            RESULT_VARIABLE EDK_BUILD_RESULT
        )
        if(NOT EDK_BUILD_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to build Philips Hue EDK")
        endif()
    endif()
    
    message(STATUS "Philips Hue EDK built successfully")
endif()

# Collect source files
file(GLOB SOURCES
    "main.cpp"
    "effects/*.cpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${EDK_DIR}/libhuestream
    ${EDK_DIR}/libhuestream/support/include
    ${EDK_DIR}/libhuestream/bridgediscovery/include
    ${EDK_DIR}/3rd_party
    ${EDK_DIR}/3rd_party/edtls/libedtls
)

# Platform-specific libraries
if(APPLE)
    # macOS specific libraries
    find_library(SYSTEM_CONFIGURATION SystemConfiguration)
    find_library(SECURITY Security)
    find_library(CORE_FOUNDATION CoreFoundation)
    find_library(IO_KIT IOKit)
    find_library(FOUNDATION Foundation)
    find_library(APP_KIT AppKit)
    find_library(CF_NETWORK CFNetwork)
    
    # Try to find Homebrew libraries
    find_library(LIBIDN2 idn2 PATHS /usr/local/opt/libidn2/lib /opt/homebrew/opt/libidn2/lib)
    find_library(LIBUNISTRING unistring PATHS /usr/local/opt/libunistring/lib /opt/homebrew/opt/libunistring/lib)
    
    if(NOT LIBIDN2 OR NOT LIBUNISTRING)
        message(WARNING "libidn2 or libunistring not found. Install with: brew install libidn2 libunistring")
    endif()
    
    set(PLATFORM_LIBS
        ${SYSTEM_CONFIGURATION}
        ${SECURITY}
        ${CORE_FOUNDATION}
        ${IO_KIT}
        ${FOUNDATION}
        ${APP_KIT}
        ${CF_NETWORK}
        ${LIBIDN2}
        ${LIBUNISTRING}
        z
        resolv
        dl
        pthread
        iconv
    )
elseif(UNIX)
    # Linux specific libraries
    set(PLATFORM_LIBS
        dl
        pthread
    )
elseif(WIN32)
    # Windows specific libraries
    set(PLATFORM_LIBS
        ws2_32
        crypt32
    )
endif()

# Link against the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    # Hue EDK libraries
    ${EDK_DIR}/build/bin/libhuestream.a
    ${EDK_DIR}/build/bin/libsupport.a
    ${EDK_DIR}/build/bin/libbridge_discovery.a
    
    # Third-party dependencies
    ${EDK_DIR}/build/external_install/lib/libmdns_responder.a
    ${EDK_DIR}/build/external_install/lib/libjson.a
    ${EDK_DIR}/build/external_install/lib/libmbedtls.a
    ${EDK_DIR}/build/external_install/lib/libmbedcrypto.a
    ${EDK_DIR}/build/external_install/lib/libmbedx509.a
    ${EDK_DIR}/build/external_install/lib/libmbedcl_wrapper.a
    ${EDK_DIR}/build/external_install/lib/libcurl.a
    ${EDK_DIR}/build/external_install/lib/libedtls_client.a
    ${EDK_DIR}/build/external_install/lib/libeverest.a
    ${EDK_DIR}/build/external_install/lib/libp256m.a
    ${EDK_DIR}/build/external_install/lib/libnghttp2_static.a
    
    # Platform-specific libraries
    ${PLATFORM_LIBS}
)

# Set the output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Print build information
message(STATUS "==================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "EDK Directory: ${EDK_DIR}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "==================================")