#ifndef ENTRY_POINT_HDR
#define ENTRY_POINT_HDR

#include "PreComp.h"

#include "Application.h"

//Currently only supports Windows.
#ifdef WIN32

//This function is global so the user can implement their own application 
//without having the write the main function. Although globals are ugly this 
//allows the engine to control how to main function is written.
extern Gust::Application* Gust::createApplication();

//Here set up the logging function. Then start then engine and run it.
//We also want to control how the life of the engine so it's not a 
//smart pointer.
int main(int /*argc*/, char** /*argv*/) 
{
    Gust::Log::init();
    GUST_PROFILE_BEGIN_SESSION("Startup", "TempestProfile_startup.json");
    auto app = Gust::createApplication();
    GUST_PROFILE_END_SESSION();
    GUST_PROFILE_BEGIN_SESSION("runtime", "TempestProfile_runtime.json");
    app->run();
    GUST_PROFILE_END_SESSION();
    GUST_PROFILE_BEGIN_SESSION("shutdown", "TempestProfile_shutdown.json");
    delete app;
    GUST_PROFILE_END_SESSION();
    return 0;
}

#endif

#endif // !ENTRY_POINT_HDR
