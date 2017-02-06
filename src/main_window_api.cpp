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

#include <deque>

#include <QtWidgets/QTreeWidget>

#include <boost/log/trivial.hpp>

#include "restapi/restapi_client.hpp"

namespace rpeditor {

void MainWindow::update_geometry(const rapidjson::Value& message)
{
    ui_->main_tab_widget_->setTabEnabled(MainTabIndex::GEOMETRY, true);

    const int num_geoms = message["num_geoms"].GetInt();
    ui_->geometry_geom_index_spinbox_->setSuffix(QString(" th (%1 geoms)").arg(num_geoms));
    ui_->geometry_geom_index_spinbox_->setMaximum(num_geoms-1);

    const int geom_index = message["index"].GetInt();
    ui_->geometry_geom_index_spinbox_->setValue(geom_index);

    ui_->geom_num_primitives_label_->setText(QString().setNum(message["num_primitives"].GetInt()));
}

// ************************************************************************************************
void MainWindow::add_member_scenegraph_item_path(rapidjson::Value& message, rapidjson::MemoryPoolAllocator<>& allocator, QTreeWidgetItem* item) const
{
    std::deque<int> index_array;
    QStringList node_name_list;
    while (item && item->parent())
    {
        index_array.push_front(item->parent()->indexOfChild(item));
        item = item->parent();
    }

    rapidjson::Value json_index_array(rapidjson::kArrayType);
    for (int index : index_array)
        json_index_array.PushBack(index, allocator);
    message.AddMember("path", json_index_array, allocator);
}

void MainWindow::on_geometry_tab_selected(QTreeWidgetItem* item)
{
    if (!restapi_client_)
        return;

    rapidjson::Document doc;
    rapidjson::Value& message = initialize_api_document(doc, "GeomNode", RPEDITOR_API_READ_STRING);
    auto& allocator = doc.GetAllocator();

    add_member_scenegraph_item_path(message, allocator, item);

    message.AddMember("index", ui_->geometry_geom_index_spinbox_->value(), doc.GetAllocator());
    restapi_client_->write(doc);
}

void MainWindow::on_scenegraph_item_changed(QTreeWidgetItem* item)
{
    switch (ui_->main_tab_widget_->currentIndex())
    {
    case 0:
        {
            on_nodepath_tab_selected(item);
            break;
        }

    case 1:
        {
            if (item->text(1) == "GeomNode")
                on_geometry_tab_selected(item);
            else
                ui_->main_tab_widget_->setTabEnabled(MainTabIndex::GEOMETRY, false);
            break;
        }

    case 2:
        {
            if (item->text(1) == "GeomNode")
                on_material_tab_selected(item);
            else
                ui_->main_tab_widget_->setTabEnabled(MainTabIndex::MATERIAL, false);
            break;
        }

    default:
        break;
    }
}

}   // namespace rpeditor
