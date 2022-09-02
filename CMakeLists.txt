cmake_minimum_required(VERSION 3.2)
# cmake_policy(SET CMP0069 NEW) 
# set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
# include(CheckIPOSupported)
# check_ipo_supported(RESULT supported OUTPUT error)

set(PLATFORM "Linux" CACHE STRING "Target build platform")
set(PLATFORM_VALUES "Linux;Windows;Android;Ios;Mac" CACHE STRING "List of possible build platforms")
set_property(CACHE PLATFORM PROPERTY STRINGS ${PLATFORM_VALUES})
set(PLATFORM ${PLATFORM}_PLATFORM)
string(TOUPPER ${PLATFORM} PLATFORM)
message(STATUS "TARGET PLATFORM IS: " ${PLATFORM})

if(${PLATFORM} MATCHES "ANDROID_PLATFORM")
# set(CMAKE_LINKER  $ENV{ANDROID_NDK_25}/toolchains/llvm/prebuilt/linux-x86_64/bin/lld)
# set(CMAKE_CXX_COMPILER $ENV{ANDROID_NDK_25}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++)
# set(CMAKE_C_COMPILER $ENV{ANDROID_NDK_25}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang)
# set(CMAKE_CXX_LINK_EXECUTABLE ${CMAKE_LINKER})
# set(CMAKE_C_LINK_EXECUTABLE ${CMAKE_LINKER})
# set(ANDROID_NDK $ENV{ANDROID_NDK_25})
# set(CMAKE_ANDROID_NDK $ENV{ANDROID_NDK_25})
set(CMAKE_ANDROID_ARCH_ABI armeabi-v7a)
set(ANDROID_ABI armeabi-v7a)
add_compile_options(--target=armv7a-linux-androideabi28)
set(ANDROID_STL c++_shared)
set(CMAKE_TOOLCHAIN_FILE $ENV{ANDROID_NDK_25}/build/cmake/android.toolchain.cmake)
message(STATUS "Using Toolchain: " ${CMAKE_TOOLCHAIN_FILE})
endif()

project(KCore)
set(CMAKE_CXX_STANDARD 20)
# list(APPEND ${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} "-rdynamic") 
# set(CMAKE_CXX_FLAGS_DEBUG "-g3")

option(BUILD_TESTS ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/include/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/src/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/external/CMakeLists.txt)

set(PROJECT_HEADER_DIR
    ${KCORE_HEADER_DIR}
    ${BACKWARD_INCLUDE_DIR}
    ${ASIO_INCLUDE_DIR}
    ${CURL_INCLUDE_DIR}
)

include_directories(${PROJECT_HEADER_DIR})

set(PROJECT_HEADERS
    ${KCORE_HEADERS}
    ${EXTERNAL_HEADERS}
)

set(PROJECT_SOURCES
    ${KCORE_SOURCES}
    ${EXTERNAL_SOURCES}
)

add_library(KCore-set-app-flags INTERFACE)
target_compile_options(KCore-set-app-flags INTERFACE -O0)

add_library(KCore STATIC ${PROJECT_HEADERS} ${PROJECT_SOURCES} )
target_link_libraries(KCore
    KCore-set-app-flags
    stdc++
    ${CMAKE_DL_LIBS}
    # ${CURL_LIB_FILE}
    # ssl #TODO curl dependency. enable it in external cmake.
    # crypto #TODO curl dependency. enable it in external cmake.
    # z #TODO curl dependency. enable it in external cmake.
)
target_compile_definitions(KCore PUBLIC  
    #disable debug optimization seems that not work in this way 
    -DCMAKE_CXX_FLAGS_DEBUG=-O0
    -DBACKWARD_HAS_UNWIND=1
    -D${PLATFORM}
)

if(${PLATFORM} MATCHES "WINDOWS_PLATFORM")
    target_compile_definitions(KCore PUBLIC  
    )
    target_link_libraries(KCore
        ws2_32
        DbgHelp
    )
endif()

# if( supported )
#     message(STATUS "IPO / LTO enabled")
#     set_property(TARGET KCore PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
# else()
#     message(STATUS "IPO / LTO not supported: <${error}>")
# endif()


if(${BUILD_TESTS})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests)
endif()
