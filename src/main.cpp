#include <QApplication>

#include <boost/log/trivial.hpp>

#include "logging.hpp"
#include "main_window.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("bluekyu");
    app.setApplicationName("rpeditor");

    // run application
    rpeditor::MainWindow* window = rpeditor::MainWindow::get_instance();
    window->show();

    const int retcode = app.exec();

    BOOST_LOG_TRIVIAL(info) << "Exiting application.";

    return retcode;
}