#include "Tab_Utilities_alarmHistoryPage_Widget.h"
#include "ui_Tab_Utilities_alarmHistoryPage_Widget.h"
#include "Tab_Utilities_Widget.h"
#include <QButtonGroup>
#include <QMessageBox>
#include <QTime>
#include <QDateTime>
#include <QDate>



Tab_Utilities_alarmHistoryPage_Widget::Tab_Utilities_alarmHistoryPage_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab_Utilities_alarmHistoryPage_Widget)
{
    Common* common = Common::instance();


    if  (common->alarm_page == NULL)
        common->alarm_page = this;
    else
        common->alarm_page_2 = this;

    ui->setupUi(this);
    ui->deviceTemplate_pushButton->hide();
    dev_index = 0;
    order = 1;
    index_u8 = 0;
    on_orderType_comboBox_currentIndexChanged(0);
    ui->queryResult_tableWidget->setColumnWidth(2, 530);
}

void Tab_Utilities_alarmHistoryPage_Widget::update_devices(std::map<std::string, Device>* btns)
{
    Common* common = Common::instance();
    for(int i=0;i<(int)devices.size();i++)
    {
        ui->devices_horizontalLayout->removeWidget(devices[i]);
        delete devices[i];
    }
    devices.clear();
    QButtonGroup *qButtonGroup = new QButtonGroup(this);
    qButtonGroup->setExclusive(true);
    int i=0;
    for(auto it=btns->begin();it!=btns->end();it++)
    {
        std::string label = it->second.model;
        QPushButton* btn = new QPushButton(label.c_str(), this);
        btn->setSizePolicy(ui->deviceTemplate_pushButton->sizePolicy());
        btn->setMinimumSize(ui->deviceTemplate_pushButton->minimumSize());
        btn->setMaximumSize(ui->deviceTemplate_pushButton->maximumSize());
        btn->setCheckable(true);
        btn->setAutoExclusive(true);
        btn->setStyleSheet(common->css.Device_ButtonStyle);

        qButtonGroup->addButton(btn);
        btn->setProperty("index", i);
        QString str = it->second.channel_id.c_str();
        btn->setProperty("channel_id", str);
        connect(btn, SIGNAL(pressed()), this, SLOT(btn_clicked()));
        ui->devices_horizontalLayout->addWidget(btn);
        devices.push_back(btn);
        if(i == dev_index)
            btn->setStyleSheet(common->css.Select_Device_ButtonStyle);
            btn->setDown(true);
        i++;
    }
}
void Tab_Utilities_alarmHistoryPage_Widget::btn_clicked()
{
    QPushButton* s = (QPushButton*)sender();
    dev_index = s->property("index").value<int>();
    for(int i=0;i<(int)devices.size();i++)
        devices[i]->setDown(false);
    s->setDown(true);
    ui->queryResult_tableWidget->clearContents();

}

Tab_Utilities_alarmHistoryPage_Widget::~Tab_Utilities_alarmHistoryPage_Widget()
{
    delete ui;
}
void Tab_Utilities_alarmHistoryPage_Widget::on_DatecomboBox_currentIndexChanged(int index)
{
    index_u8 = ui->DatecomboBox->currentIndex();

}
void Tab_Utilities_alarmHistoryPage_Widget::on_query_pushButton_clicked()
{
    ui->query_pushButton->setEnabled(false);
    QTimer::singleShot(500, this,  [=](){
        ui->query_pushButton->setEnabled(true);
});
    ui->queryResult_tableWidget->clearContents();
    Common* common = Common::instance();
    if((dev_index >= (int)devices.size()) || (common->patient_id.size() == 0))
        {
        return;
        }
    if(1==0/*common->is_server*/)
    {
        time_t now = time(NULL);
        time_t start_time = 0;
        if(index_u8 == 0)
            start_time = now - 8*60*60;
        else if(index_u8 == 1)
            start_time = now - 16*60*60;
        else if(index_u8 == 2)
            start_time = now - 24*60*60;
        else if(index_u8 == 3)
            start_time = now - 48*60*60;
        char timebuf[128];
        char timebuf1[128];
        sprintf(timebuf1, "%u", (uint32_t)start_time);
        sprintf(timebuf, "%u", (uint32_t)now);
        std::multimap<uint64_t, QJsonArray> vals;
        QJsonArray array =common->Restful_API(timebuf,timebuf1,"RTObservation");
    }
    else
    {
        std::string dummy;
        std::string sql = "SELECT alarm_no,source_timestamp.sec,alarm_description FROM _ WHERE data_source='";
        if(is_technical)
            sql.append("TechnicalAlert");
        else
            sql.append("PatientAlert");
        sql.append("' AND channel_id='");
        sql.append(devices[dev_index]->property("channel_id").value<QString>().toStdString());
        sql.append("' AND vmd_id='");
        sql.append(common->vmd_id);
        sql.append("' AND patient_id='");
        sql.append(common->patient_id);
        sql.append("' AND source_timestamp.sec>=");
        time_t now = time(NULL);
        time_t start_time = 0;
        if(index_u8 == 0)
            start_time = now - 8*60*60;
        else if(index_u8 == 1)
            start_time = now - 16*60*60;
        else if(index_u8 == 2)
            start_time = now - 24*60*60;
        else if(index_u8 == 3)
            start_time = now - 48*60*60;

        char buf[128];
        sprintf(buf, "%u", (uint32_t)start_time);
        sql.append(buf);
        sql.append(" ORDER BY ");
        if(order == 0)
        {
            sql.append("source_timestamp.sec DESC");
            sql.append(",alarm_no");
        }
        else
            {
            sql.append("alarm_description");
            sql.append(",source_timestamp.sec DESC");
            sql.append(",alarm_no");
            }
        cbl::ResultSet results = common->cbl->queryDocuments(common->db, sql, dummy);
        int row=0;
        //philo
        char sumbuf[128];
        sprintf(sumbuf, "%u", (uint32_t)start_time);
        sql.append(sumbuf);
        time_t old_timestamp,old_timestamp_2,sum,old_sum =0;
        int dur_index=1,old_dur_index; //philo
        for(auto& result: results)
        {
            ui->queryResult_tableWidget->setRowCount(row+1);
            std::string alarm_no = result.valueAtIndex(0).asstring();
            time_t timestamp = (time_t)result.valueAtIndex(1).asUnsigned();
            std::string msg = result.valueAtIndex(2).asstring();
            struct tm t;
            localtime_r(&timestamp, &t);
            sprintf(buf, "%d-%d-%d %02d:%02d:%02d", t.tm_year+1900, t.tm_mon+1,
                    t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
            int col = 0;
            QTableWidgetItem* item1 = new QTableWidgetItem(alarm_no.c_str());
            item1->setFlags(Qt::ItemIsEnabled);
            ui->queryResult_tableWidget->setItem(row, col++, item1);
            QTableWidgetItem* item2 = new QTableWidgetItem(buf);
            item2->setFlags(Qt::ItemIsEnabled);
            ui->queryResult_tableWidget->setItem(row, col++, item2);
            QTableWidgetItem* item3 = new QTableWidgetItem(msg.c_str());
            item3->setFlags(Qt::ItemIsEnabled);
            ui->queryResult_tableWidget->setItem(row, col, item3);
            //================訊息確認//philo
            QString new_message = ui->queryResult_tableWidget->item(row,col)->text();
            QString old_message,old_message_2;
            if(row>0)
                old_message = ui->queryResult_tableWidget->item(row-1,col)->text();
            if(row>1)
                old_message_2 = ui->queryResult_tableWidget->item(row-2,col)->text();
            if(QString::compare(new_message,old_message,Qt::CaseInsensitive)== 0)
            {

                if(old_timestamp - timestamp > 6)
                {
                    sum=0;
                    dur_index++;
                }
                else

                    {
                    sum+= old_timestamp - timestamp;
                    old_sum=sum;
                    }
            }
            else
               dur_index=1,old_dur_index=0,sum=0;
            col++;
            //=================Duration運算
            struct tm old_t;
            localtime_r(&sum, &old_t);
            sprintf(sumbuf, "%02d:%02d:%02d",old_t.tm_hour-8, old_t.tm_min, old_t.tm_sec);
            QTableWidgetItem* item4 = new QTableWidgetItem(QString::number(dur_index));
            item4->setFlags(Qt::ItemIsEnabled);
            ui->queryResult_tableWidget->setItem(row, col, item4);

            if (row>0) //=========刪除判定
            {
                int Y =QString::compare(new_message,old_message,Qt::CaseInsensitive);
                if ( dur_index ==old_dur_index && Y ==0)
                {
                    ui->queryResult_tableWidget->removeRow(--row);
                    ui->queryResult_tableWidget->item(row,3)->setText(sumbuf);
                    old_timestamp_2=old_timestamp;
                    old_timestamp = timestamp,old_dur_index = dur_index;
                    row++;
                    continue;
                }
                else
                {
                    ui->queryResult_tableWidget->item(row,3)->setText(sumbuf);
                };
            }
            else
                ui->queryResult_tableWidget->item(row,3)->setText(sumbuf);
            int i;
            QDateTime old_time = QDateTime::fromString(ui->queryResult_tableWidget->item(row,1)->text(),"yyyy-M-d hh:mm:ss");
                for (i=row-1,dur_index==1;old_timestamp-timestamp<=6,i>=0;i--)
                {
                    old_message =ui->queryResult_tableWidget->item(i,2)->text();
                    int Y =QString::compare(new_message,old_message,Qt::CaseInsensitive);
                    QDateTime time = QDateTime::fromString(ui->queryResult_tableWidget->item(i,1)->text(),"yyyy-M-d hh:mm:ss");
                    QDateTime dur = QDateTime::fromString(ui->queryResult_tableWidget->item(i,3)->text(),"hh:mm:ss");
                    if (old_time.secsTo(time)>6)
                        break;
                    if(Y!=0)
                        continue;
                    sum+=old_time.secsTo(time)+dur.toSecsSinceEpoch()+2209017600;//+2209017600是GMT+8
                    ui->queryResult_tableWidget->removeRow(i);
                    row--;
                    localtime_r(&sum, &old_t);
                    sprintf(sumbuf, "%02d:%02d:%02d",old_t.tm_hour-8, old_t.tm_min, old_t.tm_sec);
                    ui->queryResult_tableWidget->item(row,3)->setText(sumbuf);
            }

            old_timestamp_2=old_timestamp;
            old_timestamp = timestamp,old_dur_index = dur_index;
            row++;
        }
        if(order == 0)
            ui->queryResult_tableWidget->sortByColumn(1,Qt::DescendingOrder);
        for (int i=0;i<row;i++) //偶數列塗色
        {
            if(i % 2 ==1)
                {
                ui->queryResult_tableWidget->item(i,0)->setBackground(QBrush(QColor(11, 42, 78)));
                ui->queryResult_tableWidget->item(i,1)->setBackground(QBrush(QColor(11, 42, 78)));
                ui->queryResult_tableWidget->item(i,2)->setBackground(QBrush(QColor(11, 42, 78)));
                ui->queryResult_tableWidget->item(i,3)->setBackground(QBrush(QColor(11, 42, 78)));
            }
            QDateTime Duration = QDateTime::fromString(ui->queryResult_tableWidget->item(i,3)->text(),"hh:mm:ss");
            if(Duration.toSecsSinceEpoch()== -2209017600)
                {
                ui->queryResult_tableWidget->item(i,3)->setText("00:00:01");
                }
        }
    }
}

void Tab_Utilities_alarmHistoryPage_Widget::on_orderType_comboBox_currentIndexChanged(int index)
{
    order = index;
}
