#include "restapi/resolve_message.hpp"

#include <iostream>
#include <unordered_map>
#include <functional>

#include <boost/log/trivial.hpp>

#include <rapidjson/document.h>

namespace rpeditor {
namespace restapi {

std::unordered_map<std::string, std::function<void(const rapidjson::Document&)>> resolver_map ={
    //{ "CRModel/TWorld", ResolveTWorld },
};

void resolve_message(const std::string& restapi_message)
{
    rapidjson::Document dom;
    dom.Parse(restapi_message.c_str());

    const auto& resource = dom["resource"].GetString();
    if (resolver_map.find(resource) != resolver_map.end())
    {
        resolver_map.at(resource)(dom);
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "NO resolver for the resource: " << resource << std::endl;
    }
}

}   // namespace restapi
}   // namespace rpeditor
