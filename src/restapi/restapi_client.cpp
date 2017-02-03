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
