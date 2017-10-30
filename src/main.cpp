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

#include <QApplication>

#include "logger_manager.hpp"

#include "main_window.hpp"

void qt_logging_hander(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    std::string context_file, context_function;
    if (context.file)   context_file = context.file;
    if (context.function)   context_function = context.function;

    switch (type) {
    case QtDebugMsg:
        rpeditor::global_logger_->debug("({}:{}, {}) {}", context_file, context.line, context_function, msg.toStdString());
        break;

    case QtInfoMsg:
        rpeditor::global_logger_->info("({}:{}, {}) {}", context_file, context.line, context_function, msg.toStdString());
        break;

    case QtWarningMsg:
        rpeditor::global_logger_->warn("({}:{}, {}) {}", context_file, context.line, context_function, msg.toStdString());
        break;

    case QtCriticalMsg:
        rpeditor::global_logger_->error("({}:{}, {}) {}", context_file, context.line, context_function, msg.toStdString());
        break;

    case QtFatalMsg:
        rpeditor::global_logger_->critical("({}:{}, {}) {}", context_file, context.line, context_function, msg.toStdString());
        abort();
    }
}

int main(int argc, char *argv[])
{
    if (!rpeditor::LoggerManager::get_instance().is_created())
        rpeditor::LoggerManager::get_instance().create("rpeditor.log");

    qInstallMessageHandler(qt_logging_hander);

    QApplication app(argc, argv);

    app.setOrganizationName("bluekyu");
    app.setApplicationName("rpeditor");

    // run application
    rpeditor::MainWindow* window = rpeditor::MainWindow::get_instance();
    window->show();

    const int retcode = app.exec();

    rpeditor::global_logger_->info("Exiting application.");

    return retcode;
}
