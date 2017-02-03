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

#include "restapi_client.hpp"

#include <QtCore/QCoreApplication>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "main_window.hpp"
#include "restapi_event.hpp"

namespace rpeditor {

RestAPIClient::RestAPIClient(const QUrl& url, QObject* parent): QObject(parent), url_(url)
{
    connect(&socket_, &QWebSocket::connected, this, &RestAPIClient::on_connected);
    connect(&socket_, &QWebSocket::disconnected, this, &RestAPIClient::closed);
    socket_.open(QUrl(url_));
}

void RestAPIClient::write(const rapidjson::Document& doc)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    write(std::string(buffer.GetString(), buffer.GetSize()));
}

void RestAPIClient::close(void)
{
    socket_.close();
}

void RestAPIClient::on_connected(void)
{
    connect(&socket_, &QWebSocket::textMessageReceived, this, &RestAPIClient::on_message_received);
}

void RestAPIClient::on_message_received(QString message)
{
    QCoreApplication::postEvent(MainWindow::get_instance(), new RestAPIEvent(message.toStdString()));
}

}   // namespace rpeditor
