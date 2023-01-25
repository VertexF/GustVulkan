#include "PreComp.h"
#include "Pipeline.h"

#include <fstream>

namespace Gust 
{
    Pipeline::Pipeline(const std::string& vertexString, const std::string& fragString)
    {
        createGraphicPipeline(vertexString, fragString);
    }

    std::vector<char> Pipeline::readFile(const std::string& filePath)
    {
        std::ifstream file { filePath, std::ios::ate | std::ios::binary };

        if (!file.is_open()) 
        {
            GUST_CRITICAL("Shader file failed to open {0}", filePath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> fileBuffer(fileSize);

        file.seekg(0);
        file.read(fileBuffer.data(), fileSize);

        file.close();
        return fileBuffer;
    }

    void Pipeline::createGraphicPipeline(const std::string& vertexString, const std::string& fragString)
    {
        //auto vertCode = readFile(vertexString);
        //auto fragCode = readFile(fragString);

        //GUST_INFO("Vertex Shader Code Size : {0}", vertCode.size());
        //GUST_INFO("Fragment Shader Code Size : {0}", fragCode.size());
    }
}