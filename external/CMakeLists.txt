set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(EXTERNAL_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/external)
set(EXTERNAL_LIBS_DIR ${CMAKE_CURRENT_BINARY_DIR}/LibsBuilds)

include(ExternalProject)
set(CURL_COMMAND "curl")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(TAR_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/${CMAKE_SYSTEM_NAME}/bsdtar.exe)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(TAR_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/${CMAKE_SYSTEM_NAME}/bsdtar)
else()
    message(FATAL_ERROR "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif()



set(BACKWARD_SOURCE ${EXTERNAL_DIR}/backward-cpp-master)
set(BACKWARD_ARCHIVE ${EXTERNAL_DIR}/backward.zip)
if(NOT EXISTS ${BACKWARD_ARCHIVE})
    message(STATUS "Download Backward-cpp")
    execute_process(
        COMMAND ${CURL_COMMAND} -L -o ${BACKWARD_ARCHIVE} https://github.com/kamranrad1993/backward-cpp/archive/refs/heads/master.zip
        WORKING_DIRECTORY ${EXTERNAL_DIR}
        RESULT_VARIABLE result
    )
    if(result)
        message(STATUS "Path : "  ${BACKWARD_ARCHIVE})
        message(FATAL_ERROR "Failed To Download BACKWARD : " ${result})
    endif()

    execute_process(
        COMMAND ${TAR_COMMAND} -xf ${BACKWARD_ARCHIVE}
        WORKING_DIRECTORY ${EXTERNAL_DIR}
        RESULT_VARIABLE result
    )
    if(result)
        message(FATAL_ERROR "Failed To Extract BACKWARD : " ${error_code})
    endif()
else()
    message(STATUS "Backward Archive Loaded At Path : "  ${BACKWARD_ARCHIVE})
endif()
file(GLOB BACKWARD_HEADERS ${BACKWARD_SOURCE}/*.hpp)
file(GLOB BACKWARD_SOURCES ${BACKWARD_SOURCE}/*.cpp)
set(BACKWARD_INCLUDE_DIR ${BACKWARD_SOURCE})
list(APPEND EXTERNAL_HEADERS ${BACKWARD_HEADERS})
list(APPEND EXTERNAL_SOURCES ${BACKWARD_SOURCES})




set(ASIO_SOURCE ${EXTERNAL_DIR}/asio-1.22.1)
set(ASIO_ARCHIVE ${EXTERNAL_DIR}/asio.tar.bz2)
if(NOT EXISTS ${ASIO_ARCHIVE})
    message(STATUS "Download ASIO")
    execute_process(
        COMMAND ${CURL_COMMAND} -L -o ${ASIO_ARCHIVE} https://altushost-swe.dl.sourceforge.net/project/asio/asio/1.22.1%20%28Stable%29/asio-1.22.1.tar.bz2
        WORKING_DIRECTORY ${EXTERNAL_DIR}
        RESULT_VARIABLE result
    )
    if(result)
        message(STATUS "Path : "  ${ASIO_ARCHIVE})
        message(FATAL_ERROR "Failed To Download ASIO : " ${result})
    endif()

    execute_process(
        COMMAND ${TAR_COMMAND} -xf ${ASIO_ARCHIVE}
        WORKING_DIRECTORY ${EXTERNAL_DIR}
        RESULT_VARIABLE result
    )
    if(result)
        message(FATAL_ERROR "Failed To Extract ASIO : " ${error_code})
    endif()
else()
    message(STATUS "ASIO Archive Loaded At Path : "  ${ASIO_ARCHIVE})
endif()
file(GLOB_RECURSE ASIO_HEADERS 
    ${ASIO_SOURCE}/include/*.hpp
    ${ASIO_SOURCE}/include/*.
)
set(ASIO_INCLUDE_DIR ${ASIO_SOURCE}/include)
list(APPEND EXTERNAL_HEADERS ${ASIO_HEADERS})




set(CURL_NAME_VERSION curl-7.83.0)
set(CURL_SOURCE ${EXTERNAL_DIR}/${CURL_NAME_VERSION})
set(CURL_ARCHIVE ${EXTERNAL_DIR}/${CURL_NAME_VERSION}.zip)
if(NOT EXISTS ${CURL_ARCHIVE})
    message(STATUS "Download CURL")
    execute_process(
        COMMAND ${CURL_COMMAND} -L -o ${CURL_ARCHIVE} https://curl.se/download/curl-7.83.0.zip
        WORKING_DIRECTORY ${EXTERNAL_DIR}
        RESULT_VARIABLE result
    )
    if(result)
        message(STATUS "Path : "  ${CURL_ARCHIVE})
        message(FATAL_ERROR "Failed To Download CURL : " ${result})
    endif()

    execute_process(
        COMMAND ${TAR_COMMAND} -xf ${CURL_ARCHIVE}
        WORKING_DIRECTORY ${EXTERNAL_DIR}
        RESULT_VARIABLE result
    )
    if(result)
        message(FATAL_ERROR "Failed To Extract CURL : " ${error_code})
    endif()
else()
    message(STATUS "CURL Archive Loaded At Path : "  ${CURL_ARCHIVE})
endif()
file(GLOB_RECURSE CURL_HEADERS 
    ${CURL_SOURCE}/include/*.hpp
    ${CURL_SOURCE}/include/*.
)
set(CURL_INCLUDE_DIR ${CURL_SOURCE}/include)
set(CURL_LIB_FILE ${EXTERNAL_BUILD_DIR}/${CURL_NAME_VERSION}/lib/libcurl-d.a)
list(APPEND EXTERNAL_HEADERS ${CURL_HEADERS})
set(BUILD_SHARED_LIBS OFF)
# add_subdirectory(${CURL_SOURCE})