#include "Application.h"

#include "PreComp.h"

#include "Gust/Events/ApplicationEvents.h"
#include "Gust/Events/KeyEvents.h"
#include "Gust/Events/MouseEvents.h"
#include "Gust/Events/Event.h"
#include "Layer.h"
#include "Input.h"

#include <GLFW/glfw3.h>

namespace Gust
{
    Application* Application::_instance = nullptr;

    //First we intialise the window which sets up all the stuff needed to run.
    //Then we set up callback functions to the on event function in this class.
    //This allows events to be sent GLFW from our onEvent function.
    Application::Application(const std::string& title)
    {
        GUST_PROFILE_FUNCTION();

        _instance = this;

        _window = Window::create(WindowProps(title));
        _window->setCallbackFunction(std::bind(&Application::onEvent, this, std::placeholders::_1));

        //Renderer::init();
    }

    Application::~Application()
    {
        GUST_PROFILE_FUNCTION();

        //Renderer::shutdown();
    }

    //The only event we need to dispatch is our windows close event.
    //This sends out a signal, that goes through the dispatcher and gets handled
    //by running the WindowClosedEvent function as a callback.
    void Application::onEvent(Event& e)
    {
        GUST_PROFILE_FUNCTION();

        EventDispatcher eventDispatcher(e);
        eventDispatcher.dispatch<WindowClosedEvent>(std::bind(&Application::onWindowClosed, this, std::placeholders::_1));
        eventDispatcher.dispatch<WindowResizeEvent>(std::bind(&Application::onWindowResize, this, std::placeholders::_1));

        //This is meant to go in reserve to handle events like keypresses.
        for (auto it = _layerStack.rbegin(); it != _layerStack.rend(); it++)
        {
            //When the event is handled we don't need to continue looping
            //through the layer stack.
            if (e.isHandled)
            {
                break;
            }

            (*it)->onEvent(e);
        }
    }

    //The main loop function. Every layer need to be updated in order so we 
    //render layers on top of each other correctly.
    void Application::run()
    {
        GUST_PROFILE_FUNCTION();

        _running = true;
        while (_running)
        {
            GUST_PROFILE_SCOPE("Main run loop");

            float time = static_cast<float>(glfwGetTime());
            TimeStep timestep(time - _lastFrameTime);
            _lastFrameTime = time;

            if (_minimized == false)
            {
                {
                    GUST_PROFILE_SCOPE("Layer stack update");
                    for (Layer* layer : _layerStack)
                    {
                        layer->onUpdate(timestep);
                    }
                }
            }

            _window->onUpdate();
        }

        _window->waitDevice();
    }

    //This is what actually gets ran in the event dispatcher when the signal 
    //is sent on the event of a closed window.
    bool Application::onWindowClosed(WindowClosedEvent& closed)
    {
        _running = false;
        return true;
    }

    bool Application::onWindowResize(WindowResizeEvent& resized)
    {
        GUST_PROFILE_FUNCTION();

        if (resized.getWidth() == 0 || resized.getHeight() == 0)
        {
            _minimized = true;
            return false;
        }

        //Renderer::onResizeEvent(resized.getWidth(), resized.getHeight());

        _minimized = false;
        return false;
    }

    void Application::pushLayer(Layer* layer)
    {
        GUST_PROFILE_FUNCTION();

        _layerStack.pushLayer(layer);
    }

    void Application::pushOverlay(Layer* layer)
    {
        GUST_PROFILE_FUNCTION();

        _layerStack.pushOverlay(layer);
    }

    void Application::popLayer(Layer* layer)
    {
        GUST_PROFILE_FUNCTION();

        _layerStack.popLayer(layer);
    }

    void Application::popOverlay(Layer* layer) 
    {
        GUST_PROFILE_FUNCTION();

        _layerStack.popOverlay(layer);
    }

    void Application::close()
    {
        _running = false;
    }
}