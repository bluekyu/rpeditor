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

#include "main_window.hpp"
#include "ui_main_window.h"

#include <string>
#include <stdexcept>

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QApplication>
#include <QtWidgets/QProgressBar>
#include <QtCore/QStandardPaths>
#include <QtGui/QCloseEvent>

#include <boost/log/trivial.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>

#if defined(BOOST_OS_WINDOWS)
#include <Windows.h>
#endif

#include "restapi/restapi_client.hpp"
#include "restapi/restapi_event.hpp"
#include "restapi/resolve_message.hpp"

namespace rpeditor {

static const char* SETTING_FILE_NAME = "config.json";

void update_scenegraph_subtree(QTreeWidgetItem* parent, const rapidjson::Value& current)
{
    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText(0, current["name"].GetString());

    parent->addChild(item);

    if (current["children"].IsArray())
    {
        for (const auto& child: current["children"].GetArray())
            update_scenegraph_subtree(item, child);
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Invalid API specification.";
    }
}

// ************************************************************************************************

bool MainWindow::is_shown_system_console_ = false;
bool MainWindow::is_created_system_console_ = false;

bool MainWindow::toggle_system_console(void)
{
#if defined(BOOST_OS_WINDOWS)
    if (GetConsoleWindow())
    {
        return is_shown_system_console_ = !ShowWindow(GetConsoleWindow(), is_shown_system_console_ ? SW_HIDE : SW_RESTORE);
    }
    else
    {
        if (!AllocConsole())
        {
            BOOST_LOG_TRIVIAL(error) << "Cannot allocate console.";
            return is_shown_system_console_ = false;
        }

        if (AttachConsole(GetCurrentProcessId()))
        {
            BOOST_LOG_TRIVIAL(error) << "Cannot attach console.";
            FreeConsole();
            return is_shown_system_console_ = false;
        }

        is_created_system_console_ = true;

        FILE* dummy;
        freopen_s(&dummy, "CON", "w", stdout);
        freopen_s(&dummy, "CON", "w", stderr);

        BOOST_LOG_TRIVIAL(info) << "System console is created.";

        return is_shown_system_console_ = true;
    }
#else
    return false;
#endif
}

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui_(new Ui::MainWindow)
{
    BOOST_LOG_TRIVIAL(debug) << "Initializing MainWindow ...";

    ui_->setupUi(this);

    setup_ui();
}

void MainWindow::update_ui(void)
{
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

void MainWindow::update_scenegraph_tree(const rapidjson::Value& root_object)
{
    BOOST_LOG_TRIVIAL(debug) << "Update scenegraph in tree widget.";

    ui_->scenegraph_tree_->clear();

    QTreeWidgetItem* root_item = new QTreeWidgetItem;
    root_item->setText(0, root_object["name"].GetString());

    ui_->scenegraph_tree_->addTopLevelItem(root_item);

    if (root_object["children"].IsArray())
    {
        for (const auto& child: root_object["children"].GetArray())
            update_scenegraph_subtree(root_item, child);
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Invalid API specification.";
    }
}

void MainWindow::update_nodepath(const rapidjson::Value& message)
{
    const auto& translation = message["translation"];
    ui_->translation_edit_x_->setText(QString().setNum(translation[0].GetFloat()));
    ui_->translation_edit_y_->setText(QString().setNum(translation[1].GetFloat()));
    ui_->translation_edit_z_->setText(QString().setNum(translation[2].GetFloat()));

    const auto& hpr = message["hpr"];
    ui_->hpr_edit_h_->setText(QString().setNum(hpr[0].GetFloat()));
    ui_->hpr_edit_p_->setText(QString().setNum(hpr[1].GetFloat()));
    ui_->hpr_edit_r_->setText(QString().setNum(hpr[2].GetFloat()));

    const auto& scale = message["scale"];
    ui_->scale_edit_x_->setText(QString().setNum(scale[0].GetFloat()));
    ui_->scale_edit_y_->setText(QString().setNum(scale[1].GetFloat()));
    ui_->scale_edit_z_->setText(QString().setNum(scale[2].GetFloat()));

    ui_->visible_checkbox_->setChecked(message["visible"].GetBool());
    ui_->tight_bounds_checkbox_->setChecked(message["tight_bounds"].GetBool());
    ui_->wireframe_checkbox_->setChecked(message["wireframe"].GetBool());
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

        // skip when closing
        //set_enable_restapi_actions(false);
    }

    save_settings();

#if defined(BOOST_OS_WINDOWS)
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // avoid crashing in Qt.
    if (is_created_system_console_)
        FreeConsole();
#endif

    ev->accept();
}

bool MainWindow::event(QEvent* ev)
{
    switch (ev->type())
    {
    case RestAPIEvent::EVENT_TYPE:
        resolve_message(static_cast<RestAPIEvent*>(ev)->get_message());
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

    // NodePath tab
    ui_->translation_edit_x_->setValidator(new QDoubleValidator(ui_->translation_edit_x_));
    ui_->translation_edit_y_->setValidator(new QDoubleValidator(ui_->translation_edit_y_));
    ui_->translation_edit_z_->setValidator(new QDoubleValidator(ui_->translation_edit_z_));

    ui_->hpr_edit_h_->setValidator(new QDoubleValidator(ui_->hpr_edit_h_));
    ui_->hpr_edit_p_->setValidator(new QDoubleValidator(ui_->hpr_edit_p_));
    ui_->hpr_edit_r_->setValidator(new QDoubleValidator(ui_->hpr_edit_r_));

    ui_->scale_edit_x_->setValidator(new QDoubleValidator(ui_->scale_edit_x_));
    ui_->scale_edit_y_->setValidator(new QDoubleValidator(ui_->scale_edit_y_));
    ui_->scale_edit_z_->setValidator(new QDoubleValidator(ui_->scale_edit_z_));

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

    connect(ui_->action_import_model_, &QAction::triggered, this, &MainWindow::import_model_tirggered);
    connect(ui_->action_refresh_scenegraph_, &QAction::triggered, this, &MainWindow::refresh_scenegraph_triggered);

    ui_->action_exit->setShortcuts(QKeySequence::Quit);

    connect(ui_->action_run_renderer, &QAction::triggered, this, &MainWindow::connect_rendering_server);

//    ui_->action_options->setShortcuts(QKeySequence::Preferences);
//    connect(ui_->action_options, &QAction::triggered, [this]() {
//        OptionsDialog option_dialog(this);
//        if (option_dialog.exec()) {
//            update_ui();
//        }
//    });

#if defined(BOOST_OS_WINDOWS)
    connect(ui_->action_toggle_system_console_, &QAction::triggered, [this]() {
        ui_->action_toggle_system_console_->setChecked(toggle_system_console());
    });
#else
    // this action can be only used in Windows.
    ui_->action_toggle_system_console_->setEnabled(false);
#endif

    connect(ui_->action_about_qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui_->action_about_application, &QAction::triggered, this, &MainWindow::about_application);
    connect(ui_->scenegraph_tree_, &QTreeWidget::currentItemChanged, this, &MainWindow::on_scenegraph_item_changed);

    load_settings();
    update_ui();

    set_enable_restapi_actions(false);

    ui_->status_bar->showMessage("Ready");
}

void MainWindow::connect_rendering_server(void)
{
    if (!restapi_client_)
    {
        BOOST_LOG_TRIVIAL(info) << "Connecting rendering server.";

        restapi_client_ = new RestAPIClient(QUrl(QStringLiteral("ws://localhost:8888")), this);

        QObject::connect(restapi_client_, &RestAPIClient::closed, [this]() {
            BOOST_LOG_TRIVIAL(info) << "Rendering server is disconnected.";

            restapi_client_->deleteLater();
            restapi_client_ = nullptr;
            set_enable_restapi_actions(false);

            ui_->status_bar->showMessage("Rendering server is disconnected");
        });

        set_enable_restapi_actions(true);

        ui_->status_bar->showMessage("Rendering server is connected");
    }
}

void MainWindow::set_enable_restapi_actions(bool enable)
{
    ui_->action_import_model_->setEnabled(enable);
    ui_->action_refresh_scenegraph_->setEnabled(enable);
    ui_->nodepath_tab_->setEnabled(enable);
}

void MainWindow::about_application(void)
{
    QMessageBox::about(this, QString("About ") + QApplication::instance()->applicationName(), "");
}

}   // namespace rpeditor
