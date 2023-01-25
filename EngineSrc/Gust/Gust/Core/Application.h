#ifndef APPLICATION_HDR
#define APPLICATION_HDR

#include "PreComp.h"
#include "Window.h"
#include "Gust/Events/Event.h"
#include "Gust/Events/ApplicationEvents.h"
#include "LayerStack.h"

#include "TimeStep.h"

namespace Gust
{
    class Layer;

    //This is the main coordinator class.
    class Application
    {
    public:
        Application(const std::string& title = "Gust Engine");
        virtual ~Application();

        void onEvent(Event &e);
        void run();

        //Used to layering stuff that needs be layered on top of each other.
        void pushLayer(Layer *layer);
        void pushOverlay(Layer *layer);
        void popLayer(Layer* layer);
        void popOverlay(Layer* layer);

        inline static Application& get() { return *_instance; }
        inline Window& getWindow() const { return *_window;  }

        void close();
    private:
        bool onWindowClosed(WindowClosedEvent &closed);
        bool onWindowResize(WindowResizeEvent& resized);
    private:
        bool _running;
        bool _minimized = false;
        std::unique_ptr<Window> _window;
        //Used for things like ImGuiLayers that need to be on top of every layer.
        LayerStack _layerStack;

        float _lastFrameTime  = 0.f;
        static Application* _instance;
    };

    //To be defined in the client. This is how you use the shared library.
    Application* createApplication();
}

#endif // TEST_HDR
