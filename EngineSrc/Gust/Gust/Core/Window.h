#ifndef WINDOW_HDR
#define WINDOW_HDR

#include "PreComp.h"

#include "Gust/Events/Event.h"
#include "Core.h"

namespace Gust 
{
    //This structure holds the window properties of the window 
    struct WindowProps
    {
        std::string title;
        uint32_t width;
        uint32_t height;

        WindowProps(const std::string title = "Gust Engine",
            uint32_t w = 1280,
            uint32_t h = 720) :
            title(title), width(w), height(h)
        {
        }
    };

    //This is just an interface class for how you should write platform windowing.
    class Window 
    {
    public:
        using EventCallbackFunc = std::function<void(Event&)>;

        virtual ~Window() = default;

        virtual void onUpdate() = 0;

        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;

        //Window attributes.
        virtual void setCallbackFunction(const EventCallbackFunc &callback) = 0;
        virtual void setVSync(bool vsync) = 0;
        virtual bool isVSync() const = 0;

        virtual void *getNativeWindow() const = 0;

        virtual void waitDevice() = 0;

        //As every window need to be create with properties this static
        //function should be implemented on the platform specific class.
        static std::unique_ptr<Window> create(const WindowProps& props = WindowProps());
    };
}

#endif // !WINDOW_HDR
