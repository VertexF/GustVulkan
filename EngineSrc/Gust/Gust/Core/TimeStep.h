#ifndef TIME_STEP_HDR
#define TIME_STEP_HDR

namespace Gust 
{
    class TimeStep 
    {
    public:
        TimeStep(float time = 0.f) :
            _time(time)
        {
        }

        operator float() const { return _time; }

        float getSeconds() const { return _time; }
        float getMilliseconds() const { return _time * 1000.f; }
    private:
        float _time;
    };
}

#endif // !TIME_STEP_HDR
