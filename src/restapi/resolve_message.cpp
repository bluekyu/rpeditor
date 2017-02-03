/**
 * RPEditor
 *
 * Copyright (c) 2017, Younguk Kim (bluekyu).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "restapi/resolve_message.hpp"

#include <iostream>

#include <boost/log/trivial.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

namespace rpeditor {

ResolverMapType& get_resolver_map(void)
{
    static ResolverMapType resolver_map;
    return resolver_map;
}

void resolve_message(const std::string& restapi_message)
{
    rapidjson::Document doc;
    rapidjson::ParseResult result = doc.Parse(restapi_message.c_str());

    if (!result)
    {
        BOOST_LOG_TRIVIAL(error) << "JSON parse error: " << rapidjson::GetParseError_En(result.Code());
        return;
    }

    if (!doc.IsObject())
    {
        BOOST_LOG_TRIVIAL(error) << "JSON Document is NOT object: " << restapi_message;
        return;
    }

    if (!doc.HasMember("resource"))
    {
        BOOST_LOG_TRIVIAL(error) << "Message has NOT 'resource': " << restapi_message;
        return;
    }

    const auto& resource = doc["resource"].GetString();
    auto& resolver_map = get_resolver_map();
    if (resolver_map.find(resource) != resolver_map.end())
    {
        resolver_map.at(resource)(doc);
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "NO resolver for the resource: " << resource;
    }
}

}   // namespace rpeditor
