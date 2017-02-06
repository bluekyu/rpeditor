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

void update_scenegraph_subtree(QTreeWidgetItem* parent, const rapidjson::Value& current)
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

void MainWindow::update_geometry(const rapidjson::Value& message)
{
    ui_->geometry_tab_->setEnabled(true);

    const int num_geoms = message["num_geoms"].GetInt();
    ui_->geometry_geom_index_spinbox_->setSuffix(QString(" th (%1 geoms)").arg(num_geoms));
    ui_->geometry_geom_index_spinbox_->setMaximum(num_geoms-1);

    const int geom_index = message["index"].GetInt();
    ui_->geometry_geom_index_spinbox_->setValue(geom_index);

    ui_->geom_num_primitives_label_->setText(QString().setNum(message["num_primitives"].GetInt()));
}

void MainWindow::update_material(const rapidjson::Value& message)
{
    ui_->material_tab_->setEnabled(true);

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

// ************************************************************************************************
void MainWindow::get_scenegraph_item_node_path(std::deque<int>& index_array, QTreeWidgetItem* item) const
{
    QStringList node_name_list;
    while (item && item->parent())
    {
        index_array.push_front(item->parent()->indexOfChild(item));
        item = item->parent();
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

void MainWindow::on_nodepath_tab_selected(QTreeWidgetItem* item)
{
    if (!restapi_client_)
        return;

    rapidjson::Document doc;
    rapidjson::Value& message = initialize_api_document(doc, "NodePath", RPEDITOR_API_READ_STRING);
    auto& allocator = doc.GetAllocator();

    std::deque<int> index_array;
    get_scenegraph_item_node_path(index_array, item);

    rapidjson::Value json_index_array(rapidjson::kArrayType);
    for (int index: index_array)
        json_index_array.PushBack(index, allocator);
    message.AddMember("path", json_index_array, allocator);

    restapi_client_->write(doc);
}

void MainWindow::on_geometry_tab_selected(QTreeWidgetItem* item)
{
    if (!restapi_client_)
        return;

    rapidjson::Document doc;
    rapidjson::Value& message = initialize_api_document(doc, "GeomNode", RPEDITOR_API_READ_STRING);
    auto& allocator = doc.GetAllocator();

    std::deque<int> index_array;
    get_scenegraph_item_node_path(index_array, item);

    rapidjson::Value json_index_array(rapidjson::kArrayType);
    for (int index: index_array)
        json_index_array.PushBack(index, allocator);
    message.AddMember("path", json_index_array, allocator);

    message.AddMember("index", ui_->geometry_geom_index_spinbox_->value(), doc.GetAllocator());
    restapi_client_->write(doc);
}

void MainWindow::on_material_tab_selected(QTreeWidgetItem* item)
{
    if (!restapi_client_)
        return;

    rapidjson::Document doc;
    rapidjson::Value& message = initialize_api_document(doc, "Material", RPEDITOR_API_READ_STRING);
    auto& allocator = doc.GetAllocator();

    std::deque<int> index_array;
    get_scenegraph_item_node_path(index_array, item);

    rapidjson::Value json_index_array(rapidjson::kArrayType);
    for (int index: index_array)
        json_index_array.PushBack(index, allocator);
    message.AddMember("path", json_index_array, allocator);

    message.AddMember("index", ui_->material_geom_index_spinbox_->value(), doc.GetAllocator());
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
                ui_->geometry_tab_->setEnabled(false);
            break;
        }

    case 2:
        {
            if (item->text(1) == "GeomNode")
                on_material_tab_selected(item);
            else
                ui_->material_tab_->setEnabled(false);
            break;
        }

    default:
        break;
    }
}

void MainWindow::on_nodepath_changed(void)
{
    rapidjson::Document doc;
    rapidjson::Value& message = initialize_api_document(doc, "NodePath", RPEDITOR_API_UPDATE_STRING);
    auto& allocator = doc.GetAllocator();

    std::deque<int> index_array;
    get_scenegraph_item_node_path(index_array, ui_->scenegraph_tree_->currentItem());

    rapidjson::Value json_index_array(rapidjson::kArrayType);
    for (int index: index_array)
        json_index_array.PushBack(index, allocator);
    message.AddMember("path", json_index_array, allocator);

    // pose
    rapidjson::Value translation_array(rapidjson::kArrayType);
    translation_array.PushBack(ui_->translation_edit_x_->text().toFloat(), allocator);
    translation_array.PushBack(ui_->translation_edit_y_->text().toFloat(), allocator);
    translation_array.PushBack(ui_->translation_edit_z_->text().toFloat(), allocator);
    message.AddMember("translation", translation_array, allocator);

    rapidjson::Value hpr_array(rapidjson::kArrayType);
    hpr_array.PushBack(ui_->hpr_edit_h_->text().toFloat(), allocator);
    hpr_array.PushBack(ui_->hpr_edit_p_->text().toFloat(), allocator);
    hpr_array.PushBack(ui_->hpr_edit_r_->text().toFloat(), allocator);
    message.AddMember("hpr", hpr_array, allocator);

    rapidjson::Value scale_array(rapidjson::kArrayType);
    scale_array.PushBack(ui_->scale_edit_x_->text().toFloat(), allocator);
    scale_array.PushBack(ui_->scale_edit_y_->text().toFloat(), allocator);
    scale_array.PushBack(ui_->scale_edit_z_->text().toFloat(), allocator);
    message.AddMember("scale", scale_array, allocator);

    // visibility
    message.AddMember("visible", ui_->visible_checkbox_->isChecked(), allocator);
    message.AddMember("tight_bounds", ui_->tight_bounds_checkbox_->isChecked(), allocator);
    message.AddMember("wireframe", ui_->wireframe_checkbox_->isChecked(), allocator);

    restapi_client_->write(doc);
}

void MainWindow::on_material_changed(void)
{
    rapidjson::Document doc;
    rapidjson::Value& message = initialize_api_document(doc, "Material", RPEDITOR_API_UPDATE_STRING);
    auto& allocator = doc.GetAllocator();

    std::deque<int> index_array;
    get_scenegraph_item_node_path(index_array, ui_->scenegraph_tree_->currentItem());

    rapidjson::Value json_index_array(rapidjson::kArrayType);
    for (int index : index_array)
        json_index_array.PushBack(index, allocator);
    message.AddMember("path", json_index_array, allocator);

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
