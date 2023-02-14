#include "Tab_Utilities_Widget.h"
#include "ui_Tab_Utilities_Widget.h"
#include "ui_Tab_Utilities_alarmHistoryPage_Widget.h"

Tab_Utilities_Widget::Tab_Utilities_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab_Utilities_Widget)
{
    ui->setupUi(this);
    ui->exporterPage_stackedWidget->setCurrentIndex(1);
    ui->exporterPage_stackedWidget->setCurrentWidget(ui->exporterPage_widget);
    connect(ui->UtilitiesTab_tabWidget, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(on_UtilitiesTab_tabWidget_currentChanged(int index)));
    ui->alarmHistoryPage_widget->is_technical = 0;
    ui->technicalHistoryPage_widget->is_technical = 1;
    ui->UnhandledAlarm_widget->is_technical = 2;
    ui->UtilitiesTab_tabWidget->removeTab(3);
    ui->UtilitiesTab_tabWidget->removeTab(0);

}

Tab_Utilities_Widget::~Tab_Utilities_Widget()
{
    delete ui;
}


void Tab_Utilities_Widget::on_UtilitiesTab_tabWidget_currentChanged(int index)
{

}


