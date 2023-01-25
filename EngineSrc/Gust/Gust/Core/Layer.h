#ifndef LAYER_HDR
#define LAYER_HDR

#include "PreComp.h"
#include "Gust/Events/Event.h"

#include "TimeStep.h"

namespace Gust 
{
    //Every thing in the engine is layered. 
    //Each layer contains away to detach and attach itself.
    class Layer 
    {
    public:
        Layer(const std::string& layerName = "Layer") : _debugName(layerName) {}
        virtual ~Layer() = default;

        virtual void onAttach() = 0;
        virtual void onDetach() = 0;
        virtual void onUpdate(TimeStep timeStep) = 0;
        virtual void onEvent(Event& e) = 0;
        virtual void onImGuiRender() = 0;
        virtual bool isFinished() const = 0;

        //Only really used for debugging and shouldn't be used in release.
        inline std::string getLayer() const { return _debugName; }
    private:
        std::string _debugName;
    };
}


#endif //LAYER_HDR