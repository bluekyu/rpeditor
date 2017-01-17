#include "main_window.hpp"
#include "ui_main_window.h"

#include <string>
#include <stdexcept>

#include <QMessageBox>
#include <QApplication>
#include <QProgressBar>
#include <QTreeWidget>
#include <QFileDialog>
#include <QStandardPaths>

#include <boost/log/trivial.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>

#include "options_dialog.hpp"
#include "restapi_client.hpp"

namespace rpeditor {

static const char* SETTING_FILE_NAME = "config.json";

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui_(new Ui::MainWindow)
{
    BOOST_LOG_TRIVIAL(debug) << "Initializing MainWindow ...";

    ui_->setupUi(this);

    // logging console
    logging_sink.reset(new sink_t(boost::shared_ptr<LoggingConsoleBackend>(new LoggingConsoleBackend(ui_->logging_console))));
    logging_sink->set_formatter(log_format);
    boost::log::core::get()->add_sink(logging_sink);

    setup_ui();
}

MainWindow::~MainWindow()
{
    BOOST_LOG_TRIVIAL(debug) << "Destructing MainWindow ...";

    boost::log::core::get()->remove_sink(logging_sink);
}

void MainWindow::update_ui(void)
{
//    ui_->logging_console->setMaximumBlockCount(settings->properties.window.console_max_line);
//    resize(settings->properties.window.width, settings->properties.window.height);
//
//    ui_->action_save_project->setEnabled(bool(project));
//    ui_->action_save_project_as->setEnabled(bool(project));
//    if (project)
//    {
//        setWindowTitle(QString("rpeditor - ").append(QString::fromStdString(project->get_name())));
//    }
//    else
    {
        setWindowTitle(QApplication::instance()->applicationName());
    }
}

void MainWindow::update_scenegraph_tree(void)
{
    // TODO: implement
    BOOST_LOG_TRIVIAL(debug) << "Update scenegraph in tree widget.";
}

void MainWindow::closeEvent(QCloseEvent* ev)
{
    if (restapi_client_)
    {
        BOOST_LOG_TRIVIAL(debug) << "Waiting for closing RESTAPI client ...";
        restapi_client_->close();
    }

    if (restapi_network_thread_ && restapi_network_thread_->joinable())
    {
        restapi_network_thread_->join();
        restapi_network_thread_.reset();
    }

    save_settings();

    ev->accept();
}

bool MainWindow::event(QEvent* ev)
{
    switch (ev->type())
    {
    case EventType::UPDATE_SCENEGRAPH_TREE:
        update_scenegraph_tree();
        break;

    default:
        return QMainWindow::event(ev);
    }

    return true;
}

bool MainWindow::load_settings(void)
{
    boost::property_tree::ptree settings;

    const QString& settings_file_path = QStandardPaths::locate(QStandardPaths::StandardLocation::AppConfigLocation, SETTING_FILE_NAME, QStandardPaths::LocateFile);

    if (settings_file_path.isEmpty())
    {
        BOOST_LOG_TRIVIAL(debug) << "No '" << SETTING_FILE_NAME << "' file.";

        const QString& config_dir_path =  QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppConfigLocation);
        if (config_dir_path.isEmpty())
        {
            BOOST_LOG_TRIVIAL(error) << "Cannot find location to write configuration files.";
        }
        else
        {
            boost::filesystem::path config_path(config_dir_path.toStdString());
            config_dir_path_ = config_path.generic_string();
        }
    }
    else
    {
        config_dir_path_ = boost::filesystem::path(settings_file_path.toStdString()).parent_path().generic_string();

        try
        {
            const std::string& settings_file_path = get_settings_file_path();

            boost::property_tree::read_json(settings_file_path, settings);

            BOOST_LOG_TRIVIAL(info) << "Settings loaded: " << settings_file_path;
        }
        catch (const std::exception& e)
        {
            BOOST_LOG_TRIVIAL(error) << "Cannot read setting file: " << get_settings_file_path();
            BOOST_LOG_TRIVIAL(error) << "Error message: " << e.what();
            return false;
        }
    }

    // TODO: update UI

    return true;
}

bool MainWindow::save_settings(void)
{
    // TODO: save UI
    boost::property_tree::ptree settings;

    boost::filesystem::path settings_file_path(get_settings_file_path());
    if (settings_file_path.empty())
    {
        BOOST_LOG_TRIVIAL(error) << "Cannot find location to write setting file.";
        return false;
    }

    try
    {
        boost::property_tree::write_json(settings_file_path.generic_string(), settings, std::locale());
    }
    catch (const std::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << "Cannot write setting file: " << settings_file_path;
        BOOST_LOG_TRIVIAL(error) << "Error message: " << e.what();
        return false;
    }

    BOOST_LOG_TRIVIAL(info) << "Settings saved: " << settings_file_path;

    return true;
}

std::string MainWindow::get_settings_file_path(void) const
{
    if (config_dir_path_.empty())
        return "";

    return (boost::filesystem::path(config_dir_path_) / "config.json").generic_string();
}

void MainWindow::setup_ui(void)
{
//    progress_bar = new QProgressBar(this);
//    progress_bar->setRange(0, 100);
//    ui_->status_bar->addPermanentWidget(progress_bar);
//
//    setup_project_tree(core::project_dir_path.string(), nullptr);

    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);    // dock widget corner

//    ui_->action_new_project->setShortcuts(QKeySequence::New);
//
//    ui_->action_save_project->setShortcuts(QKeySequence::Save);
//    connect(ui_->action_save_project, &QAction::triggered, [this](){
//        // TODO: check dirty.
//        save_project();
//    });
//
//    ui_->action_save_project_as->setShortcuts(QKeySequence::SaveAs);
//    connect(ui_->action_save_project_as, &QAction::triggered, [this]() {
//        save_project(
//            QFileDialog::getSaveFileName(this, "Save Project",
//                QString::fromStdString(core::project_dir_path.generic_string()), "JSON (*.json)"
//            ).toStdString()
//        );
//    });

    ui_->action_exit->setShortcuts(QKeySequence::Quit);

    connect(ui_->action_run_renderer, &QAction::triggered, this, &MainWindow::connect_restapi_server);

//    ui_->action_options->setShortcuts(QKeySequence::Preferences);
//    connect(ui_->action_options, &QAction::triggered, [this]() {
//        OptionsDialog option_dialog(this);
//        if (option_dialog.exec()) {
//            update_ui();
//        }
//    });

    connect(ui_->action_about_qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui_->action_about_application, &QAction::triggered, this, &MainWindow::about_application);

//    connect(ui_->project_tree, &QTreeWidget::itemActivated, [this](const QTreeWidgetItem* item, int column) {
//        if (item->type() == int(ProjectTreeItemType::DIRECTORY))
//            return;
//
//        boost::filesystem::path project_path = item->text(0).toStdString();
//        while (item = item->parent())
//        {
//            project_path = boost::filesystem::path(item->text(0).toStdString()) / project_path;
//        }
//        load_project((core::project_dir_path / project_path).string());
//    });

    load_settings();
    update_ui();

    ui_->status_bar->showMessage("Ready");
}

void MainWindow::connect_restapi_server(void)
{
    BOOST_LOG_TRIVIAL(debug) << "Creating RESTAPI network thread ...";

    restapi_network_thread_ = std::make_shared<std::thread>([this](void) {

        BOOST_LOG_TRIVIAL(debug) << "RESTAPI network thread will start ...";

        try
        {
            boost::asio::io_service io_service;
            boost::asio::ip::tcp::resolver resolver(io_service);
            auto endpoint_iterator = resolver.resolve({ "127.0.0.1", "8888" });

            restapi_client_ = std::make_shared<RestAPIClient>(io_service, endpoint_iterator);

            io_service.run();

            restapi_client_.reset();
        }
        catch (std::exception& e)
        {
            BOOST_LOG_TRIVIAL(error) << "RESTAPI network thread occurs exception: " << e.what();
        }

        BOOST_LOG_TRIVIAL(debug) << "RESTAPI network thread is done!";
    });
}

void MainWindow::about_application(void)
{
    QMessageBox::about(this, QString("About ") + QApplication::instance()->applicationName(), "");
}

}   // namespace rpeditor
