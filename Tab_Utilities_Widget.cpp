#include "Tab_Utilities_Widget.h"
#include "ui_Tab_Utilities_Widget.h"
#include "ui_Tab_Utilities_alarmHistoryPage_Widget.h"

Tab_Utilities_Widget::Tab_Utilities_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab_Utilities_Widget)
{
    ui->setupUi(this);
    ui->exporterPage_stackedWidget->setCurrentIndex(0);
    ui->exporterPage_stackedWidget->setCurrentWidget(ui->exporterPage_widget);
    ui->alarmHistoryPage_stackedWidget->setCurrentWidget(ui->alarmHistoryPage_widget);
    ui->technicalAlarmPage_stackedWidget->setCurrentWidget(ui->alarmHistoryPage_widget);
    connect(ui->UtilitiesTab_tabWidget, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(on_UtilitiesTab_tabWidget_currentChanged(int index)));
}

Tab_Utilities_Widget::~Tab_Utilities_Widget()
{
    delete ui;
}


void Tab_Utilities_Widget::on_UtilitiesTab_tabWidget_currentChanged(int index)
{
    if(index == 1)
        {
        ui->alarmHistoryPage_widget->is_technical = 0;
        }
    else if(index == 2)
        {
        ui->alarmHistoryPage_widget->is_technical = 1;
        }
}


