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
