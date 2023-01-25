file(GLOB_RECURSE SPDLOG_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

file(GLOB_RECURSE SPDLOG_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_library(SPDLOG STATIC ${SPDLOG_INCLUDE} ${SPDLOG_SOURCE})

add_definitions(-DSPDLOG_COMPILED_LIB)

include_directories(SPDLOG PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

#if(MSVC)
#    set_property(TARGET SPDLOG PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#endif()