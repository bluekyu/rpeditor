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

#include "logging.hpp"

#include <fstream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <QMessageLogContext>
#include <QString>

namespace rpeditor {

//! log foramt expression.
boost::log::formatter log_format = boost::log::expressions::stream << "[" << boost::log::trivial::severity << "] " << boost::log::expressions::smessage;

/*!
 * Qt Message Handler function.
 */
void qt_message_handler(QtMsgType msg_type, const QMessageLogContext& context, const QString& msg)
{
    std::string context_file, context_function;
    if (context.file)   context_file = context.file;
    if (context.function)   context_function = context.function;

    switch (msg_type) {
    case QtDebugMsg:
        BOOST_LOG_TRIVIAL(debug) << msg.toStdString() << " (" << context_file << context.line << context_function << ")";
        break;
    case QtWarningMsg:
        BOOST_LOG_TRIVIAL(warning) << msg.toStdString() << " (" << context_file << context.line << context_function << ")";
        break;
    case QtCriticalMsg:
        BOOST_LOG_TRIVIAL(error) << msg.toStdString() << " (" << context_file << context.line << context_function << ")";
        break;
    case QtFatalMsg:
        BOOST_LOG_TRIVIAL(fatal) << msg.toStdString() << " (" << context_file << context.line << context_function << ")";
        break;
    default:
        BOOST_LOG_TRIVIAL(debug) << msg.toStdString() << " (" << context_file << context.line << context_function << ")";
        break;
    }
}

void initialize_logging(void)
{
    // add file sink.
    boost::shared_ptr<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>> sink =
        boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>>();
    sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>("rpeditor.log"));
    sink->set_formatter(log_format);
    boost::log::core::get()->add_sink(sink);

    // TODO: set from command line or config file.
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);

    qInstallMessageHandler(qt_message_handler);

    BOOST_LOG_TRIVIAL(info) << "Start Logging";
}

}   // namespace rpeditor
