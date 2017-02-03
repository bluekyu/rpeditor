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

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>

#include "restapi/config.hpp"

namespace rpeditor {

class RestAPIClient: public QObject
{
    Q_OBJECT

public:
    explicit RestAPIClient(const QUrl& url, QObject* parent = nullptr);

    void write(const std::string& json_msg);

    void write(const rapidjson::Document& doc);

    void close(void);

Q_SIGNALS:
    void closed(void);

private Q_SLOTS:
    void on_connected(void);
    void on_message_received(QString message);

private:
    QWebSocket socket_;
    QUrl url_;
};

// ************************************************************************************************

inline void RestAPIClient::write(const std::string& json_msg)
{
    socket_.sendTextMessage(QString::fromStdString(json_msg));
}

}   // namespace rpeditor
