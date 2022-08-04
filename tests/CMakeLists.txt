cmake_minimum_required(VERSION 3.2)
# cmake_policy(SET CMP0069 NEW) 
# set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
# include(CheckIPOSupported)
# check_ipo_supported(RESULT supported OUTPUT error)

set(CMAKE_CXX_STANDARD 17)
# list(APPEND ${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} "-rdynamic") 
# set(CMAKE_CXX_FLAGS_DEBUG "-g3")


function(build_test name)
    message(STATUS "Build Test : " ${name})
    file(GLOB_RECURSE TEST_SOURCE_FILES 
            ${CMAKE_CURRENT_SOURCE_DIR}/${name}/*.cpp  
            ${CMAKE_CURRENT_SOURCE_DIR}/${name}/*.h  
            ${CMAKE_CURRENT_SOURCE_DIR}/${name}/*.mm
        )

    include_directories(
        ${PROJECT_HEADER_DIR}
    )
    
    add_executable(${name} ${TEST_SOURCE_FILES})
    target_include_directories(${name} PRIVATE ${PROJECT_HEADER_DIR})
    target_compile_definitions(${name} PUBLIC  
        #disable debug optimization seems that not work in this way 
        -DCMAKE_CXX_FLAGS_DEBUG=-O0
    )

    if(${PLATFORM} MATCHES "WINDOWS_PLATFORM")
        target_compile_definitions(${name} PUBLIC  
            -D_GLIBCXX_HAS_GTHREADS
            -D_GLIBCXX_USE_C99_STDINT_TR1
        )
    endif()
    
    #disable debug optimization
    #https://devzone.nordicsemi.com/f/nordic-q-a/77780/disable-optimization-of-part-of-the-code-through-cmake
    add_library(${name}set-app-flags INTERFACE)
    target_compile_options(${name}set-app-flags INTERFACE -O0)
    target_link_libraries(${name} PRIVATE ${name}set-app-flags KCore ${CMAKE_DL_LIBS})
    link_directories(/usr/lib/debug )


    # if( supported )
    #     message(STATUS "IPO / LTO enabled")
    #     set_property(TARGET ${name} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    # else()
    #     message(STATUS "IPO / LTO not supported: <${error}>")
    # endif()
    
endfunction()

build_test(logging)
build_test(networking)
build_test(socket)
build_test(streambuffer)