#include "collapsible_widget.hpp"

#include <QScrollArea>
#include <QFrame>
#include <QLayout>
#include <QToolButton>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

namespace rpeditor {

CollapsibleWidget::CollapsibleWidget(void)
{
    QVBoxLayout* main_layout = new QVBoxLayout;
    setLayout(main_layout);

    QHBoxLayout* headline_layout = new QHBoxLayout;
    main_layout->addLayout(headline_layout);

    toggle_button_ = new QToolButton;
    toggle_button_->setStyleSheet("QToolButton { border: none; }");
    toggle_button_->setCheckable(true);
    toggle_button_->setChecked(false);
    toggle_button_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toggle_button_->setArrowType(Qt::RightArrow);
    toggle_button_->setText("Text");

    headline_layout->addWidget(toggle_button_);

    QFrame* headerline = new QFrame;
    headerline->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    headerline->setFrameShape(QFrame::HLine);
    headerline->setFrameShadow(QFrame::Plain);

    headline_layout->addWidget(headerline);

    contents_area_ = new QScrollArea;
    contents_area_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    contents_area_->setMaximumHeight(0);
    contents_area_->setMinimumHeight(0);
    main_layout->addWidget(contents_area_);

    toggle_animation_ = new QParallelAnimationGroup(this);
    toggle_animation_->addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    toggle_animation_->addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    toggle_animation_->addAnimation(new QPropertyAnimation(contents_area_, "maximumHeight"));

    connect(toggle_button_, &QToolButton::clicked, [this](const bool checked) {
        toggle_button_->setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
        toggle_animation_->setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
        toggle_animation_->start();
    });
}

void CollapsibleWidget::set_content_widget(QWidget* widget)
{
    contents_area_->setWidget(widget);

    const auto collapsedHeight = sizeHint().height() - contents_area_->maximumHeight();
    auto contentHeight = contents_area_->sizeHint().height();
    for (int i = 0; i < toggle_animation_->animationCount() - 1; ++i) {
        QPropertyAnimation * spoilerAnimation = static_cast<QPropertyAnimation *>(toggle_animation_->animationAt(i));
        spoilerAnimation->setDuration(animation_duration_);
        spoilerAnimation->setStartValue(collapsedHeight);
        spoilerAnimation->setEndValue(collapsedHeight + contentHeight);
    }

    QPropertyAnimation * contentAnimation = static_cast<QPropertyAnimation *>(toggle_animation_->animationAt(toggle_animation_->animationCount() - 1));
    contentAnimation->setDuration(animation_duration_);
    contentAnimation->setStartValue(0);
    contentAnimation->setEndValue(contentHeight);
}

}   // namespace rpeditor
