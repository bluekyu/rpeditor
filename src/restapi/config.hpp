#pragma once

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace rpeditor {

#define RESTAPI_RESOURCE_STRING "resource"
#define RESTAPI_METHOD_STRING "method"
#define RESTAPI_METHOD_PUT_STRING "PUT"
#define RESTAPI_MESSAGE_STRING "message"

#define ConfigureStaticInit(name) \
    class StaticInit ## name \
    { \
    public: \
        StaticInit ## name (); \
    }; \
    static StaticInit ## name name_; \
    StaticInit ## name:: StaticInit ## name()

inline std::string StringfyDocument(const rapidjson::Document& dom)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    dom.Accept(writer);
    
    return std::string(buffer.GetString(), buffer.GetSize());
}

}   // namespace rpeditor
