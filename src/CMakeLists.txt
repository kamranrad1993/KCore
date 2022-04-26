
set(KCORE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
message(STATUS "searching in \"" ${KCORE_SOURCE_DIR} "\" for sources")
file(GLOB_RECURSE KCORE_SOURCES
    ${KCORE_SOURCE_DIR}/*.cpp
    ${KCORE_SOURCE_DIR}/*.mm
)