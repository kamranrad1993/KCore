set(KCORE_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
message(STATUS "searching in \"" ${KCORE_HEADER_DIR} "\" for headers")
file(GLOB_RECURSE KCORE_HEADERS
    ${KCORE_HEADER_DIR}/*.h
)

