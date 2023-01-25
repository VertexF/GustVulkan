#include <iostream>
#include <Gust.h>

#include "Gust/Core/EntryPoint.h"

#include "Game2D.h"

namespace game
{
    //The client uses the application as a template to create the game.
    class Game : public Gust::Application
    {
    public:
        Game()
        {
            pushLayer(new Game2D());
        }

        virtual ~Game() = default;
    };
}

//We kick start the engine by using our client class that is 
//inherited from the the engine.
Gust::Application* Gust::createApplication() 
{
    return new game::Game();
}