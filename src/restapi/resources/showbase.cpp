#include <boost/log/trivial.hpp>

#include "restapi/resolve_message.hpp"

#include "main_window.hpp"

namespace rpeditor {

void resolve_showbase(const rapidjson::Document& doc)
{
    const std::string& method = doc[RPEDITOR_API_METHOD_STRING].GetString();
    if (method == RPEDITOR_API_UPDATE_STRING)
    {
        rpeditor::MainWindow::get_instance()->update_scenegraph_tree(doc[RPEDITOR_API_MESSAGE_STRING]["root"]);
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Unknown method: " << method;
    }
}

// ************************************************************************************************

ConfigureStaticInit(ShowBase)
{
    auto& resolver_map = get_resolver_map();
    resolver_map["ShowBase"] = resolve_showbase;
}

}   // namespace rpeditor
