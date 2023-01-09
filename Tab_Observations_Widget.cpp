#include "Tab_Observations_Widget.h"
#include "ui_Tab_Observations_Widget.h"

Tab_Observations_Widget::Tab_Observations_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab_Observations_Widget)
{
    Common* common = Common::instance();
    common->observation_widget_page = this;
    ui->setupUi(this);
    //
    connect(ui->mainPage_widget, SIGNAL(changeToMetricItemsDisplayConfigPage()), this, SLOT(changeToMetricItemsDisplayConfigPage()));
    connect(ui->mainPage_widget, SIGNAL(changeToHistoryPage()), this, SLOT(changeToHistoryPage()));
    connect(ui->historyPage_widget, SIGNAL(changeToPrevPage()), this, SLOT(changeToPrevPage()));
    connect(ui->metricItemsDisplayConfigPage_widget, SIGNAL(changeToPrevPage()), this, SLOT(changeToPrevPage()));
}

Tab_Observations_Widget::~Tab_Observations_Widget()
{
    delete ui;
}

void Tab_Observations_Widget::changeToHistoryPage()
{
    this->prevPageStack.push(ui->stackedWidget->currentWidget());
    ui->stackedWidget->setCurrentWidget(ui->historyPage_widget);
}

void Tab_Observations_Widget::changeToPrevPage()
{
    if(this->prevPageStack.isEmpty()) return;
    ui->stackedWidget->setCurrentWidget(this->prevPageStack.pop());
}

void Tab_Observations_Widget::changeToMetricItemsDisplayConfigPage()
{
    this->prevPageStack.push(ui->stackedWidget->currentWidget());
    ui->stackedWidget->setCurrentWidget(ui->metricItemsDisplayConfigPage_widget);
}
