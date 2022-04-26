cmake_minimum_required(VERSION 3.2)


project(KCore)
set(CMAKE_CXX_STANDARD 17)
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
)
target_compile_definitions(KCore PUBLIC  
    #disable debug optimization seems that not work in this way 
    -DCMAKE_CXX_FLAGS_DEBUG=-O0
)

if(${BUILD_TESTS})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests ${CMAKE_CURRENT_BINARY_DIR}/tests)
endif()
