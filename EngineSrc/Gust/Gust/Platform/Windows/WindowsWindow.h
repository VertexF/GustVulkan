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
        void createDescriptionSetLayout();
        void createGraphicsPipeline();
        void createCommandPool();
        void createDepthResources();
        void createFramebuffers();
        void createTextureImage();
        void createTextureImageView();
        void createTextureSampler();
        void createVertexBuffer();
        void createIndexBuffer();
        void createUniformBuffers();
        void createDescriptorPool();
        void createDescriptorSets();
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

        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspectsFlags);

        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling  tiling, VkFormatFeatureFlags features);
        VkFormat findDepthFormat();
        bool hadStencilComponent(VkFormat format);

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommand(VkCommandBuffer commandBuffer);
        void copyBuffer(VkBuffer sourceBuffer, VkBuffer destBuffer, VkDeviceSize size);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        void updateUniformBuffer(uint32_t currentImage);
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
        VkDescriptorSetLayout _descriptorSetLayout;
        VkPipelineLayout _pipelineLayout;
        VkPipeline _graphicsPipeline;

        VkCommandPool _commandPool;

        VkImage _depthImage;
        VkDeviceMemory _depthImageMemory;
        VkImageView _depthImageView;

        VkImage _textureImage;
        VkDeviceMemory _textureImageMemory;
        VkImageView _textureImageView;
        VkSampler _textureSampler;

        VkBuffer _vertexBuffer;
        VkDeviceMemory _vertexBufferMemory;
        VkBuffer _indexBuffer;
        VkDeviceMemory _indexBufferMemory;

        std::vector<VkBuffer> _uniformBuffers;
        std::vector<VkDeviceMemory> _uniformBufferMemory;
        std::vector<void*> _uniformBufferMapped;
        std::vector<VkCommandBuffer> _commandBuffers;

        VkDescriptorPool _descriptorPool;
        std::vector<VkDescriptorSet> _descriptorSets;

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
