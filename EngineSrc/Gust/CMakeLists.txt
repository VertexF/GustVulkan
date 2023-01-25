file(GLOB_RECURSE GUST_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/Gust/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Gust/*.h"
)

if(MSVC)
    set(GLFW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vender/glfw/include")
    set(VULKAN_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vender/vulkan/Include")
    set(SPDLOG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vender/spdlog/include")
    set(GML_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vender/glm")
    set(STB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vender/stb")
    #I need to uses a config file that is generated here. So it's not a 
    #mistake it's a binrary directory.
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GUST_SOURCE})

    set(VULKAN_LIB "${CMAKE_CURRENT_SOURCE_DIR}/vender/vulkan/Lib/vulkan-1.lib")
    set(GLFW_LIB "${CMAKE_CURRENT_SOURCE_DIR}/vender/glfw/lib-vc2022/glfw3.lib")
endif()

add_subdirectory(vender/spdlog)
add_subdirectory(vender/stb)

add_library(Gust STATIC ${GUST_SOURCE}) 

#if(MSVC)
#    set_property(TARGET Gust PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#endif()

add_compile_definitions("$<$<CONFIG:Debug>:GUST_DEBUG>")

set(PRECOMPILED_HEADER Gust/PublicInclude)
target_precompile_headers(Gust PUBLIC "${PRECOMPILED_HEADER}/PreComp.h")

target_include_directories(Gust PUBLIC ${SPDLOG_INCLUDE_DIR} ${PRECOMPILED_HEADER} ${IMGUI_INCLUDE_DIR} ${GML_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${STB_INCLUDE_DIR} ${VULKAN_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Gust PUBLIC SPDLOG STB ${GLFW_LIB} ${VULKAN_LIB})