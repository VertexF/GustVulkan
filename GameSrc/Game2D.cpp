#include "PreComp.h"
#include "Game2D.h"

#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <vector>

namespace game
{
    Game2D::Game2D() : Layer("Vulkan Game Engine")
    {
        GUST_PROFILE_FUNCTION();
    }

    void Game2D::onAttach()
    {
        GUST_PROFILE_FUNCTION();
    }

    void Game2D::onDetach()
    {
        GUST_PROFILE_FUNCTION();
    }

    void Game2D::onUpdate(Gust::TimeStep timeStep)
    {
        GUST_PROFILE_FUNCTION();
    }

    void Game2D::onEvent(Gust::Event& e)
    {
        Gust::EventDispatcher dispatcher(e);
        dispatcher.dispatch<Gust::PressedKeyEvent>(std::bind(&Game2D::onKeyPressed, this, std::placeholders::_1));
        dispatcher.dispatch<Gust::ReleasedKeyEvent>(std::bind(&Game2D::onKeyReleased, this, std::placeholders::_1));
    }

    void Game2D::onImGuiRender()
    {
        GUST_PROFILE_FUNCTION();
    }

    bool Game2D::isFinished() const
    {
        return false;
    }

    bool Game2D::onKeyPressed(Gust::PressedKeyEvent& e)
    {
        return false;
    }

    bool Game2D::onKeyReleased(Gust::ReleasedKeyEvent& e) 
    {
        return false;
    }
}
