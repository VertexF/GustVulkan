#include "PreComp.h"
#include "VulkanTest.h"


#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

namespace Gust 
{
    void VulkanTest::mainLoop() 
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

        if (glfwVulkanSupported())
        {
            
            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
            GUST_INFO("Extensions supported {0}", extensionCount);

            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();
            }
        }

        glfwDestroyWindow(window);

        glfwTerminate();
    }
}