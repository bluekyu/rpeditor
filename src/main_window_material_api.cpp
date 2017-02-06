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

#include <boost/log/trivial.hpp>

#include "restapi/restapi_client.hpp"

namespace rpeditor {

void MainWindow::update_material(const rapidjson::Value& message)
{
    ui_->main_tab_widget_->setTabEnabled(MainTabIndex::MATERIAL, true);

    const int num_geoms = message["num_geoms"].GetInt();
    ui_->material_geom_index_spinbox_->setSuffix(QString(" th (%1 geoms)").arg(num_geoms));
    ui_->material_geom_index_spinbox_->setMaximum(num_geoms-1);

    const int geom_index = message["index"].GetInt();
    ui_->material_geom_index_spinbox_->setValue(geom_index);

    ui_->material_name_label_->setText(QString::fromStdString(message["name"].GetString()));

    ui_->material_shading_model_combobox_->setCurrentIndex(message["shading_model"].GetInt());

    const auto& base_color = message["base_color"];
    auto palette = ui_->material_base_color_dialog_button_->palette();
    palette.setColor(QPalette::Button, QColor::fromRgbF(base_color[0].GetFloat(), base_color[1].GetFloat(), base_color[2].GetFloat()));
    ui_->material_base_color_dialog_button_->setPalette(palette);

    ui_->material_roughness_spinbox_->setValue(message["roughness"].GetFloat());
    ui_->material_speuclar_ior_spinbox_->setValue(message["specular_ior"].GetFloat());
}

void MainWindow::on_material_tab_selected(QTreeWidgetItem* item)
{
    if (!restapi_client_)
        return;

    rapidjson::Document doc;
    rapidjson::Value& message = initialize_api_document(doc, "Material", RPEDITOR_API_READ_STRING);
    auto& allocator = doc.GetAllocator();

    add_member_scenegraph_item_path(message, allocator, item);

    message.AddMember("index", ui_->material_geom_index_spinbox_->value(), doc.GetAllocator());
    restapi_client_->write(doc);
}

void MainWindow::on_material_changed(void)
{
    rapidjson::Document doc;
    rapidjson::Value& message = initialize_api_document(doc, "Material", RPEDITOR_API_UPDATE_STRING);
    auto& allocator = doc.GetAllocator();

    add_member_scenegraph_item_path(message, allocator, ui_->scenegraph_tree_->currentItem());

    message.AddMember("index", ui_->material_geom_index_spinbox_->value(), allocator);

    message.AddMember("shading_model", ui_->material_shading_model_combobox_->currentIndex(), allocator);

    qreal r, g, b;
    ui_->material_base_color_dialog_button_->palette().color(QPalette::Button).getRgbF(&r, &g, &b);

    rapidjson::Value base_color_array(rapidjson::kArrayType);
    base_color_array.PushBack(r, allocator);
    base_color_array.PushBack(g, allocator);
    base_color_array.PushBack(b, allocator);
    message.AddMember("base_color", base_color_array, allocator);

    message.AddMember("roughness", ui_->material_roughness_spinbox_->value(), allocator);
    message.AddMember("specular_ior", ui_->material_speuclar_ior_spinbox_->value(), allocator);

    restapi_client_->write(doc);
}

}   // namespace rpeditor
