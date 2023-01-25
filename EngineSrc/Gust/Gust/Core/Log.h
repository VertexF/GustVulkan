#ifndef LOG_HDR
#define LOG_HDR

#include "PreComp.h"

//This class sets up the logger to initalised to have the format we want.
namespace Gust
{
    class Log
    {
    public:
        static void init();

        static inline std::shared_ptr<spdlog::logger>& getLogger() { return _logger; }

    private:
        static std::shared_ptr<spdlog::logger> _logger;
    };
}

//These defined used to run the logger at different level. While giving the 
//befinite of being able to be stripped on release builds.
#define GUST_CRITICAL(...) Gust::Log::getLogger()->critical(__VA_ARGS__)
#define GUST_ERROR(...)    Gust::Log::getLogger()->error(__VA_ARGS__)
#define GUST_WARN(...)     Gust::Log::getLogger()->warn(__VA_ARGS__)
#define GUST_INFO(...)     Gust::Log::getLogger()->info(__VA_ARGS__)
#define GUST_TRACE(...)    Gust::Log::getLogger()->trace(__VA_ARGS__)

#endif // !LOG_HDR
