#ifndef PIPELINE_HDR
#define PIPELINE_HDR

#include "PreComp.h"

namespace Gust 
{
    class Pipeline 
    {
    public:
        Pipeline(const std::string& vertexString, const std::string& fragString);
    private:
        static std::vector<char> readFile(const std::string& filePath);

        void createGraphicPipeline(const std::string& vertexString, const std::string& fragString);
   };
}

#endif // !PIPELINE_HDR
