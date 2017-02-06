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

#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QFileDialog>

#include <boost/log/trivial.hpp>

#include "restapi/restapi_client.hpp"

namespace rpeditor {

inline void update_scenegraph_subtree(QTreeWidgetItem* parent, const rapidjson::Value& current)
{
    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText(0, current["name"].GetString());
    item->setText(1, current["type"].GetString());

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

void MainWindow::update_scenegraph_tree(const rapidjson::Value& root_object)
{
    BOOST_LOG_TRIVIAL(debug) << "Update scenegraph in tree widget.";

    ui_->scenegraph_tree_->clear();

    QTreeWidgetItem* root_item = new QTreeWidgetItem;
    root_item->setText(0, root_object["name"].GetString());
    root_item->setText(1, root_object["type"].GetString());

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

void MainWindow::import_model_tirggered(void)
{
    const std::string& file_path = QFileDialog::getOpenFileName(this, "Import Model", "", QString("All Files (*.*)")).toStdString();

    rapidjson::Document doc;
    rapidjson::Value& message = initialize_api_document(doc, "ShowBase", RPEDITOR_API_CREATE_STRING);
    message.AddMember("path", file_path, doc.GetAllocator());
    restapi_client_->write(doc);
}

void MainWindow::refresh_scenegraph_triggered(void)
{
    rapidjson::Document doc;
    initialize_api_document(doc, "ShowBase", RPEDITOR_API_READ_STRING);
    restapi_client_->write(doc);
}

}   // namespace rpeditor
