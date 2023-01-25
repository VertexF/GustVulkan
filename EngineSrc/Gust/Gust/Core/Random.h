#ifndef RANDOM_HDR
#define RANDOM_HDR

#include <random>

class Random 
{
public:
    static void init() 
    {
        _randomEngine.seed(std::random_device()());
    }

    static float getFloat() 
    {
        return static_cast<float>(_distribution(_randomEngine)) / static_cast<float>(std::numeric_limits<uint32_t>::max());
    }
private:
    static std::mt19937 _randomEngine;
    static std::uniform_int_distribution<std::mt19937::result_type> _distribution;
};

#endif // !RANDOM_HDR
