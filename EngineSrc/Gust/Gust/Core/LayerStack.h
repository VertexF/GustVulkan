#ifndef LAYER_STACK_HDR
#define LAYER_STACK_HDR

#include "PreComp.h"

namespace Gust 
{
    class Layer;

    //This is a wrapper around a vector. This will handle navigation of
    // the layers.
    class LayerStack 
    {
    public:
        LayerStack();
        ~LayerStack();

        void pushLayer(Layer *layer);
        void pushOverlay(Layer* overlay);
        void popLayer(Layer* layer);
        void popOverlay(Layer* overlay);

        void attachTopLayer();

        std::vector<Layer*>::iterator begin() { return _layers.begin(); }
        std::vector<Layer*>::iterator end() { return _layers.end(); }

        std::vector<Layer*>::reverse_iterator rbegin() { return _layers.rbegin(); }
        std::vector<Layer*>::reverse_iterator rend() { return _layers.rend(); }

        bool isEmpty() const { return _layers.empty(); }
        Layer* back();
    private:
        std::vector<Layer*> _layers;
        uint32_t _layerIteratorIndex = 0;
    };
}

#endif // !LAYER_STACK_HDR
