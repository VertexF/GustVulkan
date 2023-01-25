#ifndef CORE_HDR
#define CORE_HDR

#ifdef GUST_DEBUG
    #define GUST_CORE_ASSERT(x, ...) { if(!(x)) { GUST_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
    #define GUST_CORE_ASSERT(x, ...)
#endif

#endif // !CORE_HDR
