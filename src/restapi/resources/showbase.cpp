#include "restapi/resources/showbase.hpp"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>

#include "restapi/config.hpp"

namespace rpeditor {
namespace restapi {

#define RESOURCE_PREFIX "showbase"

std::string ShowBase::put_model(const std::string& file_path)
{
    rapidjson::Document dom;
    auto& allocator = dom.GetAllocator();

    dom.SetObject();
    dom.AddMember(RESTAPI_RESOURCE_STRING, RESOURCE_PREFIX "/model", allocator);
    dom.AddMember(RESTAPI_METHOD_STRING, RESTAPI_METHOD_PUT_STRING, allocator);

    rapidjson::Value message(rapidjson::kObjectType);
    message.AddMember("path", file_path, allocator);

    dom.AddMember(RESTAPI_MESSAGE_STRING, message, allocator);

    return StringfyDocument(dom);
}

}   // namespace restapi
}   // namespace rpeditor
