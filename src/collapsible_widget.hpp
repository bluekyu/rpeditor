#pragma once

#include <QWidget>

class QScrollArea;
class QToolButton;
class QParallelAnimationGroup;

namespace Ui {
class CollapsibleWidget;
}

namespace rpeditor {

/**
 * Note: http://stackoverflow.com/questions/32476006/how-to-make-an-expandable-collapsable-section-widget-in-qt
 */
class CollapsibleWidget: public QWidget
{
public:
    CollapsibleWidget(void);

    void set_content_widget(QWidget* widget);

private:
    QScrollArea* contents_area_;
    QToolButton* toggle_button_;
    QParallelAnimationGroup* toggle_animation_;
    int animation_duration_ = 300;
};

}   // namespace rpeditor
