cmake_minimum_required(VERSION 3.19.0)

project(Game 
        VERSION 0.0.0
        LANGUAGES CXX C
        DESCRIPTION "Demo project for learning Vulkan")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE GAME_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/GameSrc/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameSrc/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/GameSrc/*.vert
	${CMAKE_CURRENT_SOURCE_DIR}/GameSrc/*.frag
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(GUST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/EngineSrc/Gust")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GAME_SOURCE})

add_subdirectory(EngineSrc/Gust)

add_executable(Game ${GAME_SOURCE})

#if(MSVC)
#    set_property(TARGET Game PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#endif()

target_compile_options(Game PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)

#Using a bit of post-processing we can select the varaiables we need to
#get the correct version of the share library after compiling it.
#As we have already built the Game target it will know where to copy it to.
add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/GameSrc/Resources"
        $<TARGET_FILE_DIR:Game>)

add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/GameSrc/Resources"
        ${PROJECT_BINARY_DIR})
        
target_include_directories(Game PUBLIC ${GUST_INCLUDE_DIR})
target_link_libraries(Game PRIVATE Gust)
