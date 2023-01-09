#ifndef UTILITIESTAB_EXPORTER_WIDGET_H
#define UTILITIESTAB_EXPORTER_WIDGET_H

#include <QWidget>

namespace Ui {
class UtilitiesTab_exporter_Widget;
}

class UtilitiesTab_exporter_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit UtilitiesTab_exporter_Widget(QWidget *parent = nullptr);
    ~UtilitiesTab_exporter_Widget();

private:
    Ui::UtilitiesTab_exporter_Widget *ui;
};

#endif // UTILITIESTAB_EXPORTER_WIDGET_H
