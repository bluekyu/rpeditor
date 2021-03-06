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

#include "main_window.hpp"
#include "logger_manager.hpp"

namespace rpeditor {

bool resolve_nodepath(const rapidjson::Document& doc)
{
    const std::string& method = doc[RPEDITOR_API_METHOD_STRING].GetString();
    if (method == RPEDITOR_API_UPDATE_STRING)
    {
        rpeditor::MainWindow::get_instance()->update_nodepath(doc[RPEDITOR_API_MESSAGE_STRING]);
    }
    else
    {
        global_logger_->error("Unknown method: {}", method);
        return false;
    }

    return true;
}

// ************************************************************************************************

ConfigureStaticInit(NodePath)
{
    auto& resolver_map = get_resolver_map();
    resolver_map["NodePath"] = resolve_nodepath;
}

}	// namespace rpeditor
