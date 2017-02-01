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
