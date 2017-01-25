#pragma once

#include <QWidget>

class QScrollArea;

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
};

}   // namespace rpeditor
