#ifndef INSTRUMENTOR_HDR
#define INSTRUMENTOR_HDR

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

namespace gust
{
    struct ProfileResult
    {
        std::string _name;
        long long _startTime, _endTime;
        uint32_t _threadID;
    };

    struct InstrumentationSession
    {
        std::string _name;
    };

    class Instrumentor
    {
    private:
        InstrumentationSession* _currentSession;
        std::ofstream _outputStream;
        int _profileCount;
    public:
        Instrumentor()
            : _currentSession(nullptr), _profileCount(0)
        {
        }

        void beginSession(const std::string& name, const std::string& filepath = "results.json")
        {
            _outputStream.open(filepath);
            writeHeader();
            _currentSession = new InstrumentationSession{ name };
        }

        void endSession()
        {
            writeFooter();
            _outputStream.close();
            delete _currentSession;
            _currentSession = nullptr;
            _profileCount = 0;
        }

        void writeProfile(const ProfileResult& result)
        {
            if (_profileCount++ > 0)
            {
                _outputStream << ",";
            }

            std::string name = result._name;
            std::replace(name.begin(), name.end(), '"', '\'');

            _outputStream << "{";
            _outputStream << "\"cat\":\"function\",";
            _outputStream << "\"dur\":" << (result._endTime - result._startTime) << ',';
            _outputStream << "\"name\":\"" << name << "\",";
            _outputStream << "\"ph\":\"X\",";
            _outputStream << "\"pid\":0,";
            _outputStream << "\"tid\":" << result._threadID << ",";
            _outputStream << "\"ts\":" << result._startTime;
            _outputStream << "}";

            _outputStream.flush();
        }

        void writeHeader()
        {
            _outputStream << "{\"otherData\": {},\"traceEvents\":[";
            _outputStream.flush();
        }

        void writeFooter()
        {
            _outputStream << "]}";
            _outputStream.flush();
        }

        static Instrumentor& get()
        {
            static Instrumentor instance;
            return instance;
        }
    };

    class InstrumentationTimer
    {
    public:
        InstrumentationTimer(const char* name) : _name(name), _stopped(false)
        {
            _timePoint = std::chrono::high_resolution_clock::now();
        }

        ~InstrumentationTimer()
        {
            if (_stopped == false)
            {
                stop();
            }
        }

        void stop()
        {
            auto endPoint = std::chrono::high_resolution_clock::now();
            auto start = std::chrono::time_point_cast<std::chrono::microseconds>(_timePoint).time_since_epoch().count();
            auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endPoint).time_since_epoch().count();

            uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
            Instrumentor::get().writeProfile({ _name, start, end, threadID });

            _stopped = true;
        }
    private:
        const char* _name;
        std::chrono::time_point<std::chrono::high_resolution_clock> _timePoint;
        bool _stopped;
    };
}

//#define GUST_PROFILE 1
#if GUST_PROFILE
    #define GUST_PROFILE_BEGIN_SESSION(name, filepath) ::Gust::Instrumentor::get().beginSession(name, filepath)
    #define GUST_PROFILE_END_SESSION() ::Gust::Instrumentor::get().endSession()
    #define GUST_PROFILE_SCOPE(name) ::Gust::InstrumentationTimer timer##__LINE__(name);
    #define GUST_PROFILE_FUNCTION() GUST_PROFILE_SCOPE(__FUNCSIG__)
#else
    #define GUST_PROFILE_BEGIN_SESSION(name, filepath)
    #define GUST_PROFILE_END_SESSION()
    #define GUST_PROFILE_SCOPE(name)
    #define GUST_PROFILE_FUNCTION()
#endif

#endif // !INSTRUMENTOR_HDR