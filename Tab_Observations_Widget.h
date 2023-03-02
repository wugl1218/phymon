#ifndef TAB_OBSERVATIONS_WIDGET_H
#define TAB_OBSERVATIONS_WIDGET_H

#include <QWidget>
#include <QStack>

namespace Ui {
class Tab_Observations_Widget;
}

class Tab_Observations_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Tab_Observations_Widget(QWidget *parent = nullptr);
    ~Tab_Observations_Widget();


private:
    Ui::Tab_Observations_Widget *ui;
    QStack<QWidget*> prevPageStack;

public slots:
    void changeToHistoryPage();
    void changeToPrevPage();
    void changeToMetricItemsDisplayConfigPage();
};

#endif // TAB_OBSERVATIONS_WIDGET_H
