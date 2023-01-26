#include "WindowsWindow.h"

#include "PreComp.h"

#include <glm/glm.hpp>

#include "Gust/Events/ApplicationEvents.h"
#include "Gust/Events/KeyEvents.h"
#include "Gust/Events/MouseEvents.h"
#include "Gust/Events/Event.h"

#include "Gust/Core/Core.h"

#include <stb_image.h>
#include <cstdlib>
#include <optional>
#include <limits>
#include <array>

namespace
{
    //Makes sure the intialisation of GLFW happens only once.
    bool GLFWIntialised = false;

    void GLFWErrorCallBack(int error, const char *errorString) 
    {
        GUST_ERROR("GLFW Error ({0}): {1}", error, errorString);
    }

    const int MAX_FRAMES_IN_FLIGHT = 2;

    //Vulkan Debug Utilies like needs to be refactored.
    const std::vector<const char*> validationLayers = 
    {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

//#ifndef NDEBUG
//    const bool enableValidationLayers = false;
//#else
//    const bool enableValidationLayers = true;
//#endif // !NDEBUG
    const bool enableValidationLayers = true;

    VkResult createDebugUtilMessagerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, 
                                        const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT *debugMessanger) 
    {
        GUST_PROFILE_FUNCTION();
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        if (func != nullptr) 
        {
            return func(instance, createInfo, allocator, debugMessanger);
        }
        else 
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* allocator) 
    {
        GUST_PROFILE_FUNCTION();
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

        if (func != nullptr)
        {
            return func(instance, debugMessenger, allocator);
        }
    }

    struct Vertex 
    {
        glm::vec2 pos;
        glm::vec3 colour;

        static VkVertexInputBindingDescription getBindindDescription() 
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() 
        {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescription{};

            attributeDescription[0].binding = 0;
            attributeDescription[0].location = 0;
            attributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescription[0].offset = offsetof(Vertex, pos);

            attributeDescription[1].binding = 0;
            attributeDescription[1].location = 1;
            attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescription[1].offset = offsetof(Vertex, colour);

            return attributeDescription;
        }
    };

    const std::vector<Vertex> vertices = 
    {
        { {  0.f,  -0.5f }, { 1.f, 0.f, 0.f } },
        { {  0.5f,  0.5f }, { 0.f, 1.f, 0.f } },
        { { -0.5f,  0.5f }, { 0.f, 0.f, 1.f } }
    };
}

namespace Gust
{
    //Here we are constructing a window with the default properties.
    std::unique_ptr<Window> Window::create(const WindowProps& props)
    { 
        return std::make_unique<WindowsWindow>(props);
    }

    WindowsWindow::WindowsWindow(const WindowProps& props) : _window(nullptr)
    {
        GUST_PROFILE_FUNCTION();
        init(props);
    }

    WindowsWindow::~WindowsWindow() 
    {
        GUST_PROFILE_FUNCTION();
        shutdown();
    }

    //The windows properties is just the basic properties this window need to 
    //be created.
    //Very important to note that the order of intailsation here:
    //1) You need to intialise GLFW 
    //2) You need to create the window GLFW 
    //3) You need to create an OpenGL context with GLFW
    //4) You need to intialise GLEW 
    void WindowsWindow::init(const WindowProps& props) 
    {
        GUST_PROFILE_FUNCTION();
        _windowData.title = props.title;
        _windowData.width = props.width;
        _windowData.height = props.height;

        GUST_INFO("Creating window: {0} [{1}, {2}]", _windowData.title, _windowData.width, _windowData.height);

        if (GLFWIntialised == false)
        {
            if (glfwInit() == false)
            {
                GUST_ERROR("GLFW could not start!");
            }

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            //With this set up we now set up the errors to use our logger.
            glfwSetErrorCallback(GLFWErrorCallBack);

            GLFWIntialised = true;
        }

        _window = glfwCreateWindow(static_cast<int>(_windowData.width), static_cast<int>(_windowData.height),
            _windowData.title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(_window, this);
        glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);

        initVulkan();

        int width = 0;
        int height = 0;
        int channels;

        //stbi_uc* imageData;
        //{
        //    GUST_PROFILE_SCOPE("WindowsWindow::init loading icon - stbi_load");
        //    imageData = stbi_load("Assets/Textures/Ship.png", &width, &height, &channels, 0);
        //}

        //GLFWimage icon = { width, height, imageData};

        //glfwSetWindowIcon(_window, 1, &icon);

        glfwSetWindowUserPointer(_window, &_windowData);

        //Needs more vulkan to get this to work.
        setVSync(true);

        //All these function are using predicates to use our custom event handlers. 
        glfwSetWindowSizeCallback(_window, [](GLFWwindow *wind, int width, int height) 
            {
                WindowData& windowData = *(WindowData*)(glfwGetWindowUserPointer(wind));
                windowData.width = width;
                windowData.height = height;

                WindowResizeEvent windowResizeEvent(width, height);
                windowData.eventCallback(windowResizeEvent);
            });

        glfwSetWindowCloseCallback(_window, [](GLFWwindow * wind) 
            {
                WindowData& windowData = *(WindowData*)(glfwGetWindowUserPointer(wind));
                WindowClosedEvent closeEvent;

                windowData.eventCallback(closeEvent);
            });

        glfwSetKeyCallback(_window, [](GLFWwindow* wind, int key, int scanCode, int action, int mods)
            {
                WindowData& windowData = *(WindowData*)(glfwGetWindowUserPointer(wind));
            });

        glfwSetCharCallback(_window, [](GLFWwindow* wind, uint32_t character)
            {
                WindowData& windowData = *(WindowData*)(glfwGetWindowUserPointer(wind));
            });

        glfwSetMouseButtonCallback(_window, [](GLFWwindow *wind, int button, int action, int mods)
            {
                WindowData& windowData = *(WindowData*)(glfwGetWindowUserPointer(wind));

                //if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(g_MouseJustPressed))
                //    g_MouseJustPressed[button] = true;

                switch (action) 
                {
                case GLFW_PRESS:
                {
                    MouseButtonEventPressed pressEvent(button);
                    windowData.eventCallback(pressEvent);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonEventReleased releaseEvent(button);
                    windowData.eventCallback(releaseEvent);
                    break;
                }
                }
            });

        glfwSetScrollCallback(_window, [](GLFWwindow *wind, double x, double y) 
            {
                WindowData& windowData = *(WindowData*)(glfwGetWindowUserPointer(wind));

                MouseScrolledEvent scrollEvent(static_cast<float>(x), static_cast<float>(y));
                windowData.eventCallback(scrollEvent);
            });

        glfwSetCursorPosCallback(_window, [](GLFWwindow* wind, double x, double y) 
            {
                WindowData& windowData = *(WindowData*)(glfwGetWindowUserPointer(wind));

                MouseMovedEvent mouseMoved(static_cast<float>(x), static_cast<float>(y));
                windowData.eventCallback(mouseMoved);
            });
    }

    void WindowsWindow::shutdown()
    {
        GUST_PROFILE_FUNCTION();
        swapChainCleanUp();

        vkDestroyPipeline(_device, _graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
        vkDestroyRenderPass(_device, _renderPass, nullptr);

        vkDestroyBuffer(_device, _vertexBuffer, nullptr);
        vkFreeMemory(_device, _vertexBufferMemory, nullptr);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(_device, _renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(_device, _imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(_device, _inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(_device, _commandPool, nullptr);

        vkDestroyDevice(_device, nullptr);

        if (enableValidationLayers)
        {
            destroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(_instance, _surface, nullptr);
        vkDestroyInstance(_instance, nullptr);

        glfwDestroyWindow(_window);
    }

    //Currently just swaps the back buffer.
    void WindowsWindow::onUpdate() 
    {
        GUST_PROFILE_FUNCTION();
        glfwPollEvents();

        drawFrame();
    }

    uint32_t WindowsWindow::getWidth() const 
    { 
        return _windowData.width; 
    }

    uint32_t WindowsWindow::getHeight() const 
    { 
        return _windowData.height; 
    }

    void WindowsWindow::setCallbackFunction(const EventCallbackFunc& callback)
    {
        _windowData.eventCallback = callback;
    }

    void WindowsWindow::setVSync(bool vsync) 
    {
        GUST_PROFILE_FUNCTION();
        //Needs more Vulkan set up to get this working.

        _windowData.vSync = true;
    }

    bool WindowsWindow::isVSync() const 
    { 
        return _windowData.vSync;
    }

    void* WindowsWindow::getNativeWindow() const
    { 
        return _window; 
    }

    void WindowsWindow::drawFrame()
    {
        GUST_PROFILE_FUNCTION();
        vkWaitForFences(_device, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(_device, 1, &_inFlightFences[_currentFrame]);

        uint32_t imageIndex = -1;
        VkResult result = vkAcquireNextImageKHR(_device, _swapChain, UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) 
        {
            recreateSwapChain();
            return;
        }
        GUST_CORE_ASSERT("Failed to acquire swap chain image!", result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR);

        vkResetCommandBuffer(_commandBuffers[_currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        recordCommandBuffer(_commandBuffers[_currentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        
        VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[_currentFrame]};
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &_commandBuffers[_currentFrame];

        VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[_currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        result = vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[_currentFrame]);
        GUST_CORE_ASSERT("Failed to submit draw command buffer.", result != VK_SUCCESS);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { _swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;
        result = vkQueuePresentKHR(_presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferedResized)
        {
            _framebufferedResized = false;
            recreateSwapChain();
            return;
        }
        GUST_CORE_ASSERT("Failed to present swap chain image!", result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR);

        _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void WindowsWindow::waitDevice()
    {
        vkDeviceWaitIdle(_device);
    }

    void WindowsWindow::initVulkan() 
    {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageView();
        createRenderPass();
        createGraphicsPipeline();
        createFramebuffers();
        createCommandPool();
        createVertexBuffer();
        createCommandBuffers();
        createSyncObjects();
    }

    void WindowsWindow::createInstance() 
    {
        GUST_PROFILE_FUNCTION();
        if (!checkValidationLayerSupport() && enableValidationLayers)
        {
            GUST_CRITICAL("Validation layers requested but none available.");
        }

        //Optional setup but good for optimiation
        VkApplicationInfo appInfo{};
        //These sType's define the type of struct we are setting up.
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = _windowData.title.c_str();
        //Setup version
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
        appInfo.pEngineName = "Gust";
        //Setup version
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();

        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = dynamic_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugCreateInfo);
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);

        GUST_CORE_ASSERT("Vulkan failed to create an instance", result != VK_SUCCESS);
    }

    void WindowsWindow::setupDebugMessenger()
    {
        GUST_PROFILE_FUNCTION();
        if (!enableValidationLayers) 
        {
            return;
        }

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        VkResult result = createDebugUtilMessagerEXT(_instance, &createInfo, nullptr, &_debugMessenger);
        GUST_CORE_ASSERT("Vulkan failed to create Debug Messager Info", result != VK_SUCCESS);
    }

    void WindowsWindow::createSurface()
    {
        GUST_PROFILE_FUNCTION();

        VkResult result = glfwCreateWindowSurface(_instance, _window, nullptr, &_surface);

        GUST_CORE_ASSERT("Vulkan failed to create window surface", result != VK_SUCCESS);
    }

    void WindowsWindow::pickPhysicalDevice()
    {
        GUST_PROFILE_FUNCTION();
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

        GUST_CORE_ASSERT("Failed to find a GPU that support Vulkan", deviceCount == 0);

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

        for (const auto& device : devices) 
        {
            if (isDeviceSuitable(device)) 
            {
                _physicalDevice = device;
                break;
            }
        }

        GUST_CORE_ASSERT("Failed to find a suitable GPU", _physicalDevice != VK_NULL_HANDLE);
    }

    void WindowsWindow::createLogicalDevice() 
    {
        GUST_PROFILE_FUNCTION();

        QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
        float queuePriority = 1.f;
        for (uint32_t queueFamily : uniqueQueueFamilies) 
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) 
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else 
        {
            createInfo.enabledLayerCount = 0;
        }

        VkResult result = vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device);

        GUST_CORE_ASSERT("Failed to create logical device", result != VK_SUCCESS);

        vkGetDeviceQueue(_device,indices.graphicsFamily.value(), 0, &_graphicsQueue);
        vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQueue);
    }

    void WindowsWindow::createSwapChain()
    {
        GUST_PROFILE_FUNCTION();
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = _surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily) 
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else 
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        VkResult result = vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain);
        GUST_CORE_ASSERT("Failed to create swap chain.", result != VK_SUCCESS);

        vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
        _swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());

        _swapChainImageFormat = surfaceFormat.format;
        _swapChainExtent = extent;
    }

    void WindowsWindow::createImageView() 
    {
        GUST_PROFILE_FUNCTION();
        _swapChainImageViews.resize(_swapChainImages.size());

        for (size_t i = 0; i < _swapChainImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = _swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = _swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]);
            GUST_CORE_ASSERT("Failed to create image view", result != VK_SUCCESS);
        }
    }

    void WindowsWindow::createRenderPass() 
    {
        GUST_PROFILE_FUNCTION();
        VkAttachmentDescription colourAttachment{};
        colourAttachment.format = _swapChainImageFormat;
        colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colourAttachmentRef{};
        colourAttachmentRef.attachment = 0;
        colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colourAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colourAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkResult result = vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass);
        GUST_CORE_ASSERT("Failed to create render pass!", result != VK_SUCCESS);
    }

    void WindowsWindow::createGraphicsPipeline()
    {
        GUST_PROFILE_FUNCTION();
        auto vertexShaderCode = readFile("Assets/Shaders/simple_shader.vert.spv");
        auto fragmentShaderCode = readFile("Assets/Shaders/simple_shader.frag.spv");

        VkShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
        VkShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCode);

        VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
        vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderStageInfo.module = vertexShaderModule;
        vertexShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
        fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderStageInfo.module = fragmentShaderModule;
        fragmentShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] =
        {
            vertexShaderStageInfo,
            fragmentShaderStageInfo
        };

        auto bindingDescription = Vertex::getBindindDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colourBlendAttachment{};
        colourBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colourBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colourBlending{};
        colourBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colourBlending.logicOpEnable = VK_FALSE;
        colourBlending.logicOp = VK_LOGIC_OP_COPY;
        colourBlending.attachmentCount = 1;
        colourBlending.pAttachments = &colourBlendAttachment;
        colourBlending.blendConstants[0] = 0.f;
        colourBlending.blendConstants[1] = 0.f;
        colourBlending.blendConstants[2] = 0.f;
        colourBlending.blendConstants[3] = 0.f;

        std::vector<VkDynamicState> dynamicStates = 
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        VkResult result = vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout);
        GUST_CORE_ASSERT("Failed to create pipeline layout.", result != VK_SUCCESS);

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colourBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = _pipelineLayout;
        pipelineInfo.renderPass = _renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        result = vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline);
        GUST_CORE_ASSERT("Failed to create graphics pipeline.", result != VK_SUCCESS);

        vkDestroyShaderModule(_device, fragmentShaderModule, nullptr);
        vkDestroyShaderModule(_device, vertexShaderModule, nullptr);
    }

    void WindowsWindow::createFramebuffers()
    {
        _swapChainFramebuffers.resize(_swapChainImageViews.size());

        for (size_t i = 0; i < _swapChainImageViews.size(); i++)
        {
            VkImageView attachments[] = 
            {
                _swapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = _renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = _swapChainExtent.width;
            framebufferInfo.height = _swapChainExtent.height;
            framebufferInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]);
            GUST_CORE_ASSERT("Failed to create framebuffer", result != VK_SUCCESS);
        }
    }

    void WindowsWindow::createCommandPool() 
    {
        GUST_PROFILE_FUNCTION();
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(_physicalDevice);
        
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        VkResult result = vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool);
        GUST_CORE_ASSERT("Failed to create command pool", result != VK_SUCCESS);
    }

    void WindowsWindow::createVertexBuffer()
    {
        GUST_PROFILE_FUNCTION();

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data = nullptr;
        vkMapMemory(_device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(_device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);
        copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

        vkDestroyBuffer(_device, stagingBuffer, nullptr);
        vkFreeMemory(_device, stagingBufferMemory, nullptr);
    }

    void WindowsWindow::createCommandBuffers() 
    {
        GUST_PROFILE_FUNCTION();
        _commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = _commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

        VkResult result = vkAllocateCommandBuffers(_device, &allocateInfo, _commandBuffers.data());
        GUST_CORE_ASSERT("Failed to allocate command pool", result != VK_SUCCESS);
    }

    void WindowsWindow::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) 
    {
        GUST_PROFILE_FUNCTION();
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    VkShaderModule WindowsWindow::createShaderModule(const std::vector<char>& code)
    {
        GUST_PROFILE_FUNCTION();
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        VkResult result = vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule);
        GUST_CORE_ASSERT("Failed to create shader module!", result != VK_SUCCESS);

        return shaderModule;
    }

    VkSurfaceFormatKHR WindowsWindow::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
    {
        GUST_PROFILE_FUNCTION();
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR WindowsWindow::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) 
    {
        GUST_PROFILE_FUNCTION();
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D WindowsWindow::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        GUST_PROFILE_FUNCTION();
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
        {
            return capabilities.currentExtent;
        }
        else 
        {
            int width = 0;
            int height = 0;

            glfwGetFramebufferSize(_window, &width, &height);

            VkExtent2D actualExtent = 
            {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    SwapChainSupportDetails WindowsWindow::querySwapChainSupport(VkPhysicalDevice device)
    {
        GUST_PROFILE_FUNCTION();
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

        if (formatCount != 0) 
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) 
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    bool WindowsWindow::isDeviceSuitable(VkPhysicalDevice device) 
    {
        GUST_PROFILE_FUNCTION();
        //TODO: Have a smarter chosing process.

        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    QueueFamilyIndices WindowsWindow::findQueueFamilies(VkPhysicalDevice device)
    {
        GUST_PROFILE_FUNCTION();
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamilies : queueFamilies)
        {
            if (queueFamilies.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);

            if (presentSupport) 
            {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) 
            {
                break;
            }

            i++;
        }

        return indices;
    }

    std::vector<const char*> WindowsWindow::getRequiredExtensions() 
    {
        GUST_PROFILE_FUNCTION();
        //We need this because Vulkan is agnostic. So we need to get extentions available to it.
        uint32_t glfwExtentionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtentionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtentionCount);

        if (enableValidationLayers) 
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool WindowsWindow::checkValidationLayerSupport() 
    {
        GUST_PROFILE_FUNCTION();
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) 
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) 
            {
                if (strcmp(layerName, layerProperties.layerName) == 0) 
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) 
            {
                return false;
            }
        }

        return true;
    }

    void WindowsWindow::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(_device, &bufferInfo, nullptr, &buffer);
        GUST_CORE_ASSERT("Failed to create vertex buffer.", result != VK_SUCCESS);

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(_device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memRequirements.size;
        allocateInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        result = vkAllocateMemory(_device, &allocateInfo, nullptr, &bufferMemory);
        GUST_CORE_ASSERT("Failed to allocate vertex buffer memory.", result != VK_SUCCESS);

        vkBindBufferMemory(_device, buffer, bufferMemory, 0);
    }

    void WindowsWindow::copyBuffer(VkBuffer sourceBuffer, VkBuffer destBuffer, VkDeviceSize size)
    {
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = _commandPool;
        allocateInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(_device, &allocateInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, sourceBuffer, destBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(_graphicsQueue);

        vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);
    }

    bool WindowsWindow::checkDeviceExtensionSupport(VkPhysicalDevice device) 
    {
        GUST_PROFILE_FUNCTION();
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtension(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) 
        {
            requiredExtension.erase(extension.extensionName);
        }

        return requiredExtension.empty();
    }

    uint32_t WindowsWindow::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) 
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        GUST_CORE_ASSERT("Failed to find suitable memory type.", true);
    }

    void WindowsWindow::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) 
    {
        GUST_PROFILE_FUNCTION();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
        GUST_CORE_ASSERT("Failed to begin recording command buffer!", result != VK_SUCCESS);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = _renderPass;
        renderPassInfo.framebuffer = _swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = _swapChainExtent;

        VkClearValue clearColour = { { { 0.0f, 0.0f, 0.f, 1.f } } };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColour;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
        
        VkViewport viewport{};
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = static_cast<float>(_swapChainExtent.width);
        viewport.height = static_cast<float>(_swapChainExtent.height);
        viewport.minDepth = 0.f;
        viewport.maxDepth = 1.f;

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = _swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        VkBuffer vertexBuffers[] = { _vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
        vkCmdEndRenderPass(commandBuffer);

        result = vkEndCommandBuffer(commandBuffer);
        GUST_CORE_ASSERT("Failed to end recording command buffer.", result != VK_SUCCESS);
    }

    void WindowsWindow::createSyncObjects()
    {
        _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkResult result = vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]);
            GUST_CORE_ASSERT("Failed to create synchronisation objects for a frame.", result != VK_SUCCESS);

            result = vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]);
            GUST_CORE_ASSERT("Failed to create synchronisation objects for a frame.", result != VK_SUCCESS);

            result = vkCreateFence(_device, &fenceInfo, nullptr, &_inFlightFences[i]);
            GUST_CORE_ASSERT("Failed to create synchronisation objects for a frame.", result != VK_SUCCESS);
        }
    }

    void WindowsWindow::swapChainCleanUp()
    {
        for (auto framebuffer : _swapChainFramebuffers)
        {
            vkDestroyFramebuffer(_device, framebuffer, nullptr);
        }

        for (auto imageView : _swapChainImageViews)
        {
            vkDestroyImageView(_device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(_device, _swapChain, nullptr);
    }

    void WindowsWindow::recreateSwapChain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(_window, &width, &height);

        while (width == 0 || height == 0) 
        {
            glfwGetFramebufferSize(_window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(_device);

        swapChainCleanUp();

        createSwapChain();
        createImageView();
        createFramebuffers();
    }

    std::vector<char> WindowsWindow::readFile(const std::string& filename) 
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            GUST_CRITICAL("Shader file failed to open {0}", filename);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> fileBuffer(fileSize);

        file.seekg(0);
        file.read(fileBuffer.data(), fileSize);

        file.close();
        return fileBuffer;
    }

    void WindowsWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto app = reinterpret_cast<WindowsWindow*>(glfwGetWindowUserPointer(window));
        app->_framebufferedResized = true;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL WindowsWindow::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData) 
    {
        GUST_PROFILE_FUNCTION();

        switch (messageSeverity) 
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            GUST_ERROR("Validation Layer: {0}", callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            GUST_WARN("Validation Layer: {0}", callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            GUST_INFO("Validation Layer: {0}", callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            GUST_TRACE("Validation Layer: {0}", callbackData->pMessage);
            break;
        }

        return false;
    }
}