#pragma once

#include <thread>

#include <QMainWindow>

#include <boost/property_tree/ptree.hpp>

#include "restapi/config.hpp"

class QTreeWidgetItem;

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
    static MainWindow* get_instance(void)
    {
        static MainWindow instance;
        return &instance;
    }

    /**
     * @return visibility of console.
     */
    static bool toggle_system_console(void);
    static bool is_created_system_console(void);

    void update_ui(void);

    void update_scenegraph_tree(const rapidjson::Value& root_object);
    void update_nodepath(const rapidjson::Value& message);

protected:
    virtual void closeEvent(QCloseEvent* ev);
    virtual bool event(QEvent* ev) override;

private:
    static bool is_shown_system_console_;
    static bool is_created_system_console_;

    explicit MainWindow(QWidget* parent = nullptr);

    bool load_settings(void);
    bool save_settings(void);

    std::string get_settings_file_path(void) const;

    void setup_ui(void);

    void connect_rendering_server(void);

    void set_enable_restapi_actions(bool enable);

    void about_application(void);

    /** RPEditor API */
    ///@{

    void import_model_tirggered(void);

    void refresh_scenegraph_triggered(void);

    void on_scenegraph_item_changed(const QTreeWidgetItem* item, const QTreeWidgetItem*);

    void on_nodepath_changed(void);

    ///@}

    Ui::MainWindow* ui_;

    std::string config_dir_path_;   ///< Path of configuration directory of application.

    RestAPIClient* restapi_client_;
    std::shared_ptr<std::thread> restapi_network_thread_;
};

}   // namespace rpeditor
