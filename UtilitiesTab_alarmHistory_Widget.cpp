#include "UtilitiesTab_alarmHistory_Widget.h"
#include "ui_UtilitiesTab_alarmHistory_Widget.h"
#include <QCalendarWidget>
#include <QEventLoop>
#include <QButtonGroup>
#include <QMessageBox>

UtilitiesTab_alarmHistory_Widget::UtilitiesTab_alarmHistory_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UtilitiesTab_alarmHistory_Widget)
{
    ui->setupUi(this);
    //動態新增Device按鈕
    QList<QString> btnNameList = {"Savina", "MP40"};
    QList<QString> btnDescriptionList = {"Dreager\nSavina", "Philips\nMP40"};
    //
    QButtonGroup* btnGroup = new QButtonGroup(this);
    QPushButton* templateBtn = ui->deviceTemplate_pushButton; //按鈕範本，已經在UI先設定相關屬性方便後續套用
    for(int i=0; i < btnNameList.length(); i++)
    {
        QPushButton* button = new QPushButton(btnDescriptionList[i], this);
        button->setProperty("DeviceName", btnNameList[i]);
        button->setSizePolicy(templateBtn->sizePolicy());
        button->setMinimumSize(templateBtn->minimumSize());
        button->setMaximumSize(templateBtn->maximumSize());
        button->setCheckable(templateBtn->isCheckable());
        button->setStyle(templateBtn->style());
        deviceButtonList.append(button);
        btnGroup->addButton(button);
        ui->devices_horizontalLayout->addWidget(button);
    }
    templateBtn->deleteLater(); //方便建立動態產生的按鈕範本，所有按鈕建立後即可以刪除。
    deviceButtonList.first()->click(); //預設選取第一個項目
    //
    connect(btnGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(on_deviceButton_Clicked(QAbstractButton*)));
    on_orderType_comboBox_currentIndexChanged(0);
    //
    ui->queryResult_tableWidget->resizeColumnsToContents();
}

UtilitiesTab_alarmHistory_Widget::~UtilitiesTab_alarmHistory_Widget()
{
    delete ui;
}

void UtilitiesTab_alarmHistory_Widget::showEvent(QShowEvent *event)
{
    QDateTime now = QDateTime::currentDateTime();
    ui->startDate_dateTimeEdit->setDateTime(now);
    ui->endDate_dateTimeEdit->setDateTime(now);
    this->queryStartDateTime = this->queryEndDateTime = now;
    QWidget::showEvent(event);
}

void UtilitiesTab_alarmHistory_Widget::on_deviceButton_Clicked(QAbstractButton* button)
{
    this->queryDeviceName = button->property("DeviceName").toString();
//    qDebug("Query %s...",this->queryDeviceName.toStdString().c_str());
}

void UtilitiesTab_alarmHistory_Widget::on_query_pushButton_clicked()
{
    QDateTime now = QDateTime::currentDateTime();
    if((this->queryStartDateTime < now.addDays(-2)
         || this->queryEndDateTime > now)
        || (this->queryStartDateTime > this->queryEndDateTime))
    {
        QMessageBox::warning(this, "Query date range error!", "limit before 48 hours.");
//        qDebug("now:%s\nbefore:%s\nstart:%s\nend:%s", now.toString("yyyy-MM-dd hh:mm").toStdString().c_str(), now.addDays(-2).toString("yyyy-MM-dd hh:mm").toStdString().c_str(), queryStartDateTime.toString("yyyy-MM-dd hh:mm").toStdString().c_str(), queryEndDateTime.toString("yyyy-MM-dd hh:mm").toStdString().c_str());
        return;
    }
    qDebug("Query start:'%s' end:'%s' orderBy:'%s'"
           , queryStartDateTime.toString("yyyy-MM-dd hh:mm").toStdString().c_str()
           , queryEndDateTime.toString("yyyy-MM-dd hh:mm").toStdString().c_str()
           , queryOrderBy.toStdString().c_str()
           );
}


void UtilitiesTab_alarmHistory_Widget::on_startDate_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    this->queryStartDateTime = dateTime;
}


void UtilitiesTab_alarmHistory_Widget::on_endDate_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    this->queryEndDateTime = dateTime;
}

void UtilitiesTab_alarmHistory_Widget::on_orderType_comboBox_currentIndexChanged(int index)
{
    switch(index)
    {
        case 0: //datetime
        default:
            this->queryOrderBy = "datetime";
            break;
        case 1: //duration
            this->queryOrderBy = "duration";
            break;
        case 2: //message
            this->queryOrderBy = "message";
            break;
    }
}

