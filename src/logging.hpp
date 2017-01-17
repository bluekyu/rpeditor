#pragma once

#include <QPlainTextEdit>

#include <boost/log/sinks/basic_sink_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace rpeditor {

extern boost::log::formatter log_format;

void initialize_logging(void);

class LoggingConsoleBackend: public boost::log::sinks::basic_formatted_sink_backend<char>
{
public:
    LoggingConsoleBackend(QPlainTextEdit* logging_console): logging_console(logging_console) {}

    void consume(boost::log::record_view const& rec, string_type const& message)
    {
        logging_console->appendPlainText(message.c_str());
    }

private:
    QPlainTextEdit* logging_console;
};

}   // namespace rpeditor
