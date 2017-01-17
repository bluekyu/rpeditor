#pragma once

#include <thread>

#include <QMainWindow>

#include <boost/property_tree/ptree.hpp>

#include "logging.hpp"

namespace Ui {
class MainWindow;
}

namespace rpeditor {

class RestAPIClient;

/**
 */
class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    enum EventType
    {
        UPDATE_SCENEGRAPH_TREE = QEvent::User+1,
    };

private:
    using sink_t = boost::log::sinks::synchronous_sink<LoggingConsoleBackend>;

public:
    static MainWindow* get_instance(void)
    {
        static MainWindow instance;
        return &instance;
    }

    ~MainWindow();

    void update_ui(void);

    void update_scenegraph_tree(void);

protected:
    virtual void MainWindow::closeEvent(QCloseEvent* ev);
    virtual bool event(QEvent* ev) override;

private:
    explicit MainWindow(QWidget* parent = nullptr);

    bool load_settings(void);
    bool save_settings(void);

    std::string get_settings_file_path(void) const;

    void setup_ui(void);

    void connect_restapi_server(void);

    void about_application(void);

    Ui::MainWindow* ui_;

    boost::shared_ptr<sink_t> logging_sink = nullptr;

    std::string config_dir_path_;   ///< Path of configuration directory of application.

    std::shared_ptr<RestAPIClient> restapi_client_;
    std::shared_ptr<std::thread> restapi_network_thread_;
};

}   // namespace rpeditor
