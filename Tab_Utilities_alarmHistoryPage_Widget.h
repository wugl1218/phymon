#ifndef TAB_UTILITIES_ALARMHISTORYPAGE_WIDGET_H
#define TAB_UTILITIES_ALARMHISTORYPAGE_WIDGET_H

#include <QWidget>
#include <QAbstractButton>
#include <QDateTime>
#include "Common.h"
#include <QTime>


namespace Ui {
class Tab_Utilities_alarmHistoryPage_Widget;
}

class Tab_Utilities_alarmHistoryPage_Widget : public QWidget
{
    Q_OBJECT

public:
    int dev_index;
    int order;
    int is_technical;
    uint8_t index_u8;//philo
    Ui::Tab_Utilities_alarmHistoryPage_Widget *ui;
    explicit Tab_Utilities_alarmHistoryPage_Widget(QWidget *parent = nullptr);
    ~Tab_Utilities_alarmHistoryPage_Widget();
    void update_devices(std::map<std::string, Device>* btns);


private:
    std::vector<QAbstractButton*> devices;

private slots:
    void btn_clicked();
    void on_query_pushButton_clicked();
    void on_orderType_comboBox_currentIndexChanged(int index);
    void on_DatecomboBox_currentIndexChanged(int index);
};

#endif // TAB_UTILITIES_ALARMHISTORYPAGE_WIDGET_H
