#ifndef WINDOWS_WINDOW_HDR
#define WINDOWS_WINDOW_HDR

#include "PreComp.h"

//Needed to make the GLEW linking is linking to the static version.
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Gust/Core/Window.h"

namespace 
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails 
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
}

namespace Gust 
{
    class GraphicsContext;

    //This is the Windows OS windo versoin.
    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(const WindowProps &props);
        virtual ~WindowsWindow();

        virtual void onUpdate() override;

        uint32_t getWidth() const override;
        uint32_t getHeight() const override;

        //Window attributes.
        void setCallbackFunction(const EventCallbackFunc& callback) override;
        virtual void setVSync(bool vsync) override;
        virtual bool isVSync() const override;

        virtual void* getNativeWindow() const override;

        void drawFrame();
        virtual void waitDevice() override;

    private:
        virtual void init(const WindowProps& props);
        virtual void shutdown();

        //Vulkan validation set up.
        void initVulkan();

        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        void createImageView();
        void createRenderPass();
        void createGraphicsPipeline();
        void createFramebuffers();
        void createCommandPool();
        void createVertexBuffer();
        void createCommandBuffers();
        void createSyncObjects();

        void swapChainCleanUp();
        void recreateSwapChain();

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        VkShaderModule createShaderModule(const std::vector<char>& code);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        bool isDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        std::vector<const char*> getRequiredExtensions();
        bool checkValidationLayerSupport();
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

        static std::vector<char> readFile(const std::string& filename);

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData);
        bool _framebufferedResized = false;
    private:
        GLFWwindow *_window;
        GraphicsContext *_context;

        VkInstance _instance;
        VkDebugUtilsMessengerEXT _debugMessenger;
        VkSurfaceKHR _surface;

        VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
        VkDevice _device;

        VkQueue _graphicsQueue;
        VkQueue _presentQueue;

        VkSwapchainKHR _swapChain;
        std::vector<VkImage> _swapChainImages;
        VkFormat _swapChainImageFormat;
        VkExtent2D _swapChainExtent;
        std::vector<VkImageView> _swapChainImageViews;
        std::vector<VkFramebuffer> _swapChainFramebuffers;

        VkRenderPass _renderPass;
        VkPipelineLayout _pipelineLayout;
        VkPipeline _graphicsPipeline;

        VkCommandPool _commandPool;

        VkBuffer _vertexBuffer;
        VkDeviceMemory _vertexBufferMemory;

        std::vector<VkCommandBuffer> _commandBuffers;

        std::vector<VkSemaphore> _imageAvailableSemaphores;
        std::vector<VkSemaphore> _renderFinishedSemaphores;
        std::vector<VkFence> _inFlightFences;
        uint32_t _currentFrame = 0;

        //This structure allow use to pass in the window data to GLFW
        //without the need to pass in the WindowsWindow class meaning we can
        //cut down on how much data is being handled by GLFW internally.
        struct WindowData 
        {
            std::string title;
            uint32_t width;
            uint32_t height;
            bool vSync;

            //This allows use to use function callback to events.
            EventCallbackFunc eventCallback;
        };

        WindowData _windowData;
    };
}

#endif // WINDOWS_WINDOW_HDR
