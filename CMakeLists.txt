cmake_minimum_required(VERSION 3.2)
# cmake_policy(SET CMP0069 NEW) 
# set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
# include(CheckIPOSupported)
# check_ipo_supported(RESULT supported OUTPUT error)


project(KCore)
set(CMAKE_CXX_STANDARD 20)
# list(APPEND ${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} "-rdynamic") 
# set(CMAKE_CXX_FLAGS_DEBUG "-g3")


if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(PLATFORM "WINDOWS_PLATFORM")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PLATFORM "LINUX_PLATFORM")
else()
    message(FATAL_ERROR "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif()


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
