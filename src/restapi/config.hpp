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

#pragma once

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>

namespace rpeditor {

#define RPEDITOR_API_RESOURCE_STRING "resource"
#define RPEDITOR_API_METHOD_STRING "method"
#define RPEDITOR_API_MESSAGE_STRING "message"

#define RPEDITOR_API_CREATE_STRING "CREATE"
#define RPEDITOR_API_READ_STRING "READ"
#define RPEDITOR_API_UPDATE_STRING "UPDATE"

#define ConfigureStaticInit(name) \
    class StaticInit ## name \
    { \
    public: \
        StaticInit ## name (); \
    }; \
    static StaticInit ## name name_; \
    StaticInit ## name:: StaticInit ## name()


/**
 * @return 'message' JSON object.
 */
inline rapidjson::Value& initialize_api_document(rapidjson::Document& doc, const std::string& resource, const std::string& method)
{
    auto& allocator = doc.GetAllocator();
    doc.SetObject();

    doc.AddMember(RPEDITOR_API_RESOURCE_STRING, resource, allocator);
    doc.AddMember(RPEDITOR_API_METHOD_STRING, method, allocator);
    doc.AddMember(RPEDITOR_API_MESSAGE_STRING, rapidjson::Value(rapidjson::kObjectType).Move(), allocator);
    return doc[RPEDITOR_API_MESSAGE_STRING];
}

}   // namespace rpeditor
