#ifndef UTILITIESTAB_ALARMHISTORY_WIDGET_H
#define UTILITIESTAB_ALARMHISTORY_WIDGET_H

#include <QWidget>
#include <QAbstractButton>
#include <QDateTime>

namespace Ui {
class UtilitiesTab_alarmHistory_Widget;
}

class UtilitiesTab_alarmHistory_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit UtilitiesTab_alarmHistory_Widget(QWidget *parent = nullptr);
    ~UtilitiesTab_alarmHistory_Widget();

protected:
    void showEvent(QShowEvent *event);

private:
    Ui::UtilitiesTab_alarmHistory_Widget *ui;
    QList<QAbstractButton*> deviceButtonList;
    QString queryDeviceName;
    QDateTime queryStartDateTime, queryEndDateTime;
    QString queryOrderBy;

private slots:
    void on_deviceButton_Clicked(QAbstractButton* button);
    void on_query_pushButton_clicked();
    void on_startDate_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime);
    void on_endDate_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime);
    void on_orderType_comboBox_currentIndexChanged(int index);
};

#endif // UTILITIESTAB_ALARMHISTORY_WIDGET_H
