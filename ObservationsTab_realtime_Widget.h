#ifndef OBSERVATIONSTAB_REALTIME_WIDGET_H
#define OBSERVATIONSTAB_REALTIME_WIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QAbstractButton>

namespace Ui {
class ObservationsTab_realtime_Widget;
}

class ObservationsTab_realtime_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit ObservationsTab_realtime_Widget(QWidget *parent = nullptr);
    ~ObservationsTab_realtime_Widget();

private:
    Ui::ObservationsTab_realtime_Widget *ui;
    QList<QScrollArea*> trendViewList;
    QList<QAbstractButton*> trendViewBtnList;

signals:

private slots:
    void on_trendView_pushButton_clicked(QAbstractButton* button);
};

#endif // OBSERVATIONSTAB_REALTIME_WIDGET_H
