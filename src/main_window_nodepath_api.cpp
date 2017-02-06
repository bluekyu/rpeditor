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

void MainWindow::on_nodepath_tab_selected(QTreeWidgetItem* item)
{
    if (!restapi_client_)
        return;

    rapidjson::Document doc;
    rapidjson::Value& message = initialize_api_document(doc, "NodePath", RPEDITOR_API_READ_STRING);
    auto& allocator = doc.GetAllocator();

    add_member_scenegraph_item_path(message, allocator, item);

    restapi_client_->write(doc);
}

void MainWindow::on_nodepath_changed(void)
{
    rapidjson::Document doc;
    rapidjson::Value& message = initialize_api_document(doc, "NodePath", RPEDITOR_API_UPDATE_STRING);
    auto& allocator = doc.GetAllocator();

    add_member_scenegraph_item_path(message, allocator, ui_->scenegraph_tree_->currentItem());

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

}   // namespace rpeditor
