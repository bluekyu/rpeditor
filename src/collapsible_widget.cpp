#include "collapsible_widget.hpp"

#include <QScrollArea>
#include <QFrame>
#include <QLayout>
#include <QToolButton>

namespace rpeditor {

CollapsibleWidget::CollapsibleWidget(void)
{
    QVBoxLayout* main_layout = new QVBoxLayout;
    setLayout(main_layout);

    QHBoxLayout* headline_layout = new QHBoxLayout;
    main_layout->addLayout(headline_layout);

    QToolButton* toggle_button = new QToolButton;
    toggle_button->setStyleSheet("QToolButton { border: none; }");
    toggle_button->setCheckable(true);
    toggle_button->setChecked(false);
    toggle_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toggle_button->setArrowType(Qt::RightArrow);
    toggle_button->setText("Text");

    headline_layout->addWidget(toggle_button);

    QFrame* headerline = new QFrame;
    headerline->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    headerline->setFrameShape(QFrame::HLine);
    headerline->setFrameShadow(QFrame::Plain);

    headline_layout->addWidget(headerline);

    contents_area_ = new QScrollArea;
    contents_area_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    main_layout->addWidget(contents_area_);
}

void CollapsibleWidget::set_content_widget(QWidget* widget)
{
    contents_area_->setWidget(widget);
}

}   // namespace rpeditor
