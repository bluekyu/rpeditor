#pragma once

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace rpeditor {
namespace restapi {

#define RESTAPI_RESOURCE_STRING "resource"
#define RESTAPI_METHOD_STRING "method"
#define RESTAPI_METHOD_PUT_STRING "PUT"
#define RESTAPI_MESSAGE_STRING "message"

inline std::string StringfyDocument(const rapidjson::Document& dom)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    dom.Accept(writer);
    
    return std::string(buffer.GetString(), buffer.GetSize());
}

}   // namespace restapi
}   // namespace rpeditor
