#ifndef GAME_2D_HDR
#define GAME_2D_HDR

#include <Gust.h>

#include "Gust/Events/Event.h"
#include "Gust/Events/KeyEvents.h"
#include "Gust/Core/Layer.h"
#include "Gust/Core/TimeStep.h"

#include "Gust/Renderer/Pipeline.h"

namespace game
{
    class Game2D : public Gust::Layer
    {
    public:
        Game2D();
        virtual ~Game2D() = default;

        virtual void onAttach() override;
        virtual void onDetach() override;
        virtual void onUpdate(Gust::TimeStep timeStep) override;
        virtual void onEvent(Gust::Event& e) override;
        virtual void onImGuiRender() override;
        virtual bool isFinished() const override;

    private:
        bool onKeyPressed(Gust::PressedKeyEvent& e);
        bool onKeyReleased(Gust::ReleasedKeyEvent& e);
    private:
        float _posX;
        float _posY;
    };
}

#endif //!GAME_2D_HDR