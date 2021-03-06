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

#include <string>

#include <QtCore/QEvent>

namespace rpeditor {

class RestAPIEvent: public QEvent
{
public:
    static const int EVENT_TYPE = QEvent::User + 1;

public:
    RestAPIEvent(const std::string& restapi_message);

    const std::string& get_message(void) const;

private:
    std::string restapi_message_;
};

// ************************************************************************************************

inline RestAPIEvent::RestAPIEvent(const std::string& restapi_message): QEvent(QEvent::Type(EVENT_TYPE)), restapi_message_(restapi_message)
{
}

inline const std::string& RestAPIEvent::get_message(void) const
{
    return restapi_message_;
}

}   // namespace rpeditor
